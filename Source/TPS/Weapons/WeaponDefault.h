// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"

#include "FunctionLibrary/Types.h"
#include "Weapons/Projectiles/ProjectileDefault.h"
#include "WeaponDefault.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponFireStart, UAnimMontage*, AnimFire);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponReloadStart,UAnimMontage*,Anim);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponReloadEnd, bool ,bIsSuccess, int32, AmmoSafe);

UCLASS()
class TPS_API AWeaponDefault : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponDefault();

	FOnWeaponFireStart OnWeaponFireStart;
	FOnWeaponReloadEnd OnWeaponReloadEnd;
	FOnWeaponReloadStart OnWeaponReloadStart;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
	class USceneComponent* SceneComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
	class USkeletalMeshComponent* SkeletalMeshWeapon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
	class UStaticMeshComponent* StaticMeshWeapon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
	class UArrowComponent* ShootLocation = nullptr;

	UPROPERTY()
	FWeaponInfo WeaponSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	FAdditionalWeaponInfo WeaponInfo;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void FireTick(float DeltaTime);
	void ReloadTick(float DeltaTime);
	void DispersionTick(float DeltaTime);
	void ClipDropTick(float DeltaTime);
	void ShellDropTick(float DeltaTime);

	void WeaponInit();

	UFUNCTION(BlueprintCallable)
	void SetWeaponStateFire(bool bIsFire);

	bool CheckWeaponCanFire();

	FProjectileInfo GetProjectile();

	UFUNCTION()
	void Fire();

	void UpdateStateWeapon(EMovementState NewMovementState);
	void ChangeDispersionByShot();
	float GetCurrentDispersion() const;
	FVector ApplyDispersionToShot(FVector DirectionShoot) const;

	FVector GetFireEndLocation() const;
	int8 GetNumberProjectileByShot() const;

	float FireTimer = 0.0f;
	float FireDelay = 0.0f;
	float WeaponDamage = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reload Logic")
	float ReloadTimer = 0.0f;
	//Remove !!! Debug
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reload Logic Debug")
	float ReloadTime = 0.0f;

	//Flags
	bool BlockFire = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire Logic")
	bool WeaponFiring = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reload Logic")
	bool WeaponReloading = false;
	bool WeaponAiming = false;

	bool DropClipFlag = false;
	float DropClipTimer = -1.0f;

	bool DropShellFlag = false;
	float DropShellTimer = -1.0f;

	//Dispersion
	bool ShouldReduceDispersion = false;
	float CurrentDispersion = 0.0f;
	float CurrentDispersionMax = 1.0f;
	float CurrentDispersionMin = 0.1f;
	float CurrentDispersionRecoil = 0.1f;
	float CurrentDispersionReduction = 0.1f;

	FVector ShootEndLocation = FVector(0);

	UFUNCTION(BlueprintCallable)
	int32 GetWeaponRound();

	void InitReload();
	void FinishReload();
	void CancelReload();

	bool CheckCanWeaponReload();
	int8 GetAailableAmmoForReload();

	UFUNCTION()
	void InitDropMesh(UStaticMesh* DropMesh, FTransform Offset, FVector DropImpulseDirection, float LifeTimeMesh, float ImpulseRandomDisperssion, float PowerImpulse, float CustomMass);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool ShowDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	float SizeVectorToChangeShootDirectionLogic = 100.0f;
};
