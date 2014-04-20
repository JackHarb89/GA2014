// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GAPlayerController.h"
#include "Net/UnrealNetwork.h"


AGAPlayerController::AGAPlayerController(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
}



#pragma region Shop
// *** SYSTEM IS EXPERIMENTAL AND WIP ***
/*
void AGAPlayerController::SellItem(AGAItem* item){
	gaCharacter->Ressource += item->Value;
	gaCharacter->InventoryItems.Remove(item);
	UE_LOG(LogClass, Log, TEXT("*** PLAYER :: SOLD ITEM ***"));
	item->Destroy();
}
void AGAPlayerController::BuyItem(AGAItem* item){
	if (gaCharacter->Ressource - item->Value >= 0){
		gaCharacter->Ressource -= item->Value;
		UE_LOG(LogClass, Log, TEXT("*** PLAYER :: BAUGHT ITEM ***"));
	}
}
*/
#pragma endregion
