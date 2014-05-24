// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "GAGameMode.generated.h"

UCLASS(minimalapi)
class AGAGameMode : public AGameMode
{
	GENERATED_UCLASS_BODY()

	int32 playerCount;

	/** select best spawn point for player */
	virtual AActor* ChoosePlayerStart(AController* Player) OVERRIDE;
};


