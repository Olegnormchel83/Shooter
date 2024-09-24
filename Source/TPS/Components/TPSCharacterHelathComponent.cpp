// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/TPSCharacterHelathComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TPSCharacter.h"

DEFINE_LOG_CATEGORY_STATIC(LogTPSCharacterHelathComponent, All, All);

void UTPSCharacterHealthComponent::ChangeHealthValue(float ChangeValue)
{	
	if (!GetWorld()) return;

	const auto Player = Cast<ATPSCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!Player) return;

	if (bIsInvincibility && ChangeValue < 0) return;

	if (!Player->IsAlive()) return;

	float CurrentDamage = ChangeValue * CoefDamage;

	if (Shield > 0.0f && ChangeValue < 0.0f)
	{
		ChangeShieldValue(CurrentDamage);
		if (Shield <= 0.0f)
		{
			if (ShieldDestroyedFX)
			{
				//FX
				UGameplayStatics::SpawnEmitterAttached(
					ShieldDestroyedFX,
					Player->GetMesh(),
					ShieldFXSocketName,
					FVector::ZeroVector,
					FRotator::ZeroRotator,
					EAttachLocation::SnapToTarget);
			}
			else
			{
				UE_LOG(LogTPSCharacterHelathComponent, Display, TEXT("ShieldDestroyedFX is NONE"));
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

	const auto Player = Cast<ATPSCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!Player) return;

	if (bIsInvincibility && ChangeValue < 0) return;

	if (!Player->IsAlive()) return;

	Shield += ChangeValue;

	OnShieldChanged.Broadcast(Shield, ChangeValue);

	if (ChangeValue < 0.0f && GetCurrentShield() > 0.0f)
	{
		UE_LOG(LogTPSCharacterHelathComponent, Display, TEXT("UTPSCharacterHealthComponent::ChangeShieldValue - Shield Damaged"));
		if (ShieldDamagedFX)
		{
			UGameplayStatics::SpawnEmitterAttached(
				ShieldDamagedFX, 
				Player->GetMesh(), 
				ShieldFXSocketName, 
				FVector::ZeroVector, 
				FRotator::ZeroRotator, 
				EAttachLocation::SnapToTarget);
		}
		else
		{
			UE_LOG(LogTPSCharacterHelathComponent, Display, TEXT("ShieldDamagedFX is NONE"));
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

	/*
	GetWorld()->GetTimerManager().SetTimer(
			TimerHadle_CooldownShieldTimer, 
			this, 
			&UTPSCharacterHealthComponent::CooldownShieldEnd, 
			CooldownShieldRecoverTime,
			false);

	GetWorld()->GetTimerManager().ClearTimer(TimerHadle_ShieldRecoveryRateTimer);
	*/
}

void UTPSCharacterHealthComponent::CooldownShieldEnd()
{
	if (!GetWorld()) return;

	const auto Player = Cast<ATPSCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!Player) return;

	if (!Player->IsAlive()) return;
	
	/*
	GetWorld()->GetTimerManager().SetTimer(
			TimerHadle_ShieldRecoveryRateTimer,
			this,
			&UTPSCharacterHealthComponent::RecoveryShield,
			ShieldRecoverRate,
			true);
	*/
}

void UTPSCharacterHealthComponent::RecoveryShield()
{
	if (!GetWorld()) return;

	const auto Player = Cast<ATPSCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!Player) return;

	if (!Player->IsAlive()) return;

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

bool UTPSCharacterHealthComponent::IsInvincibility()
{
	return bIsInvincibility;
}
