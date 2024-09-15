// Fill out your copyright notice in the Description page of Project Settings.

#include "StateEffects/TPS_StateEffect.h"
#include "Components/TPSHelathComponent.h"
#include "Interfaces/TPS_IGameActor.h"
#include "Kismet/GameplayStatics.h"
#include "TPSCharacter.h"
#include "Components/TPSCharacterHelathComponent.h"
#include "Structure/TPS_EnvironmentStructure.h"
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
		/*
		FVector Loc = FVector(FVector::ZeroVector);

		auto Character = Cast<ACharacter>(myActor);
		if (Character)
		{
			Loc = FVector(FVector::ZeroVector);
		}

		auto Player = Cast<ATPSCharacter>(myActor);
		if (Player)
		{
			Loc = Player->GetParticleOffset();
		}

		auto Environment = Cast<ATPS_EnvironmentStructure>(myActor);
		if (Environment)
		{
			Loc = Environment->GetParticleOffset();
		}
		*/

		ParticleEmmiter = UGameplayStatics::SpawnEmitterAttached(
			ParticleEffect,
			myActor->GetRootComponent(),
			NameBoneToAttachParticle,
			ParticleOffset,
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

	if (Character && !bStatus && !CharacterStunned)
	{
		Player->GetStun();
		CharacterStunned = true;
	}
	else if (Character && bStatus && CharacterStunned)
	{
		Player->StunOut();
		CharacterStunned = false;
	}
}

void UTPS_StateEffect_Invincibility::DestroyObject()
{
	if (CheckCharHealthComponent() && bIsCharacterHasBuff)
	{
		bIsCharacterHasBuff = false;
		CharHealthComp->bIsInvincibility = false;
	}

	Super::DestroyObject();
}

void UTPS_StateEffect_Invincibility::Execute()
{
	if (CheckCharHealthComponent() && !bIsCharacterHasBuff)
	{
		bIsCharacterHasBuff = true;
		CharHealthComp->bIsInvincibility = true;
	}
}

bool UTPS_StateEffect::CheckCharHealthComponent()
{
	if (!myActor) return false;

	auto Player = Cast<ATPSCharacter>(myActor);
	if (!Player) return false;

	auto CharacterHealthComponent = Cast<UTPSCharacterHealthComponent>
		(Player->GetComponentByClass
		(UTPSCharacterHealthComponent::StaticClass()));

	if (!CharacterHealthComponent) return false;

	CharHealthComp = CharacterHealthComponent;

	return true;
}

void UTPS_StateEffect_HPBoost::DestroyObject()
{
	if (CheckCharHealthComponent() && bIsCharacterHasBuff)
	{
		if (CharHealthComp->GetCurrentHealth() >= CharHealthComp->GetDefaultMaxHealth())
		{
			AfterBuffHealth = CharHealthComp->GetDefaultMaxHealth();
		}
		else
		{
			AfterBuffHealth = CharHealthComp->GetCurrentHealth();
		}
		CharHealthComp->SetCurrentHealth(AfterBuffHealth);

		bIsCharacterHasBuff = false;
		CharHealthComp->SetMaxHealth(CharHealthComp->GetDefaultMaxHealth());
		CharHealthComp->OnHPBuffTaken.Broadcast(
			AfterBuffHealth,
			CharHealthComp->GetDefaultMaxHealth());

	}

	Super::DestroyObject();
}

void UTPS_StateEffect_HPBoost::Execute()
{
	if (CheckCharHealthComponent() && !bIsCharacterHasBuff)
	{
		bIsCharacterHasBuff = true;
		CharHealthComp->SetMaxHealth(NewMaxHealth);
		CharHealthComp->OnHPBuffTaken.Broadcast(
			CharHealthComp->GetCurrentHealth(), 
			NewMaxHealth);
	}
}
