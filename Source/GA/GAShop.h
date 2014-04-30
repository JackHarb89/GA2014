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

	// Every Item In Shop
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Item")				TArray<UBlueprint*> ShopItems;

	// Chosen Item With Cost
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Item")				float ItemCost;
	UPROPERTY(Replicated)																	AGAItem* BoughtItem;

	// Server Item Generation
	UFUNCTION(reliable, server, WithValidation)												void ServerGenerateItem();

	// Client Item Generation
	void GenerateItem();

	// Client Item Buy
	AGAItem* BuyItem();
};
