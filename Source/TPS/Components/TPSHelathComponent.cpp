// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/TPSHelathComponent.h"

DEFINE_LOG_CATEGORY_STATIC(HealthComponentLog, All, All);

UTPSHealthComponent::UTPSHealthComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

	Health = DefaultMaxHealth;
}


void UTPSHealthComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UTPSHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

float UTPSHealthComponent::GetCurrentHealth()
{
	return Health;
}

float UTPSHealthComponent::GetMaxHealth()
{
	return MaxHealth;
}

float UTPSHealthComponent::GetDefaultMaxHealth()
{
	return DefaultMaxHealth;
}

void UTPSHealthComponent::SetCurrentHealth(float NewHealth)
{
	Health = NewHealth;
}

void UTPSHealthComponent::SetMaxHealth(float NewMaxHealthValue)
{
	MaxHealth = NewMaxHealthValue;
}

void UTPSHealthComponent::ChangeHealthValue(float ChangeValue)
{
	ChangeValue *= CoefDamage;

	UE_LOG(HealthComponentLog, Display, TEXT("Current HP - %.1f"), GetCurrentHealth());
	UE_LOG(HealthComponentLog, Display, TEXT("Damaghe/Heal - %.1f"), ChangeValue);

	float tmp = Health + ChangeValue;
	if (tmp > GetMaxHealth())
	{
		Health = GetMaxHealth();
	}

	if (tmp > 0 && tmp <= GetMaxHealth())
	{
		Health = tmp;
	}

	OnHealthChanged.Broadcast(Health, ChangeValue);

	UE_LOG(HealthComponentLog, Display, TEXT("Changed HP - %.1f"), GetCurrentHealth());

	if (tmp <= 0)
	{
		OnDead.Broadcast();
	}

	/*
	UE_LOG(LogTemp, Display, TEXT("Current HP: %0.1f"), GetCurrentHealth());
	UE_LOG(LogTemp, Display, TEXT("Changed HP: %0.1f"), ChangeValue);
	*/
	
	
}
