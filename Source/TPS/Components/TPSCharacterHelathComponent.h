// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TPSHelathComponent.h"
#include "TPSCharacterHelathComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShieldChanged, float, Shield, float, Damage);

UCLASS()
class TPS_API UTPSCharacterHealthComponent : public UTPSHealthComponent
{
	GENERATED_BODY()
	

public:

	FTimerHandle TimerHadle_CooldownShieldTimer;
	FTimerHandle TimerHadle_ShieldRecoveryRateTimer;

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Shield")
	FOnShieldChanged OnShieldChanged;

protected:

	float Shield = 100.0f;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	float CooldownShieldRecoverTime = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	float ShieldRecoverValue = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	float ShieldRecoverRate = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shield")
	UParticleSystem* ShieldDamagedFX = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shield")
	UParticleSystem* ShieldDestroyedFX = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shield")
	FName ShieldFXSocketName = "SpawnShieldDamageSocket";

	void ChangeHealthValue(float ChangeValue) override;

	UFUNCTION(BlueprintCallable)
	float GetCurrentShield();

	void ChangeShieldValue(float ChangeValue);

	void CooldownShieldEnd();

	void RecoveryShield();
};
