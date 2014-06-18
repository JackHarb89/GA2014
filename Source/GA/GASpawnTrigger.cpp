// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GACharacter.h"
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
	else if (OtherActor->ActorHasTag("Orc") && ActorHasTag("BaseTrigger")){
		for (TActorIterator<AGACharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr){
			ActorItr->ReduceBaseHealth();
		}
		OtherActor->DestroyConstructedComponents();
		OtherActor->Destroy();
		UE_LOG(LogClass, Log, TEXT("*** TRIGGER :: TOUCHED ACTOR ORC ***"));
	}
}