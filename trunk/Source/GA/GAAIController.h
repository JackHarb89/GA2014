// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/AIController.h"
#include "GAAIController.generated.h"

/**
 * 
 */
UCLASS()
class AGAAIController : public AAIController
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(transient)	
		TSubobjectPtr<class UBlackboardComponent> BlackboardComp;

	UPROPERTY(transient)	
		TSubobjectPtr<class UBehaviorTreeComponent> BehaviorComp;

	UFUNCTION(BlueprintCallable, Category = Behavior)
		void FindClosestPlayer();
	UFUNCTION(BlueprintCallable, Category = Behavior)
		void CheckPlayerDeath();
	UFUNCTION(BlueprintCallable, Category = Behavior)
		void RemoveTarget();
	UFUNCTION(BlueprintCallable, Category = Behavior)
		void CheckAlternativeMovementWithLOS();

	UFUNCTION(BlueprintCallable, Category = Behavior)
		void CheckIfReachedAltLocation();

	UFUNCTION(BlueprintCallable, Category = Behavior)
		void ForceMoveToPlayer();

	virtual void Possess(class APawn* InPawn) OVERRIDE;
	
	int32 SearchInterval;
	float SearchTimer;

	FVector LastPosition;

	int32 UnstuckInterval;
	float UnstuckTimer;

	virtual void Tick(float Delta) OVERRIDE;
};
