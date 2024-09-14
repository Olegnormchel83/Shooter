// Fill out your copyright notice in the Description page of Project Settings.


#include "Structure/TPS_EnvironmentStructure.h"
#include "Materials/MaterialInterface.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

DEFINE_LOG_CATEGORY_STATIC(EnvironmentStructureLog, All, All);

ATPS_EnvironmentStructure::ATPS_EnvironmentStructure()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ATPS_EnvironmentStructure::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATPS_EnvironmentStructure::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ATPS_EnvironmentStructure::GetParticleOffset()
{
	return ParticleOffset;
}

EPhysicalSurface ATPS_EnvironmentStructure::GetSurfaceType()
{
	EPhysicalSurface Result = EPhysicalSurface::SurfaceType_Default;
	const auto* myMesh = Cast<UStaticMeshComponent>
		(GetComponentByClass(UStaticMeshComponent::StaticClass()));

	if (myMesh)
	{
		UMaterialInterface* myMaterial = myMesh->GetMaterial(0);
		if (myMaterial)
		{
			Result = myMaterial->GetPhysicalMaterial()->SurfaceType;
		}
	}

	return Result;
}

TArray<UTPS_StateEffect*> ATPS_EnvironmentStructure::GetAllCurrentEffects()
{
	return Effects;
}

void ATPS_EnvironmentStructure::RemoveEffect(UTPS_StateEffect* RemovedEffect)
{
	Effects.Remove(RemovedEffect);
}

void ATPS_EnvironmentStructure::AddEffect(UTPS_StateEffect* NewEffect)
{	
	Effects.Add(NewEffect);
}
