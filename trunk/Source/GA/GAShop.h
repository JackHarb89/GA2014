// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GAItem.h"
#include "GAShop.generated.h"

/**
 * 
 */
UCLASS(ABSTRACT)
class AGAShop : public AActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Item")				TArray<UBlueprint*> ShopItems;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Item")				float ItemCost;
	UPROPERTY(Replicated)																	AGAItem* BoughtItem;

	UFUNCTION(reliable, server, WithValidation)												void ServerGenerateItem();

	void GenerateItem();
	AGAItem* BuyItem();
};
