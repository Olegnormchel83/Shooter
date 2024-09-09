// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSPlayerController.h"
#include "Engine/World.h"

ATPSPlayerController::ATPSPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ATPSPlayerController::OnUnPossess()
{
	Super::OnUnPossess();


}
