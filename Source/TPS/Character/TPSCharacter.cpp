// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TPSInventoryComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Game/TPSGameInstance.h"
#include "Engine/World.h"

ATPSCharacter::ATPSCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	InventoryComponent = CreateDefaultSubobject<UTPSInventoryComponent>(TEXT("InvenctoryComponent"));
	if (InventoryComponent)
	{
		InventoryComponent->OnSwitchWeapon.AddDynamic(this, &ATPSCharacter::InitWeapon);
	}

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ATPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(InventoryComponent);

	if (CursorMaterial)
	{
		CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
	}
}

void ATPSCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (CurrentCursor)
	{
		APlayerController* myPC = Cast<APlayerController>(GetController());
		if (myPC)
		{
			FHitResult TraceHitResult;
			myPC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();

			CurrentCursor->SetWorldLocation(TraceHitResult.Location);
			CurrentCursor->SetWorldRotation(CursorR);
		}
	}

	MovementTick(DeltaSeconds);
}

void ATPSCharacter::SetupPlayerInputComponent(UInputComponent* NewInputComponent)
{
	Super::SetupPlayerInputComponent(NewInputComponent);

	NewInputComponent->BindAxis(TEXT("MoveForward"), this, &ATPSCharacter::InputAxisX);
	NewInputComponent->BindAxis(TEXT("MoveRight"), this, &ATPSCharacter::InputAxisY);

	NewInputComponent->BindAction(TEXT("FireEvent"), EInputEvent::IE_Pressed, this, &ATPSCharacter::InputAttackPressed);
	NewInputComponent->BindAction(TEXT("FireEvent"), EInputEvent::IE_Released, this, &ATPSCharacter::InputAttackReleased);
	NewInputComponent->BindAction(TEXT("ReloadEvent"), EInputEvent::IE_Released, this, &ATPSCharacter::TryReloadWeapon);

	NewInputComponent->BindAction(TEXT("SwitchNextWeapon"), EInputEvent::IE_Pressed, this, &ATPSCharacter::TrySwitchNextWeapon);
	NewInputComponent->BindAction(TEXT("SwitchPreviousWeapon"), EInputEvent::IE_Pressed, this, &ATPSCharacter::TrySwitchPreviousWeapon);

}

UDecalComponent* ATPSCharacter::GetCursorToWorld()
{
	return CurrentCursor;
}

void ATPSCharacter::InputAxisY(float Value)
{
	AxisY = Value;
}

void ATPSCharacter::InputAxisX(float Value)
{
	AxisX = Value;
}

void ATPSCharacter::InputAttackPressed()
{
	AttackCharEvent(true);
}

void ATPSCharacter::InputAttackReleased()
{
	AttackCharEvent(false);
}

void ATPSCharacter::MovementTick(float DeltaTime)
{
	if (SprintRunEnabled && (AxisY != 0 || AxisX != 0))
	{
		AddMovementInput(GetActorForwardVector());
		DecreaseStamina();
	}
	else
	{
		AddMovementInput(FVector(1.0f, 0.0f, 0.0f), AxisX);
		AddMovementInput(FVector(0.0f, 1.0f, 0.0f), AxisY);
	}

	if (!SprintRunEnabled)
	{
		IncreaseStamina();
	}

	APlayerController* myController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (myController)
	{
		FHitResult ResultHit;
		//myController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery6, false, ResultHit);
		myController->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);
		float Yaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
		SetActorRotation(FQuat(FRotator(0.0f, Yaw, 0.0f)));

		if (CurrentWeapon)
		{
			FVector Displacement = FVector(0);
			switch (MovementState)
			{
			case EMovementState::Aim_State:
				Displacement = FVector(0.0f, 0.0f, 160.0f);
				CurrentWeapon->ShouldReduceDispersion = true;
				break;
			case EMovementState::AimWalk_State:
				Displacement = FVector(0.0f, 0.0f, 160.0f);
				CurrentWeapon->ShouldReduceDispersion = true;
				break;
			case EMovementState::Walk_State:
				Displacement = FVector(0.0f, 0.0f, 120.0f);
				CurrentWeapon->ShouldReduceDispersion = false;
				break;
			case EMovementState::Run_State:
				Displacement = FVector(0.0f, 0.0f, 120.0f);
				CurrentWeapon->ShouldReduceDispersion = false;
				break;
			case EMovementState::SprintRun_State:
				break;
			default:
				break;
			}
			CurrentWeapon->ShootEndLocation = ResultHit.Location + Displacement;
		}
	}
}

void ATPSCharacter::CharacterUpdate()
{
	float ResSpeed = 400.0f;
	switch (MovementState)
	{
	case EMovementState::Aim_State:
		ResSpeed = MovementSpeedInfo.AimSpeedNormal;
		break;
	case EMovementState::AimWalk_State:
		ResSpeed = MovementSpeedInfo.AimSpeedWalk;
		break;
	case EMovementState::Walk_State:
		ResSpeed = MovementSpeedInfo.WalkSpeedNormal;
		break;
	case EMovementState::Run_State:
		ResSpeed = MovementSpeedInfo.RunSpeedNormal;
		break;
	case EMovementState::SprintRun_State:
		ResSpeed = MovementSpeedInfo.SprintRunSpeedRun;
		break;
	default:
		break;
	}

	GetCharacterMovement()->MaxWalkSpeed = ResSpeed;
}

void ATPSCharacter::ChangeMovementState()
{
	if (!WalkEnabled && !SprintRunEnabled && !AimEnabled)
	{
		MovementState = EMovementState::Run_State;
	}
	else
	{
		if (SprintRunEnabled && !FMath::IsNearlyZero(CurrentStamina))
		{
			WalkEnabled = false;
			AimEnabled = false;
			MovementState = EMovementState::SprintRun_State;
		}
		if (WalkEnabled && !SprintRunEnabled && AimEnabled)
		{
			MovementState = EMovementState::AimWalk_State;
		}
		else
		{
			if (WalkEnabled && !SprintRunEnabled && !AimEnabled)
			{
				MovementState = EMovementState::Walk_State;
			}
			else
			{
				if (!WalkEnabled && !SprintRunEnabled && AimEnabled)
				{
					MovementState = EMovementState::Aim_State;
				}
			}
		}
	}
	CharacterUpdate();

	AWeaponDefault* myWeapon = GetCurrentWeapon();
	if (myWeapon)
	{
		myWeapon->UpdateStateWeapon(MovementState);
	}
}

void ATPSCharacter::DecreaseStamina()
{
	if (CurrentStamina > 0 && SprintRunEnabled)
	{
		CurrentStamina -= MinusStamina;
		if (CurrentStamina <= 0)
		{
			SprintRunEnabled = false;
			ChangeMovementState();
		}
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Stamina: %f"), CurrentStamina));
}

void ATPSCharacter::IncreaseStamina()
{
	if (CurrentStamina < StaminaPoints && !SprintRunEnabled)
	{
		CurrentStamina += PlusStamina;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Stamina: %f"), CurrentStamina));
}

void ATPSCharacter::AttackCharEvent(bool bIsFiring)
{
	AWeaponDefault* myWeapon = nullptr;
	myWeapon = GetCurrentWeapon();
	if (myWeapon)
	{
		if (myWeapon->WeaponReloading == false)
		{
			myWeapon->SetWeaponStateFire(bIsFiring);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ATPSCharacter::AttackCharEvent - CurrentWeapon - NULL"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("ATPSCharacter::AttackCharEvent - CurrentWeapon - NULL ")));
	}
}

AWeaponDefault* ATPSCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}

int32 ATPSCharacter::GetCurrentWeaponIndex()
{
	return CurrentIndexWeapon;
}

void ATPSCharacter::InitWeapon(FName IdWeapon, FAdditionalWeaponInfo AdditionalWeaponInfo, int32 NewCurrentIndexWeapon)
{
	if (CurrentWeapon) 
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}

	UTPSGameInstance* myGI = Cast<UTPSGameInstance>(GetGameInstance());
	FWeaponInfo myWeaponInfo;
	if (myGI)
	{
		if (myGI->GetWeaponInfoByName(IdWeapon, myWeaponInfo))
		{
			if (myWeaponInfo.WeaponClass)
			{
				FVector SpawnLocation = FVector(0);
				FRotator SpawnRotation = FRotator(0);

				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = GetInstigator();

				AWeaponDefault* myWeapon = Cast<AWeaponDefault>(GetWorld()->SpawnActor(myWeaponInfo.WeaponClass, &SpawnLocation, &SpawnRotation, SpawnParams));
				if (myWeapon)
				{
					FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
					myWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketRightHand"));
					CurrentWeapon = myWeapon;

					myWeapon->WeaponSettings = myWeaponInfo;
					myWeapon->WeaponInfo = AdditionalWeaponInfo;
					//Remove !!! Debug
					myWeapon->ReloadTime = myWeaponInfo.ReloadTime;
					myWeapon->UpdateStateWeapon(MovementState);
					
					if (InventoryComponent)
					{
						CurrentIndexWeapon = NewCurrentIndexWeapon;
					}

					myWeapon->OnWeaponFireStart.AddDynamic(this, &ATPSCharacter::CharFire);

					myWeapon->OnWeaponReloadStart.AddDynamic(this, &ATPSCharacter::WeaponReloadStart);
					myWeapon->OnWeaponReloadEnd.AddDynamic(this, &ATPSCharacter::WeaponReloadEnd);

					if (CurrentWeapon->GetWeaponRound() <= 0 && CurrentWeapon->CheckCanWeaponReload())
					{
						CurrentWeapon->InitReload();
					}

					if (InventoryComponent)
					{
						InventoryComponent->OnWeaponAmmoAvailable.Broadcast(myWeapon->WeaponSettings.WeaponType);
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ATPSCharacter::InitWeapon - Weapon not found in table - NULL"));
		}
	}
}

void ATPSCharacter::RemoveCurrentWeapon()
{

}


void ATPSCharacter::WeaponReloadStart(UAnimMontage* Anim)
{
	WeaponReloadStart_BP(Anim);
}

void ATPSCharacter::WeaponReloadEnd(bool bIsSuccess, int32 AmmoTake)
{
	if (InventoryComponent)
	{
		InventoryComponent->AmmoSlotChangeValue(CurrentWeapon->WeaponSettings.WeaponType, -AmmoTake);
		InventoryComponent->SetAdditionalInfoWeapon(CurrentIndexWeapon, CurrentWeapon->WeaponInfo);
	}
	WeaponReloadEnd_BP(bIsSuccess);
}

void ATPSCharacter::WeaponReloadStart_BP_Implementation(UAnimMontage* Anim)
{
	//in BP
}

void ATPSCharacter::WeaponReloadEnd_BP_Implementation(bool bIsSuccess)
{
	//in BP
}

void ATPSCharacter::CharFire(UAnimMontage* Anim)
{
	if (InventoryComponent && CurrentWeapon)
	{
		InventoryComponent->SetAdditionalInfoWeapon(CurrentIndexWeapon, CurrentWeapon->WeaponInfo);
	}
	CharFire_BP(Anim);
}

void ATPSCharacter::CharFire_BP_Implementation(UAnimMontage* Anim)
{
	//in BP
}

void ATPSCharacter::TryReloadWeapon()
{
	if (CurrentWeapon)
	{
		if (CurrentWeapon->GetWeaponRound() < CurrentWeapon->WeaponSettings.MaxRound 
			&& CurrentWeapon->CheckCanWeaponReload())
		{
			CurrentWeapon->InitReload();
		}
	}
}


void ATPSCharacter::TrySwitchNextWeapon()
{
	if (InventoryComponent->WeaponSlots.Num() > 1)
	{
		int8 OldIndex = CurrentIndexWeapon;
		FAdditionalWeaponInfo OldInfo;
		if (CurrentWeapon)
		{
			OldInfo = CurrentWeapon->WeaponInfo;
			if (CurrentWeapon->WeaponReloading)
			{
				CurrentWeapon->CancelReload();
			}
		}

		if (InventoryComponent)
		{
			if (InventoryComponent->SwitchWeaponToIndex(CurrentIndexWeapon + 1, OldIndex, OldInfo, true))
			{
				
			}
		}
	}
}

void ATPSCharacter::TrySwitchPreviousWeapon()
{
	if (InventoryComponent->WeaponSlots.Num() > 1)
	{
		int8 OldIndex = CurrentIndexWeapon;
		FAdditionalWeaponInfo OldInfo;
		if (CurrentWeapon)
		{
			OldInfo = CurrentWeapon->WeaponInfo;
			if (CurrentWeapon->WeaponReloading)
			{
				CurrentWeapon->CancelReload();
			}
		}

		if (InventoryComponent)
		{
			if (InventoryComponent->SwitchWeaponToIndex(CurrentIndexWeapon - 1, OldIndex, OldInfo, false))
			{

			}
		}
	}
}