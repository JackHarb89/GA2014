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
		float AggroRange = 1500;
		APawn* ClosestPlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		for (TActorIterator<AGACharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr){
			if (FVector::Dist(EnemyPawn->GetActorLocation(), ClosestPlayerPawn->GetActorLocation()) >
				FVector::Dist(EnemyPawn->GetActorLocation(), ActorItr->GetActorLocation())){
				ClosestPlayerPawn = *ActorItr;
			}
		}
		if (FVector::Dist(EnemyPawn->GetActorLocation(), ClosestPlayerPawn->GetActorLocation()) > AggroRange){
			ClosestPlayerPawn = NULL;
		}
		OwnerComp->GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), ClosestPlayerPawn);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}