// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TPSPlayerController.generated.h"

UCLASS()
class ATPSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	ATPSPlayerController();

	virtual void OnUnPossess() override;
};
