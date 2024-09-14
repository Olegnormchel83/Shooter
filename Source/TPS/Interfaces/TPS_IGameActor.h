// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StateEffects/TPS_StateEffect.h"

#include "TPS_IGameActor.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTPS_IGameActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TPS_API ITPS_IGameActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/*
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Event")
	void AvailableForEffectsBP();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Event")
	bool AvailableForEffects();

	virtual bool AvailableForEffectsOnlyCPP();
	*/

	virtual FVector GetParticleOffset();

	virtual EPhysicalSurface GetSurfaceType();

	virtual TArray<UTPS_StateEffect*> GetAllCurrentEffects();
	virtual void RemoveEffect(UTPS_StateEffect* RemovedEffect);
	virtual void AddEffect(UTPS_StateEffect* NewEffect);

};
