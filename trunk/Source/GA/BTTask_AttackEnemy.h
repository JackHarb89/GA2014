// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AI/BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_AttackEnemy.generated.h"

/**
 * 
 */
UCLASS()
class UBTTask_AttackEnemy : public UBTTask_BlackboardBase
{
	GENERATED_UCLASS_BODY()

	virtual EBTNodeResult::Type ExecuteTask(class UBehaviorTreeComponent* OwnerComp, uint8* NodeMemory) OVERRIDE;

};
