// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "GAGameMode.generated.h"

UCLASS(minimalapi)
class AGAGameMode : public AGameMode
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, noclear, BlueprintReadWrite, Category = "HUD Classes", meta = (DisplayName = "Main Menu HUD Class")) 
		TSubclassOf<class AGA_HUD> MainMenuHud;
	UPROPERTY(EditAnywhere, noclear, BlueprintReadWrite, Category = "HUD Classes", meta = (DisplayName = "Transition HUD Class"))
		TSubclassOf<class AGA_HUD> TransitionHud;
	UPROPERTY(EditAnywhere, noclear, BlueprintReadWrite, Category = "HUD Classes", meta = (DisplayName = "Game HUD Class"))
		TSubclassOf<class AGA_HUD> GameHud;

	virtual void PostSeamlessTravel() OVERRIDE;
	virtual void StartNewPlayer(APlayerController * NewPlayer) OVERRIDE;

};

