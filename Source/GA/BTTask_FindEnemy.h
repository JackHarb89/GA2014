// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "BTTask_FindEnemy.generated.h"

/**
 * 
 */
UCLASS()
class UBTTask_FindEnemy : public UBTTask_BlackboardBase
{
	GENERATED_UCLASS_BODY()

	virtual EBTNodeResult::Type ExecuteTask(class UBehaviorTreeComponent* OwnerComp, uint8* NodeMemory) OVERRIDE;
};
