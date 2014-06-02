// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GAShop.h"
#include "Net/UnrealNetwork.h"



AGAShop::AGAShop(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	ItemCost = 100;

	bAlwaysRelevant = true;
	bReplicates = true;
}

// Replicates All Replicated Attributes
void AGAShop::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGAShop, BoughtItem);
	DOREPLIFETIME(AGAShop, ItemCost);
}

#pragma region Buy Item

// Generates One Item From The Possible Items
void AGAShop::GenerateItem(){
	if (Role < ROLE_Authority){
		ServerGenerateItem();
	}
	else{
		int32 randomIndex = FMath::RandRange(0, ShopItems.Num() - 1);
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = GetInstigator();
		SpawnParams.bNoCollisionFail = true;

		FVector SpawnLocation = GetActorLocation();
		FRotator SpawnRotation = GetActorRotation();

		// Spawn Item
		BoughtItem = GetWorld()->SpawnActor<AGAItem>(ShopItems[randomIndex]->GeneratedClass, SpawnLocation, SpawnRotation, SpawnParams);
		BoughtItem->finishedDropAnimation = true;
		UE_LOG(LogClass, Log, TEXT("*** SERVER :: GENERATED ITEM ***"));
	}
}

// Client Call To Buy The Generated Item
AGAItem* AGAShop::BuyItem(){
	GenerateItem();
	return BoughtItem;
}

#pragma endregion

#pragma region Network - Buy Item

bool AGAShop::ServerGenerateItem_Validate(){return true;}
void AGAShop::ServerGenerateItem_Implementation(){GenerateItem();}

#pragma endregion