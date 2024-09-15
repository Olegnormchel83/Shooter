// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Particles/ParticleSystemComponent.h"

#include "TPS_StateEffect.generated.h"

class UTPSCharacterHealthComponent;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class TPS_API UTPS_StateEffect : public UObject
{
	GENERATED_BODY()
	
public:

	virtual bool InitObject(AActor* Actor);
	virtual void DestroyObject();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TArray<TEnumAsByte<EPhysicalSurface>> PossibleInteractSurface;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bIsStakable = false;

	bool CheckCharHealthComponent();
	UTPSCharacterHealthComponent* CharHealthComp = nullptr;

	AActor* myActor = nullptr;

};

UCLASS()
class TPS_API UTPS_StateEffect_ExecuteOnce : 
	public UTPS_StateEffect
{
	GENERATED_BODY()

public:

	bool InitObject(AActor* Actor) override;
	void DestroyObject() override;

	virtual void ExecuteOnce();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Execute Once")
	float PowerExecuteOnce = 20.0f;

};

UCLASS()
class TPS_API UTPS_StateEffect_ExecuteTimer : 
	public UTPS_StateEffect
{
	GENERATED_BODY()

public:
	bool InitObject(AActor* Actor) override;
	void DestroyObject() override;

	virtual void Execute();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting ExecuteTimer")
	float PowerExecuteTimer = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting ExecuteTimer")
	float Timer = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting ExecuteTimer")
	float RateTime = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting ExecuteTimer")
	FName NameBoneToAttachParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting ExecuteTimer")
	FVector ParticleOffset = FVector(FVector::ZeroVector);

	FTimerHandle TimerHandle_ExecuteTimer;
	FTimerHandle TimerHandle_EffectTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting ExecuteTimer")
	UParticleSystem* ParticleEffect = nullptr;

	UParticleSystemComponent* ParticleEmmiter = nullptr;
};

UCLASS()
class TPS_API UTPS_StateEffect_DisableInput : 
	public UTPS_StateEffect_ExecuteTimer
{
	GENERATED_BODY()

public:

	virtual void DestroyObject() override;
	virtual void Execute() override;

	bool CharacterStunned = false;

	void ChangeCharacterInputStatus(bool bStatus);
};

UCLASS()
class TPS_API UTPS_StateEffect_Invincibility :
	public UTPS_StateEffect_ExecuteTimer
{
	GENERATED_BODY()

public:

	void DestroyObject() override;
	void Execute() override;
	
	bool bIsCharacterHasBuff = false;
};

UCLASS()
class TPS_API UTPS_StateEffect_HPBoost :
	public UTPS_StateEffect_ExecuteTimer
{
	GENERATED_BODY()

public:

	void DestroyObject() override;
	void Execute() override;

	bool bIsCharacterHasBuff = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HPBoost Settings");
	float NewMaxHealth = 150.0f;

	float AfterBuffHealth = 1.0f;

};