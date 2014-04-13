// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GASpawnTrigger.h"


AGASpawnTrigger::AGASpawnTrigger(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	isTriggered = false;
}


void AGASpawnTrigger::ReceiveActorBeginOverlap(class AActor* OtherActor){
	if (OtherActor->ActorHasTag("TriggerAble") && !isTriggered){
		isTriggered = true;
		UE_LOG(LogClass, Log, TEXT("*** TRIGGER :: TOUCHED ACTOR ***"));
	}
}