// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GASpawnTrigger.generated.h"

/**
 * 
 */
UCLASS()
class AGASpawnTrigger : public AActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY() bool isTriggered;

	virtual void ReceiveActorBeginOverlap(class AActor* OtherActor) OVERRIDE;

};

