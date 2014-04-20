// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerController.h"
#include "GAItem.h"
#include "GACharacter.h"
#include "GAEnemy.h"
#include "GAPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class AGAPlayerController : public APlayerController
{
	GENERATED_UCLASS_BODY()

public:	

protected:
	// Shop
	void SellItem(AGAItem* item);
	void BuyItem(AGAItem* item);
};
