

#pragma once

#include "AI/BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_SetFinalDestination.generated.h"

/**
 * 
 */
UCLASS()
class UBTTask_SetFinalDestination : public UBTTask_BlackboardBase
{
	GENERATED_UCLASS_BODY()

	
	virtual EBTNodeResult::Type ExecuteTask(class UBehaviorTreeComponent* OwnerComp, uint8* NodeMemory) OVERRIDE;
};
