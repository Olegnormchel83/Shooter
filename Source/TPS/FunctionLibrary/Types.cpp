// Fill out your copyright notice in the Description page of Project Settings.


#include "Types.h"
#include "TPS.h"
#include "Interfaces/TPS_IGameActor.h"

DEFINE_LOG_CATEGORY_STATIC(TypesLog, All, All);

void UTypes::AddEffectBySurfaceType(AActor* TakeEffectActor, TSubclassOf<UTPS_StateEffect> AddEffectClass, EPhysicalSurface SurfaceType)
{
	if (SurfaceType != EPhysicalSurface::SurfaceType_Default && TakeEffectActor && AddEffectClass)
	{
		UTPS_StateEffect* myEffect = Cast<UTPS_StateEffect>(AddEffectClass->GetDefaultObject());
		if (myEffect)
		{

			bool bIsHavePossibleSurface = false;
			int8 i = 0;
			while (i < myEffect->PossibleInteractSurface.Num() && !bIsHavePossibleSurface)
			{
				if (myEffect->PossibleInteractSurface[i] == SurfaceType)
				{
					bIsHavePossibleSurface = true;
					bool bIsCanAddEffect = false;
					if (!myEffect->bIsStakable)
					{
						//UE_LOG(TypesLog, Display, TEXT("This effect is NOT STAKABLE"));
						int8 j = 0;
						TArray<UTPS_StateEffect*> CurrentEffects;
						ITPS_IGameActor* myInterface = Cast<ITPS_IGameActor>(TakeEffectActor);
						if (myInterface)
						{
							CurrentEffects = myInterface->GetAllCurrentEffects();
						}

						if (CurrentEffects.Num() > 0)
						{
							while (j < CurrentEffects.Num() && !bIsCanAddEffect)
							{ 
								if (CurrentEffects[j]->GetClass() != AddEffectClass)
								{
									bIsCanAddEffect = true;
								}
								j++;
							}
						}
						else
						{
							//UE_LOG(TypesLog, Display, TEXT("This effect is STAKABLE"));
							bIsCanAddEffect = true;
						}
					}
					else
					{
						bIsCanAddEffect = true;
					}

					if (bIsCanAddEffect)
					{
						UTPS_StateEffect* NewEffect = NewObject<UTPS_StateEffect>
							(TakeEffectActor, AddEffectClass);
						if (NewEffect)
						{
							NewEffect->InitObject(TakeEffectActor);
						}
					}
				}
				i++;
			}
		}
	}
}
