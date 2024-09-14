// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponDefault.h"
#include "Engine/StaticMeshActor.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TPSInventoryComponent.h"
#include "StateEffects/TPS_StateEffect.h"

// Sets default values
AWeaponDefault::AWeaponDefault()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = SceneComponent;

	SkeletalMeshWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	SkeletalMeshWeapon->SetGenerateOverlapEvents(false);
	SkeletalMeshWeapon->SetCollisionProfileName(TEXT("NoCollision"));
	SkeletalMeshWeapon->SetupAttachment(RootComponent);

	StaticMeshWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	StaticMeshWeapon->SetGenerateOverlapEvents(false);
	StaticMeshWeapon->SetCollisionProfileName(TEXT("NoCollision"));
	StaticMeshWeapon->SetupAttachment(RootComponent);

	ShootLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("ShootLocation"));
	ShootLocation->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AWeaponDefault::BeginPlay()
{
	Super::BeginPlay();
	WeaponInit();
}

// Called every frame
void AWeaponDefault::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FireTick(DeltaTime);
	ReloadTick(DeltaTime);
	DispersionTick(DeltaTime);
	ShellDropTick(DeltaTime);
	ClipDropTick(DeltaTime);

	if (FireDelay <= 4.0f)
	{
		FireDelay += DeltaTime;
	}
}

void AWeaponDefault::FireTick(float DeltaTime)
{
	if (GetWeaponRound() > 0)
	{
		if (WeaponFiring)
		{
			if (!WeaponReloading)
			{
				if (FireTimer < FireDelay)
				{
					Fire();
					FireDelay = 0;
				}
			}
		}
	}
	else
	{
		if (!WeaponReloading && CheckCanWeaponReload())
		{
			InitReload();
		}
	}
}

void AWeaponDefault::ReloadTick(float DeltaTime)
{
	if (WeaponReloading)
	{
		if (ReloadTimer < 0.0f)
		{
			FinishReload();
		}
		else
		{
			ReloadTimer -= DeltaTime;
		}
	}
}

void AWeaponDefault::DispersionTick(float DeltaTime)
{
	if (!WeaponReloading)
	{
		if (!WeaponFiring)
		{
			if (ShouldReduceDispersion)
			{
				CurrentDispersion -= CurrentDispersionReduction;
			}
			else
			{
				CurrentDispersion += CurrentDispersionReduction;
			}
		}

		if (CurrentDispersion < CurrentDispersionMin)
		{
			CurrentDispersion = CurrentDispersionMin;
		}
		else
		{
			if (CurrentDispersion > CurrentDispersionMax)
			{
				CurrentDispersion = CurrentDispersionMax;
			}
		}
	}
	if (ShowDebug)
	{
		UE_LOG(LogTemp, Warning, TEXT("Dispersion: MAX = %f. MIN = %f. Current = %f"), CurrentDispersionMax, CurrentDispersionMin, CurrentDispersion);
	}
}

void AWeaponDefault::ClipDropTick(float DeltaTime)
{
	if (DropClipFlag)
	{
		if (DropClipTimer < 0.0f)
		{
			DropClipFlag = false;
			InitDropMesh(WeaponSettings.ClipDropMesh.DropMesh,
				WeaponSettings.ClipDropMesh.DropMeshOffset,
				WeaponSettings.ClipDropMesh.DropMeshImpulseDir,
				WeaponSettings.ClipDropMesh.DropMeshLifeTime,
				WeaponSettings.ClipDropMesh.ImpulseRandomDispersion,
				WeaponSettings.ClipDropMesh.PowerImpulse,
				WeaponSettings.ClipDropMesh.CustomMass);
		}
		else
		{
			DropClipTimer -= DeltaTime;
		}
	}
}

void AWeaponDefault::ShellDropTick(float DeltaTime)
{
	if (DropShellFlag)
	{
		if (DropShellTimer < 0.0f)
		{
			DropShellFlag = false;
			InitDropMesh(WeaponSettings.ShellBullets.DropMesh,
				WeaponSettings.ShellBullets.DropMeshOffset,
				WeaponSettings.ShellBullets.DropMeshImpulseDir,
				WeaponSettings.ShellBullets.DropMeshLifeTime,
				WeaponSettings.ShellBullets.ImpulseRandomDispersion,
				WeaponSettings.ShellBullets.PowerImpulse,
				WeaponSettings.ShellBullets.CustomMass);
		}
		else
		{
			DropShellTimer -= DeltaTime;
		}
	}
}

void AWeaponDefault::WeaponInit()
{
	if (SkeletalMeshWeapon && !SkeletalMeshWeapon->SkeletalMesh)
	{
		SkeletalMeshWeapon->DestroyComponent(true);
	}

	if (StaticMeshWeapon && !StaticMeshWeapon->GetStaticMesh())
	{
		StaticMeshWeapon->DestroyComponent(true);
	}

	UpdateStateWeapon(EMovementState::Run_State);
}

void AWeaponDefault::SetWeaponStateFire(bool bIsFire)
{
	if (CheckWeaponCanFire())
	{
		WeaponFiring = bIsFire;
	}
	else
	{
		WeaponFiring = false;
		FireTimer = 0.01f; 
	}
}

bool AWeaponDefault::CheckWeaponCanFire()
{
	return !BlockFire;
}

void AWeaponDefault::Fire()
{
	UAnimMontage* AnimToPlay = nullptr;
	if (WeaponAiming)
	{
		AnimToPlay = WeaponSettings.AnimWeaponInfo.AnimCharFireAim;
	}
	else
	{
		AnimToPlay = WeaponSettings.AnimWeaponInfo.AnimCharFire;
	}

	if (WeaponSettings.AnimWeaponInfo.AnimWeaponFire
		&& SkeletalMeshWeapon
		&& SkeletalMeshWeapon->GetAnimInstance())
	{
		SkeletalMeshWeapon->GetAnimInstance()->Montage_Play(WeaponSettings.AnimWeaponInfo.AnimWeaponFire);
	}

	if (WeaponSettings.ShellBullets.DropMesh)
	{
		if (WeaponSettings.ShellBullets.DropMeshTime < 0.0f)
		{
			InitDropMesh(WeaponSettings.ShellBullets.DropMesh,
				WeaponSettings.ShellBullets.DropMeshOffset,
				WeaponSettings.ShellBullets.DropMeshImpulseDir,
				WeaponSettings.ShellBullets.DropMeshLifeTime,
				WeaponSettings.ShellBullets.ImpulseRandomDispersion,
				WeaponSettings.ShellBullets.PowerImpulse,
				WeaponSettings.ShellBullets.CustomMass);
		}
		else
		{
			DropShellFlag = true;
			DropShellTimer = WeaponSettings.ShellBullets.DropMeshTime;
		}
	}

	FireTimer = WeaponSettings.RateOfFire;
	WeaponInfo.Round--;
	ChangeDispersionByShot();

	OnWeaponFireStart.Broadcast(AnimToPlay);

	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), WeaponSettings.SoundFireWeapon, ShootLocation->GetComponentLocation());
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponSettings.EffectFireWeapon, ShootLocation->GetComponentTransform());

	int8 NumberProjectile = GetNumberProjectileByShot();

	if (ShootLocation)
	{
		FVector SpawnLocation = ShootLocation->GetComponentLocation();
		FRotator SpawnRotation = ShootLocation->GetComponentRotation();
		FProjectileInfo ProjectileInfo;
		ProjectileInfo = GetProjectile();

		FVector EndLocation;
		for (int8 i = 0; i < NumberProjectile; i++)
		{
			EndLocation = GetFireEndLocation();

			if (ProjectileInfo.Projectiles)
			{
				FVector Dir = ShootEndLocation - SpawnLocation;
				Dir.Normalize();

				FMatrix myMatrix(Dir, FVector(0, 0, 0), FVector(0, 0, 0), FVector::ZeroVector);
				SpawnRotation = myMatrix.Rotator();

				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParams.Owner = GetOwner();
				SpawnParams.Instigator = GetInstigator();

				AProjectileDefault* myProjectile = Cast<AProjectileDefault>(GetWorld()->
					SpawnActor(ProjectileInfo.Projectiles, &SpawnLocation, &SpawnRotation, SpawnParams));
				if (myProjectile)
				{
					myProjectile->InitProjectile(WeaponSettings.ProjectileSettings);
				}
			}
			else
			{
				FHitResult Hit;
				TArray<AActor*> Actors;

				UKismetSystemLibrary::LineTraceSingle(GetWorld(), SpawnLocation, EndLocation * WeaponSettings.DistanceTrace,
					ETraceTypeQuery::TraceTypeQuery4, false, Actors, EDrawDebugTrace::ForDuration, Hit, true,
					FLinearColor::Red, FLinearColor::Green, 5.0f);

				if (ShowDebug)
				{
					DrawDebugLine(GetWorld(), SpawnLocation, SpawnLocation + ShootLocation->GetForwardVector() * WeaponSettings.DistanceTrace,
						FColor::Black, false, 5.f, (uint8)'\000', 0.5f);
				}

				if (Hit.GetActor() && Hit.PhysMaterial.IsValid())
				{
					EPhysicalSurface mySurfaceType = UGameplayStatics::GetSurfaceType(Hit);

					if (WeaponSettings.ProjectileSettings.HitDecals.Contains(mySurfaceType))
					{
						UMaterialInterface* myMaterial = WeaponSettings.ProjectileSettings.HitDecals[mySurfaceType];

						if (myMaterial && Hit.GetComponent())
						{
							UGameplayStatics::SpawnDecalAttached(myMaterial, FVector(20.0f), Hit.GetComponent(), NAME_None, Hit.ImpactPoint,
								Hit.ImpactNormal.Rotation(), EAttachLocation::KeepWorldPosition);
						}
					}
					if (WeaponSettings.ProjectileSettings.HitFXs.Contains(mySurfaceType))
					{
						UParticleSystem* myParticle = WeaponSettings.ProjectileSettings.HitFXs[mySurfaceType];

						if (myParticle)
						{
							UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), myParticle,
								FTransform(Hit.ImpactNormal.Rotation(), Hit.ImpactPoint, FVector(2.0f)));
						}
					}

					UTypes::AddEffectBySurfaceType(Hit.GetActor(), ProjectileInfo.Effect, mySurfaceType);

					UGameplayStatics::ApplyPointDamage(
						Hit.GetActor(),
						WeaponSettings.WeaponDamage,
						Hit.TraceEnd,
						Hit,
						GetInstigatorController(),
						this,
						NULL);
				}
			}
		}
	}
}

void AWeaponDefault::UpdateStateWeapon(EMovementState NewMovementState)
{
	BlockFire = false;

	switch (NewMovementState)
	{
	case EMovementState::Aim_State:
		CurrentDispersionMax = WeaponSettings.DispersionWeapon.Aim_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSettings.DispersionWeapon.Aim_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSettings.DispersionWeapon.Aim_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSettings.DispersionWeapon.Aim_StateDispersionAimReduction;
		break;
	case EMovementState::AimWalk_State:
		CurrentDispersionMax = WeaponSettings.DispersionWeapon.AimWalk_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSettings.DispersionWeapon.AimWalk_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSettings.DispersionWeapon.AimWalk_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSettings.DispersionWeapon.AimWalk_StateDispersionAimReduction;
		break;
	case EMovementState::Walk_State:
		CurrentDispersionMax = WeaponSettings.DispersionWeapon.Walk_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSettings.DispersionWeapon.Walk_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSettings.DispersionWeapon.Walk_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSettings.DispersionWeapon.Walk_StateDispersionAimReduction;
		break;
	case EMovementState::Run_State:
		CurrentDispersionMax = WeaponSettings.DispersionWeapon.Run_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSettings.DispersionWeapon.Run_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSettings.DispersionWeapon.Run_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSettings.DispersionWeapon.Run_StateDispersionAimReduction;
		break;
	case EMovementState::SprintRun_State:
	case EMovementState::Stun_State:
		BlockFire = true;
		SetWeaponStateFire(false);
	default:
		break;
	}
}

void AWeaponDefault::ChangeDispersionByShot()
{
	CurrentDispersion += CurrentDispersionRecoil;
}

float AWeaponDefault::GetCurrentDispersion() const
{
	float Result = CurrentDispersion;
	return Result;
}

FVector AWeaponDefault::ApplyDispersionToShot(FVector DirectionShoot) const
{
	return FMath::VRandCone(DirectionShoot, GetCurrentDispersion() * PI / 180.f);
}

FVector AWeaponDefault::GetFireEndLocation() const
{
	bool bShotDirection = false;
	FVector EndLocation = FVector(0.f);

	FVector tmpV = (ShootLocation->GetComponentLocation() - ShootEndLocation);

	if (tmpV.Size() > SizeVectorToChangeShootDirectionLogic)
	{
		EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShot((ShootLocation->GetComponentLocation() - ShootEndLocation).GetSafeNormal()) * -20000.0f;
		if (ShowDebug)
		{
			DrawDebugCone(GetWorld(), ShootLocation->GetComponentLocation(), -(ShootLocation->GetComponentLocation() - ShootEndLocation), WeaponSettings.DistanceTrace, GetCurrentDispersion() * PI / 180.f, GetCurrentDispersion() * PI / 180.f, 32, FColor::Emerald, false, .1f, (uint8)'\000', 1.0f);
		}
	}
	else
	{
		EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShot(ShootLocation->GetForwardVector()) * 20000.0f;
		if (ShowDebug)
		{
			DrawDebugCone(GetWorld(), ShootLocation->GetComponentLocation(), ShootLocation->GetForwardVector(), WeaponSettings.DistanceTrace, GetCurrentDispersion() * PI / 180.f, GetCurrentDispersion() * PI / 180.f, 32, FColor::Emerald, false, .1f, (uint8)'\000', 1.0f);
		}
	}

	if (ShowDebug)
	{
		//direction weapon look - Cyan
		DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), ShootLocation->GetComponentLocation() + ShootLocation->GetForwardVector() * 500.0f, FColor::Cyan, false, 5.f, (uint8)'\000', 0.5f);

		//direction projectile must fly - Red
		DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), ShootEndLocation, FColor::Red, false, 5.f, (uint8)'\000', 0.5f);

		//direction projectile current fly - White
		DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), EndLocation, FColor::White, false, 5.f, (uint8)'\000', 0.5f);
	}

	return EndLocation;
}

int8 AWeaponDefault::GetNumberProjectileByShot() const
{
	return WeaponSettings.NumberProjectileByShot;
}

int32 AWeaponDefault::GetWeaponRound()
{
	return WeaponInfo.Round;
}

void AWeaponDefault::InitReload()
{
	WeaponFiring = false;
	WeaponReloading = true;
	ReloadTimer = WeaponSettings.ReloadTime;

	UAnimMontage* AnimToPlay = nullptr;
	if (WeaponAiming)
	{
		AnimToPlay = WeaponSettings.AnimWeaponInfo.AnimCharReloadAim;
	}
	else
	{
		AnimToPlay = WeaponSettings.AnimWeaponInfo.AnimCharReload;
	}

	OnWeaponReloadStart.Broadcast(AnimToPlay);

	UAnimMontage* AnimWeaponToPlay = nullptr;
	if (WeaponAiming)
	{
		AnimWeaponToPlay = WeaponSettings.AnimWeaponInfo.AnimWeaponReloadAim;
	}
	else
	{
		AnimWeaponToPlay = WeaponSettings.AnimWeaponInfo.AnimWeaponReload;
	}

	if (WeaponSettings.AnimWeaponInfo.AnimWeaponReload
		&& SkeletalMeshWeapon
		&& SkeletalMeshWeapon->GetAnimInstance())
	{
		SkeletalMeshWeapon->GetAnimInstance()->Montage_Play(AnimWeaponToPlay);
	}

	if (WeaponSettings.ClipDropMesh.DropMesh)
	{
		DropClipFlag = true;
		DropClipTimer = WeaponSettings.ClipDropMesh.DropMeshTime;
	}
}

void AWeaponDefault::FinishReload()
{
	WeaponReloading = false;

	int8 AvailableAmmoFromInventory = GetAailableAmmoForReload();
	int8 AmmoNeedTake;
	int8 NeedToReload = WeaponSettings.MaxRound - WeaponInfo.Round;

	if (NeedToReload > AvailableAmmoFromInventory)
	{
		WeaponInfo.Round = AvailableAmmoFromInventory;
		AmmoNeedTake = AvailableAmmoFromInventory;
	}
	else
	{
		WeaponInfo.Round += NeedToReload;
		AmmoNeedTake = NeedToReload;
	}

	OnWeaponReloadEnd.Broadcast(true, AmmoNeedTake);
}

void AWeaponDefault::CancelReload()
{
	WeaponReloading = false;
	if (SkeletalMeshWeapon && SkeletalMeshWeapon->GetAnimInstance())
	{
		SkeletalMeshWeapon->GetAnimInstance()->StopAllMontages(0.11f);
	}

	OnWeaponReloadEnd.Broadcast(false, 0);
	DropClipFlag = false;
}

bool AWeaponDefault::CheckCanWeaponReload()
{
	bool Result = true;
	if (GetOwner())
	{
		UTPSInventoryComponent* MyInv = Cast<UTPSInventoryComponent>
			(GetOwner()->GetComponentByClass(UTPSInventoryComponent::StaticClass()));
		if (MyInv)
		{
			int8 AvailableAmmoForWeapon;
			if (!MyInv->CheckAmmoForWeapon(WeaponSettings.WeaponType, AvailableAmmoForWeapon))
			{
				Result = false;
			}
		}
	}

	return Result;
}

int8 AWeaponDefault::GetAailableAmmoForReload()
{
	int8 AvailableAmmoForWeapon = WeaponSettings.MaxRound;
	if (GetOwner())
	{
		UTPSInventoryComponent* MyInv = Cast<UTPSInventoryComponent>
			(GetOwner()->GetComponentByClass(UTPSInventoryComponent::StaticClass()));
		if (MyInv)
		{
			if (MyInv->CheckAmmoForWeapon(WeaponSettings.WeaponType, AvailableAmmoForWeapon))
			{
				//AvailableAmmoForWeapon = AvailableAmmoForWeapon;
			}
		}
	}

	return AvailableAmmoForWeapon;

	/*
	maybe

	return WeaponSettings.MaxRound ???
	*/
}

void AWeaponDefault::InitDropMesh(UStaticMesh* DropMesh, FTransform Offset, FVector DropImpulseDirection, float LifeTimeMesh, float ImpulseRandomDisperssion, float PowerImpulse, float CustomMass)
{
	if (DropMesh)
	{
		FTransform Transform;

		FVector LocalDir = this->GetActorForwardVector() * Offset.GetLocation().X + this->GetActorRightVector() * Offset.GetLocation().Y + this->GetActorUpVector() * Offset.GetLocation().Z;

		Transform.SetLocation(GetActorLocation() + LocalDir);
		Transform.SetScale3D(Offset.GetScale3D());

		Transform.SetRotation((GetActorRotation() + Offset.Rotator()).Quaternion());
		AStaticMeshActor* NewActor = nullptr;

		FActorSpawnParameters Param;
		Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		Param.Owner = this;
		NewActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Transform, Param);

		if (NewActor && NewActor->GetStaticMeshComponent())
		{
			NewActor->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("IgnoreOnlyPawn"));
			NewActor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

			NewActor->SetActorTickEnabled(true);
			NewActor->InitialLifeSpan = LifeTimeMesh;

			NewActor->GetStaticMeshComponent()->Mobility = EComponentMobility::Movable;
			NewActor->GetStaticMeshComponent()->SetSimulatePhysics(true);
			NewActor->GetStaticMeshComponent()->SetStaticMesh(DropMesh);

			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Block);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECollisionResponse::ECR_Block);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECollisionResponse::ECR_Block);

			if (CustomMass > 0.0f)
			{
				NewActor->GetStaticMeshComponent()->SetMassOverrideInKg(NAME_None, CustomMass, true);
			}

			if (!DropImpulseDirection.IsNearlyZero())
			{
				FVector FinalDir;
				LocalDir += DropImpulseDirection * 1000.0f;

				if (!FMath::IsNearlyZero(ImpulseRandomDisperssion))
				{
					FinalDir += UKismetMathLibrary::RandomUnitVectorInConeInDegrees(LocalDir, ImpulseRandomDisperssion);
				}
				FinalDir.GetSafeNormal(0.0001f);

				NewActor->GetStaticMeshComponent()->AddImpulse(FinalDir * PowerImpulse);
			}
		}
	}
}

FProjectileInfo AWeaponDefault::GetProjectile()
{
	return WeaponSettings.ProjectileSettings;
}