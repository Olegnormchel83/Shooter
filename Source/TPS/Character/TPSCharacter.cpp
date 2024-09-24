// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TPSInventoryComponent.h"
#include "Components/TPSCharacterHelathComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Game/TPSGameInstance.h"
#include "Weapons/Projectiles/ProjectileDefault.h"
#include "Engine/World.h"

DEFINE_LOG_CATEGORY_STATIC(CharacterLog, All, All);

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
	HealthComponent = CreateDefaultSubobject<UTPSCharacterHealthComponent>(TEXT("HealthComponent"));

	if (HealthComponent)
	{
		HealthComponent->OnDead.AddDynamic(this, &ATPSCharacter::CharDead);
	}


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

	NewInputComponent->BindAction(TEXT("ChangeToSprint"), EInputEvent::IE_Pressed, this, &ATPSCharacter::InputSprintPressed);
	NewInputComponent->BindAction(TEXT("ChangeToWalk"), EInputEvent::IE_Pressed, this, &ATPSCharacter::InputWalkPressed);
	NewInputComponent->BindAction(TEXT("AimEvent"), EInputEvent::IE_Pressed, this, &ATPSCharacter::InputAimPressed);
	NewInputComponent->BindAction(TEXT("ChangeToSprint"), EInputEvent::IE_Released, this, &ATPSCharacter::InputSprintReleased);
	NewInputComponent->BindAction(TEXT("ChangeToWalk"), EInputEvent::IE_Released, this, &ATPSCharacter::InputWalkReleased);
	NewInputComponent->BindAction(TEXT("AimEvent"), EInputEvent::IE_Released, this, &ATPSCharacter::InputAimReleased);

	NewInputComponent->BindAction(TEXT("FireEvent"), EInputEvent::IE_Pressed, this, &ATPSCharacter::InputAttackPressed);
	NewInputComponent->BindAction(TEXT("FireEvent"), EInputEvent::IE_Released, this, &ATPSCharacter::InputAttackReleased);
	NewInputComponent->BindAction(TEXT("ReloadEvent"), EInputEvent::IE_Released, this, &ATPSCharacter::TryReloadWeapon);

	NewInputComponent->BindAction(TEXT("SwitchNextWeapon"), EInputEvent::IE_Pressed, this, &ATPSCharacter::TrySwitchNextWeapon);
	NewInputComponent->BindAction(TEXT("SwitchPreviousWeapon"), EInputEvent::IE_Pressed, this, &ATPSCharacter::TrySwitchPreviousWeapon);

	NewInputComponent->BindAction(TEXT("AbilityAction"), EInputEvent::IE_Pressed, this, &ATPSCharacter::TryAbilityEnabled);

	NewInputComponent->BindAction(TEXT("DropCurrentWeapon"), EInputEvent::IE_Pressed, this, &ATPSCharacter::DropCurrentWeapon);
	
	TArray<FKey> HotKeys;
	HotKeys.Add(EKeys::One);
	HotKeys.Add(EKeys::Two);
	HotKeys.Add(EKeys::Three);
	HotKeys.Add(EKeys::Four);
	HotKeys.Add(EKeys::Five);
	HotKeys.Add(EKeys::Six);
	HotKeys.Add(EKeys::Seven);
	HotKeys.Add(EKeys::Eight);
	HotKeys.Add(EKeys::Nine);
	HotKeys.Add(EKeys::Zero);

	NewInputComponent->BindKey(HotKeys[1], IE_Pressed, this, &ATPSCharacter::TKeyPressed<1>);
	NewInputComponent->BindKey(HotKeys[2], IE_Pressed, this, &ATPSCharacter::TKeyPressed<2>);
	NewInputComponent->BindKey(HotKeys[3], IE_Pressed, this, &ATPSCharacter::TKeyPressed<3>);
	NewInputComponent->BindKey(HotKeys[4], IE_Pressed, this, &ATPSCharacter::TKeyPressed<4>);
	NewInputComponent->BindKey(HotKeys[5], IE_Pressed, this, &ATPSCharacter::TKeyPressed<5>);
	NewInputComponent->BindKey(HotKeys[6], IE_Pressed, this, &ATPSCharacter::TKeyPressed<6>);
	NewInputComponent->BindKey(HotKeys[7], IE_Pressed, this, &ATPSCharacter::TKeyPressed<7>);
	NewInputComponent->BindKey(HotKeys[8], IE_Pressed, this, &ATPSCharacter::TKeyPressed<8>);
	NewInputComponent->BindKey(HotKeys[9], IE_Pressed, this, &ATPSCharacter::TKeyPressed<9>);
	NewInputComponent->BindKey(HotKeys[0], IE_Pressed, this, &ATPSCharacter::TKeyPressed<0>);

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

void ATPSCharacter::InputWalkPressed()
{
	WalkEnabled = true;
	ChangeMovementState();
}

void ATPSCharacter::InputWalkReleased()
{
	WalkEnabled = false;
	ChangeMovementState();
}

void ATPSCharacter::InputSprintPressed()
{
	SprintRunEnabled = true;
	ChangeMovementState();
}

void ATPSCharacter::InputSprintReleased()
{
	SprintRunEnabled = false;
	ChangeMovementState();
}

void ATPSCharacter::InputAimPressed()
{
	AimEnabled = true;
	ChangeMovementState();
}

void ATPSCharacter::InputAimReleased()
{
	AimEnabled = false;
	ChangeMovementState();
}

void ATPSCharacter::MovementTick(float DeltaTime)
{
	if (!bIsAlive) return;

	if (Stunned) return;
	
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
			case EMovementState::Stun_State:
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
		ResSpeed = MovementSpeedInfo.AimSpeedNormal * MoveSpeedCoef;
		//UE_LOG(CharacterLog, Display, TEXT("Character State - Aim"));
		break;
	case EMovementState::AimWalk_State:
		ResSpeed = MovementSpeedInfo.AimSpeedWalk * MoveSpeedCoef;
		//UE_LOG(CharacterLog, Display, TEXT("Character State - AimWalk"));
		break;
	case EMovementState::Walk_State:
		ResSpeed = MovementSpeedInfo.WalkSpeedNormal * MoveSpeedCoef;
		//UE_LOG(CharacterLog, Display, TEXT("Character State - Walk"));
		break;
	case EMovementState::Run_State:
		ResSpeed = MovementSpeedInfo.RunSpeedNormal * MoveSpeedCoef;
		//UE_LOG(CharacterLog, Display, TEXT("Character State - Run"));
		break;
	case EMovementState::SprintRun_State:
		ResSpeed = MovementSpeedInfo.SprintRunSpeedRun * MoveSpeedCoef;
		//UE_LOG(CharacterLog, Display, TEXT("Character State - Sprint"));
		break;
	case EMovementState::Stun_State:
		ResSpeed = MovementSpeedInfo.StunStateSpeed * MoveSpeedCoef;
		//UE_LOG(CharacterLog, Display, TEXT("Character State - Stun"));
	default:
		break;
	}

	GetCharacterMovement()->MaxWalkSpeed = ResSpeed;
	//UE_LOG(CharacterLog, Display, TEXT("Character Speed - %.1f"), ResSpeed);
}

void ATPSCharacter::ChangeMovementState()
{
	if (Stunned)
	{
		WalkEnabled = false;
		AimEnabled = false;
		SprintRunEnabled = false;

		MovementState = EMovementState::Stun_State;
	}

	if (!WalkEnabled && !SprintRunEnabled && !AimEnabled && !Stunned)
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
		if (WalkEnabled && !SprintRunEnabled && AimEnabled && !Stunned)
		{
			MovementState = EMovementState::AimWalk_State;
		}
		else
		{
			if (WalkEnabled && !SprintRunEnabled && !AimEnabled && !Stunned)
			{
				MovementState = EMovementState::Walk_State;
			}
			else
			{
				if (!WalkEnabled && !SprintRunEnabled && AimEnabled && !Stunned)
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
	if (CurrentStamina < MaxStaminaPoints && !SprintRunEnabled)
	{
		CurrentStamina += PlusStamina;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Stamina: %f"), CurrentStamina));
}

void ATPSCharacter::AttackCharEvent(bool bIsFiring)
{
	if (Stunned) return;

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

void ATPSCharacter::InitWeapon(FName IdWeapon, FAdditionalWeaponInfo AdditionalWeaponInfo, int32 NewCurrentIndexWeapon)
{
	RemoveCurrentWeapon();

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

					myWeapon->OnWeaponFireStart.AddDynamic(this, &ATPSCharacter::WeaponFireStart);

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
	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}
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

bool ATPSCharacter::TrySwitchWeaponToIndexByKeyInput(int32 ToIndex)
{
	bool bIsSuccess = false;
	if (!InventoryComponent) return false;
	if (!CurrentWeapon) return false;
	if (!CurrentWeapon->WeaponReloading && InventoryComponent->WeaponSlots.IsValidIndex(ToIndex))
	{
		if (CurrentIndexWeapon != ToIndex)
		{
			int32 OldIndex = CurrentIndexWeapon;
			FAdditionalWeaponInfo OldInfo;

			OldInfo = CurrentWeapon->WeaponInfo;
			if (CurrentWeapon->WeaponReloading)
			{
				CurrentWeapon->CancelReload();
			}

			bIsSuccess = InventoryComponent->SwitchWeaponByIndex(ToIndex, OldIndex, OldInfo);
		}
	}

	return bIsSuccess;
}

void ATPSCharacter::WeaponReloadStart_BP_Implementation(UAnimMontage* Anim)
{
	//in BP
}

void ATPSCharacter::WeaponReloadEnd_BP_Implementation(bool bIsSuccess)
{
	//in BP
}

void ATPSCharacter::WeaponFireStart(UAnimMontage* Anim)
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
			if (InventoryComponent->SwitchWeaponToNextPreviousIndex(CurrentIndexWeapon + 1, OldIndex, OldInfo, true))
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
			if (InventoryComponent->SwitchWeaponToNextPreviousIndex(CurrentIndexWeapon - 1, OldIndex, OldInfo, false))
			{

			}
		}
	}
}

void ATPSCharacter::DropCurrentWeapon() //TODO: Add swap to next weapon in inventory
{
	if (InventoryComponent)
	{
		FDropItem ItemInfo;
		InventoryComponent->DropWeaponByIndex(CurrentIndexWeapon, ItemInfo);
	}
}

void ATPSCharacter::CharDead()
{
	int32 rnd = FMath::RandHelper(DeadAnims.Num());
	float AnimTime = 0.0f;
	if (DeadAnims.IsValidIndex(rnd) && DeadAnims[rnd] && GetMesh() && GetMesh()->GetAnimInstance())
	{
		AnimTime = DeadAnims[rnd]->GetPlayLength();
		GetMesh()->GetAnimInstance()->Montage_Play(DeadAnims[rnd]);
	}

	bIsAlive = false;

	UnPossessed();

	//Timer ragdoll
	GetWorldTimerManager().SetTimer(TimerHadle_RagdollTimer, this, &ATPSCharacter::EnableRagdoll, AnimTime, false);

	GetCursorToWorld()->SetVisibility(false);
}

void ATPSCharacter::TryAbilityEnabled()
{
	if (AbilityEffect)
	{
		UTPS_StateEffect* NewEffect = NewObject<UTPS_StateEffect>(this, AbilityEffect);
		if (NewEffect)
		{
			NewEffect->InitObject(this);
		}
	}
}

float ATPSCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if (bIsAlive)
	{
		HealthComponent->ChangeHealthValue(-DamageAmount);
	}

	if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		AProjectileDefault* myProjectile = Cast<AProjectileDefault>(DamageCauser);
		if (myProjectile)
		{
			UTypes::AddEffectBySurfaceType(this, myProjectile->ProjectileSettings.Effect, GetSurfaceType());
		}
	}

	/*
	const auto* myInterface = Cast<ITPS_IGameActor>(Hit.GetActor());
	if (myInterface)
	{
		UTypes::AddEffectBySurfaceType(Hit.GetActor(), ProjectileInfo.Effect, mySurfacetype);
	}
	*/

	return ActualDamage;
}

void ATPSCharacter::EnableRagdoll()
{
	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetMesh()->SetSimulatePhysics(true);
	}
}

void ATPSCharacter::GetStun()
{
	Stunned = true;

	AWeaponDefault* myWeapon = GetCurrentWeapon();
	if (myWeapon)
	{
		myWeapon->CancelReload();
	}
	
	ChangeMovementState();
	InputAttackReleased();
	PlayAnimMontage(StunAnimation);
	GetCharacterMovement()->StopMovementImmediately();
	DisableInput(Cast<APlayerController>(GetController()));
}

void ATPSCharacter::StunOut()
{
	Stunned = false;

	StopAnimMontage(StunAnimation);
	ChangeMovementState();
	EnableInput(Cast<APlayerController>(GetController()));
}

FVector ATPSCharacter::GetParticleOffset()
{
	return ParticleOffset;
}

EPhysicalSurface ATPSCharacter::GetSurfaceType()
{
	EPhysicalSurface Result = EPhysicalSurface::SurfaceType_Default;
	if (HealthComponent->GetCurrentShield() <= 0)
	{
		if (GetMesh())
		{
			UMaterialInterface* myMaterial = GetMesh()->GetMaterial(0);
			if (myMaterial)
			{
				Result = myMaterial->GetPhysicalMaterial()->SurfaceType;
			}
		}
	}

	return Result;
}

TArray<UTPS_StateEffect*> ATPSCharacter::GetAllCurrentEffects()
{
	return Effects;
}

void ATPSCharacter::RemoveEffect(UTPS_StateEffect* RemovedEffect)
{
	Effects.Remove(RemovedEffect);
}

void ATPSCharacter::AddEffect(UTPS_StateEffect* NewEffect)
{
	Effects.Add(NewEffect);
}
