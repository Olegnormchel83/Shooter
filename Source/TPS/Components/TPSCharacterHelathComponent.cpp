// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/TPSCharacterHelathComponent.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogTPSCharacterHelathComponent, All, All);

void UTPSCharacterHealthComponent::ChangeHealthValue(float ChangeValue)
{	
	if (!GetWorld()) return;

	float CurrentDamage = ChangeValue * CoefDamage;

	if (Shield > 0.0f && ChangeValue < 0.0f)
	{
		ChangeShieldValue(CurrentDamage);
		if (Shield <= 0.0f)
		{
			if (ShieldDestroyedFX) // && SoundShieldDestroyed??? 
			{
				//FX
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShieldDestroyedFX, FTransform());
			}

			UE_LOG(LogTPSCharacterHelathComponent, Display, TEXT("UTPSCharacterHealthComponent::ChangeHealthValue - Shield Destroyed"));
		}
	}
	else
	{
		Super::ChangeHealthValue(ChangeValue);
	}

}

float UTPSCharacterHealthComponent::GetCurrentShield()
{
	return Shield;
}

void UTPSCharacterHealthComponent::ChangeShieldValue(float ChangeValue)
{
	if (!GetWorld()) return;

	Shield += ChangeValue;

	OnShieldChanged.Broadcast(Shield, ChangeValue);

	if (ChangeValue < 0.0f && GetCurrentShield() > 0.0f)
	{
		UE_LOG(LogTPSCharacterHelathComponent, Display, TEXT("UTPSCharacterHealthComponent::ChangeShieldValue - Shield Damaged"));
		if (ShieldDamagedFX)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShieldDamagedFX, FTransform());
		}
	}

	if (Shield > 100.0f)
	{
		Shield = 100.0f;
	}
	else
	{
		if (Shield < 0.0f)
		{
			Shield = 0.0f;
		}
	}
	GetWorld()->GetTimerManager().SetTimer(
			TimerHadle_CooldownShieldTimer, 
			this, 
			&UTPSCharacterHealthComponent::CooldownShieldEnd, 
			CooldownShieldRecoverTime,
			false);

	GetWorld()->GetTimerManager().ClearTimer(TimerHadle_ShieldRecoveryRateTimer);
}

void UTPSCharacterHealthComponent::CooldownShieldEnd()
{
	if (!GetWorld()) return;
	
	GetWorld()->GetTimerManager().SetTimer(
			TimerHadle_ShieldRecoveryRateTimer,
			this,
			&UTPSCharacterHealthComponent::RecoveryShield,
			ShieldRecoverRate,
			true);
	
}

void UTPSCharacterHealthComponent::RecoveryShield()
{
	if (!GetWorld()) return;

	float tmp = Shield;
	tmp += ShieldRecoverValue;
	if (tmp > 100.0f)
	{
		Shield = 100.0f;
		GetWorld()->GetTimerManager().ClearTimer(TimerHadle_ShieldRecoveryRateTimer);
	}
	else
	{
		Shield = tmp;
	} 

	OnShieldChanged.Broadcast(Shield, ShieldRecoverValue);
}
