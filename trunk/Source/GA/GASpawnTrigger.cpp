// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GASpawnTrigger.h"


AGASpawnTrigger::AGASpawnTrigger(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
}

// If Trigger Overlaps With Another Actor *** OVERRIDE - Checks Actor With Tag "TriggerAble" ***
void AGASpawnTrigger::ReceiveActorBeginOverlap(class AActor* OtherActor){
	if (OtherActor->ActorHasTag("TriggerAble") && !isTriggered){
		isTriggered = true;
		UE_LOG(LogClass, Log, TEXT("*** TRIGGER :: TOUCHED ACTOR ***"));
	}
}