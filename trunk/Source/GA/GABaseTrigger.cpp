// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GACharacter.h"
#include "GABaseTrigger.h"
#include "GAGameState.h"


AGABaseTrigger::AGABaseTrigger(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
}

// If Trigger Overlaps With Another Actor *** OVERRIDE - Checks Actor With Tag "TriggerAble" ***
void AGABaseTrigger::ReceiveActorBeginOverlap(class AActor* OtherActor){
	Super::ReceiveActorBeginOverlap(OtherActor);
	if (OtherActor->ActorHasTag("Orc") && GetWorld()->GetGameState<AGAGameState>()){
		OtherActor->DestroyConstructedComponents();
		OtherActor->Destroy();
		UE_LOG(LogClass, Log, TEXT("*** TRIGGER :: TOUCHED ACTOR ORC ***"));
	}
}