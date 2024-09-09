// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/TPSHelathComponent.h"

UTPSHealthComponent::UTPSHealthComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

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

void UTPSHealthComponent::SetCurrentHealth(float NewHealth)
{
	Health = NewHealth;
}

void UTPSHealthComponent::ChangeHealthValue(float ChangeValue)
{
	ChangeValue *= CoefDamage;

	Health += ChangeValue;
	OnHealthChanged.Broadcast(Health, ChangeValue);

	if (Health > 100.0f)
	{
		Health = 100.0f;
	}
	else
	{
		if (Health <= 0.0f)
		{
			OnDead.Broadcast();
		}
	}
	
	/*
	UE_LOG(LogTemp, Display, TEXT("Current Health: %0.1f"), GetCurrentHealth());
	UE_LOG(LogTemp, Display, TEXT("Recieved Damage: %0.1f"), -ChangeValue);
	*/
	
}
