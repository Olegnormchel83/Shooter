// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"

#include "Types.generated.h"

class AWeaponDefault;

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Aim_State UMETA(DisplayName = "Aim State"),
	AimWalk_State UMETA(DisplayName = "AimWalk State"),
	Walk_State UMETA(DisplayName = "Walk State"),
	Run_State UMETA(DisplayName = "Run State"),
	SprintRun_State UMETA(DisplayName = "SprintRun State")
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	RifleType UMETA(DisplayName = "Rifle"),
	ShotgunType UMETA(DisplayName = "Shotgun"),
	SniperType UMETA(DisplayName = "SniperRifle"),
	GrenadeLauncherType UMETA(DisplayName = "GrenadeLauncher"),
	RocketLauncherType UMETA(DisplayName = "RocketLauncher"),
	NoneType UMETA(DisplayName = "None")
};

USTRUCT(BlueprintType)
struct FCharacterSpeed
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float AimSpeedNormal = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeedNormal = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RunSpeedNormal = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float AimSpeedWalk = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintRunSpeedRun = 800.0f;
};

USTRUCT(BlueprintType)
struct FProjectileInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
	TSubclassOf<class AProjectileDefault> Projectiles = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
	UStaticMesh* ProjectileStaticMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
	FTransform ProjectileStaticMeshOffset = FTransform();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
	UParticleSystem* ProjectileTrailFx = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
	FTransform PorjectileTrailFxOffset = FTransform();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
	float ProjectileDamage = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
	float ProjectileLifeTime = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
	float ProjectileInitSpeed = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	TMap<TEnumAsByte<EPhysicalSurface>, UMaterialInterface*> HitDecals;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	TMap<TEnumAsByte<EPhysicalSurface>, UParticleSystem*> HitFXs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	USoundBase* HitSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explode")
	UParticleSystem* ExploseFX = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explode")
	USoundBase* ExploseSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explode")
	USoundBase* PreExploseSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explode")
	float ProjectileMinRadiusDamage = 75.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explode")
	float ProjectileMaxRadiusDamage = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explode")
	float ExploseMaxDamage = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explode")
	float ExploseFallofCoef = 1.0f;

};

USTRUCT(BlueprintType)
struct FWeaponDispersion
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | Aim_State")
	float Aim_StateDispersionAimMax = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | Aim_State")
	float Aim_StateDispersionAimMin = 0.3f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | Aim_State")
	float Aim_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | Aim_State")
	float Aim_StateDispersionAimReduction = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | AimWalk_State")
	float AimWalk_StateDispersionAimMax = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | AimWalk_State")
	float AimWalk_StateDispersionAimMin = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | AimWalk_State")
	float AimWalk_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | AimWalk_State")
	float AimWalk_StateDispersionAimReduction = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | Walk_State")
	float Walk_StateDispersionAimMax = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | Walk_State")
	float Walk_StateDispersionAimMin = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | Walk_State")
	float Walk_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | Walk_State")
	float Walk_StateDispersionAimReduction = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | Run_State")
	float Run_StateDispersionAimMax = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | Run_State")
	float Run_StateDispersionAimMin = 4.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | Run_State")
	float Run_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | Run_State")
	float Run_StateDispersionAimReduction = 0.1f;
};

USTRUCT(BlueprintType)
struct FAnimationWeaponInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	UAnimMontage* AnimCharFire = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	UAnimMontage* AnimCharReload = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	UAnimMontage* AnimCharFireAim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	UAnimMontage* AnimCharReloadAim = nullptr;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Weapon")
	UAnimMontage* AnimWeaponReload = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Weapon")
	UAnimMontage* AnimWeaponReloadAim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Weapon")
	UAnimMontage* AnimWeaponFire = nullptr;
};

USTRUCT(BlueprintType)
struct FDropMeshInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropMesh")
	UStaticMesh* DropMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropMesh")
	float DropMeshTime = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropMesh")
	float DropMeshLifeTime = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropMesh")
	FTransform DropMeshOffset = FTransform();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropMesh")
	FVector DropMeshImpulseDir = FVector(0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropMesh")
	float PowerImpulse = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropMesh")
	float ImpulseRandomDispersion = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropMesh")
	float CustomMass = 0.0f;

};

USTRUCT(BlueprintType)
struct FWeaponInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Calss")
	TSubclassOf<AWeaponDefault> WeaponClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float ReloadTime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float RateOfFire = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	int32 MaxRound = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	int32 NumberProjectileByShot= 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
	FWeaponDispersion DispersionWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* SoundFireWeapon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* SoundReloadWeapon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	UParticleSystem* EffectFireWeapon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	FProjectileInfo ProjectileSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	float WeaponDamage = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	float DistanceTrace = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitEffect")
	UDecalComponent* DecalOnHit = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	FAnimationWeaponInfo AnimWeaponInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	FDropMeshInfo ClipDropMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	FDropMeshInfo ShellBullets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	float SwitchTimeToWeapon = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	UTexture2D* WeaponIcon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	EWeaponType WeaponType = EWeaponType::RifleType;

};

USTRUCT(BlueprintType)
struct FAdditionalWeaponInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
	int32 Round = 30;
};

USTRUCT(BlueprintType)
struct FWeaponSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponSlot")
	FName NameItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponSlot")
	FAdditionalWeaponInfo AdditionalInfo;

};

USTRUCT(BlueprintType)
struct FAmmoSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmmoSlot")
	EWeaponType WeaponType = EWeaponType::RifleType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmmoSlot")
	int32 Cout = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmmoSlot")
	int32 MaxCout = 100;

};

USTRUCT(BlueprintType)
struct FDropItem : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropItem")
	UStaticMesh* WeaponStaticMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropItem")
	USkeletalMesh* WeaponSkeletalMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropItem")
	UParticleSystem* ParticleSystem = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropItem")
	FTransform Offset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropItem")
	FWeaponSlot WeaponInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropItem")
	FVector DropWeaponScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropItem")
	FVector DropItemParticleScale;

};

UCLASS()
class TPS_API UTypes : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};
