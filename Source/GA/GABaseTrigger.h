// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GABaseTrigger.generated.h"

/**
 * 
 */
UCLASS()
class AGABaseTrigger : public AActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(Transient) bool isTriggered;

	virtual void ReceiveActorBeginOverlap(class AActor* OtherActor) OVERRIDE;
};

