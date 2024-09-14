// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/TPS_IGameActor.h"
#include "StateEffects/TPS_StateEffect.h"

#include "TPS_EnvironmentStructure.generated.h"

UCLASS()
class TPS_API ATPS_EnvironmentStructure : public AActor, public ITPS_IGameActor
{
	GENERATED_BODY()
	
public:	

	ATPS_EnvironmentStructure();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

	EPhysicalSurface GetSurfaceType() override;

	TArray<UTPS_StateEffect*> GetAllCurrentEffects() override;
	void RemoveEffect(UTPS_StateEffect* RemovedEffect) override;
	void AddEffect(UTPS_StateEffect* NewEffect) override;

	//Effects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TArray<UTPS_StateEffect*> Effects;

};
