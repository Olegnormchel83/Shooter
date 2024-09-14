// Fill out your copyright notice in the Description page of Project Settings.


#include "StateEffects/TPS_StateEffect.h"
#include "Components/TPSHelathComponent.h"
#include "Interfaces/TPS_IGameActor.h"
#include "Kismet/GameplayStatics.h"
#include "TPSCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY_STATIC(StateEffectLog, All, All);

bool UTPS_StateEffect::InitObject(AActor* Actor)
{
	myActor = Actor;

	ITPS_IGameActor* myInterface = Cast<ITPS_IGameActor>(myActor);
	if (myInterface)
	{
		myInterface->AddEffect(this);
	}

	return true;
}


void UTPS_StateEffect::DestroyObject()
{
	ITPS_IGameActor* myInterface = Cast<ITPS_IGameActor>(myActor);
	if (myInterface)
	{
		myInterface->RemoveEffect(this);
	}

	myActor = nullptr;
	if (this && this->IsValidLowLevel())
	{
		this->ConditionalBeginDestroy();
	}
}

bool UTPS_StateEffect_ExecuteOnce::InitObject(AActor* Actor)
{
	Super::InitObject(Actor);
	ExecuteOnce();
	return true;
}

void UTPS_StateEffect_ExecuteOnce::DestroyObject()
{
	Super::DestroyObject();
}

void UTPS_StateEffect_ExecuteOnce::ExecuteOnce()
{
	if (myActor)
	{
		auto* myHealthComp = Cast<UTPSHealthComponent>
			(myActor->GetComponentByClass(UTPSHealthComponent::StaticClass()));
		if (myHealthComp)
		{
			myHealthComp->ChangeHealthValue(PowerExecuteOnce);
		}
	}

	DestroyObject();
}

bool UTPS_StateEffect_ExecuteTimer::InitObject(AActor* Actor)
{
	Super::InitObject(Actor);

	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle_EffectTimer, 
		this, 
		&UTPS_StateEffect_ExecuteTimer::DestroyObject, 
		Timer, 
		false);
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle_ExecuteTimer, 
		this, 
		&UTPS_StateEffect_ExecuteTimer::Execute, 
		RateTime, 
		true);

	if (ParticleEffect)
	{
		//TODO: For object with interface create func that return offset, Name Bones
		FName NameBoneToAttached;
		FVector Loc = FVector(FVector::ZeroVector);

		ParticleEmmiter = UGameplayStatics::SpawnEmitterAttached(
			ParticleEffect, 
			myActor->GetRootComponent(), 
			NameBoneToAttached, 
			Loc, 
			FRotator::ZeroRotator, 
			EAttachLocation::SnapToTarget, 
			false);
	}

	return true;
}

void UTPS_StateEffect_ExecuteTimer::DestroyObject()
{
	if (ParticleEmmiter)
	{
		ParticleEmmiter->DestroyComponent();
		ParticleEmmiter = nullptr;
	}
	Super::DestroyObject();
}

void UTPS_StateEffect_ExecuteTimer::Execute()
{
	if (myActor)
	{
		auto* myHealthComp = Cast<UTPSHealthComponent>
			(myActor->GetComponentByClass(UTPSHealthComponent::StaticClass()));

		if (myHealthComp)
		{
			myHealthComp->ChangeHealthValue(PowerExecuteTimer);
		}
	}
}

void UTPS_StateEffect_DisableInput::DestroyObject()
{
	if (myActor)
	{
		ChangeCharacterInputStatus(true);

		auto Character = Cast<ACharacter>(myActor);
		if (Character)
		{
			Character->StopAnimMontage(StunAnimation);
			Character->GetCharacterMovement()->MaxWalkSpeed = 600;	
		}
	}

	Super::DestroyObject();
}

void UTPS_StateEffect_DisableInput::Execute()
{
	if (!myActor) return;

	ChangeCharacterInputStatus(false);
}

void UTPS_StateEffect_DisableInput::ChangeCharacterInputStatus(bool bStatus)
{
	auto Character = Cast<ACharacter>(myActor);
	auto Player = Cast<ATPSCharacter>(myActor);

	if (!Character || !Player) return;

	if (StunAnimation)
	{
		if (Character && !bStatus && !CharacterStunned)
		{
			UE_LOG(StateEffectLog, Display, TEXT("Character Get Stun"));

			Player->GetStun();
			CharacterStunned = true;
		}
		else if (Character && bStatus && CharacterStunned)
		{
			UE_LOG(StateEffectLog, Display, TEXT("Character Back"));

			Player->StunOut();
			CharacterStunned = false;
		}
	}
}
