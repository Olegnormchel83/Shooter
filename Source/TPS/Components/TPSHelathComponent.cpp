// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/TPSHelathComponent.h"

UTPSHealthComponent::UTPSHealthComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

	Health = MaxHealth;
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

void UTPSHealthComponent::SetCurrentHealth(float NewHealth)
{
	Health = NewHealth;
}

void UTPSHealthComponent::ChangeHealthValue(float ChangeValue)
{
	ChangeValue *= CoefDamage;

	float tmp = Health + ChangeValue;
	if (tmp > MaxHealth)
	{
		Health = MaxHealth;
	}

	if (tmp > 0 && tmp <= MaxHealth)
	{
		Health = tmp;
	}

	OnHealthChanged.Broadcast(Health, ChangeValue);

	if (tmp <= 0)
	{
		OnDead.Broadcast();
	}

	
	UE_LOG(LogTemp, Display, TEXT("Current HP: %0.1f"), GetCurrentHealth());
	UE_LOG(LogTemp, Display, TEXT("Changed HP: %0.1f"), ChangeValue);
	
	
}
