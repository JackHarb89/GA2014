// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GACharacter.h"
#include "BTTask_FindEnemy.h"


UBTTask_FindEnemy::UBTTask_FindEnemy(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}

EBTNodeResult::Type UBTTask_FindEnemy::ExecuteTask(class UBehaviorTreeComponent* OwnerComp, uint8* NodeMemory)
{
	
	if (GetWorld()->GetFirstPlayerController() == NULL) return EBTNodeResult::Failed;

	AAIController* MyAI = Cast<AAIController>(OwnerComp->GetOwner());
	APawn* EnemyPawn = MyAI->GetPawn();
	if (MyAI && EnemyPawn){
		FName PlayerKeyID = "Target";
		uint8 BlackboardKeyID = OwnerComp->GetBlackboardComponent()->GetKeyID(PlayerKeyID);

		AActor* Target = ((AActor*) OwnerComp->GetBlackboardComponent()->GetValueAsObject(BlackboardKeyID));
		if (Target) {
			UNavigationSystem::SimpleMoveToActor(MyAI, Target);
			//MyAI->MoveToActor(Target);
			return EBTNodeResult::Succeeded;
		}
		else {
			MyAI->StopMovement();
			return EBTNodeResult::Failed;
		}
	}
	return EBTNodeResult::Failed;
}