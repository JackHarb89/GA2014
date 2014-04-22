// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GAItem.h"
#include "Net/UnrealNetwork.h"


AGAItem::AGAItem(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	finishedDropAnimation = false;
	bReplicates = true;
	bAlwaysRelevant = true;
	bReplicateMovement = true;
}

void AGAItem::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME(AGAItem, finishedDropAnimation);
	DOREPLIFETIME(AGAItem, ItemID);
	DOREPLIFETIME(AGAItem, Value);
	DOREPLIFETIME(AGAItem, Rarity);
	DOREPLIFETIME(AGAItem, Slot);
	DOREPLIFETIME(AGAItem, ItemStats);
	DOREPLIFETIME(AGAItem, AuraPlayer);
	DOREPLIFETIME(AGAItem, AuraGroup); 
}

#pragma region Animation

void AGAItem::SetDropAnimationFinished(){
	if(Role < ROLE_Authority){
		ServerSetDropAnimationFinished();
	}
	else{
		ItemStats.CalculateAttack();
		ItemStats.CalculateArmor();
	}
	
	finishedDropAnimation = true;
}

#pragma endregion

#pragma region Network - Animation

bool AGAItem::ServerSetDropAnimationFinished_Validate(){ return true;}
void AGAItem::ServerSetDropAnimationFinished_Implementation(){finishedDropAnimation = true;}

#pragma endregion