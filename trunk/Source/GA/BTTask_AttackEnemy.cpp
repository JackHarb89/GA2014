// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "BTTask_AttackEnemy.h"
#include "GAEnemy.h"
#include "GACharacter.h"


UBTTask_AttackEnemy::UBTTask_AttackEnemy(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}


EBTNodeResult::Type UBTTask_AttackEnemy::ExecuteTask(class UBehaviorTreeComponent* OwnerComp, uint8* NodeMemory)
{
	AAIController* MyAI = Cast<AAIController>(OwnerComp->GetOwner());
	if (MyAI && MyAI->GetPawn())
	{
		FName PlayerKeyID = "Player";
		uint8 BlackboardKeyID = OwnerComp->GetBlackboardComponent()->GetKeyID(PlayerKeyID);
		AGACharacter* ClosestPlayerPawn = Cast<AGACharacter>(OwnerComp->GetBlackboardComponent()->GetValueAsObject(BlackboardKeyID));

		if (ClosestPlayerPawn){
			FVector PlayerLocation = ClosestPlayerPawn->GetActorLocation();
			FVector EnemyLocation = MyAI->GetPawn()->GetActorLocation();
			AGAEnemy* enemy = (AGAEnemy*)MyAI->GetCharacter();

			if (FVector::Dist(PlayerLocation, EnemyLocation) <= enemy->SimpleAttackRange){
				if (enemy->DealDamage()) return EBTNodeResult::Succeeded;
			}
		}
	}

	return EBTNodeResult::Failed;
}

