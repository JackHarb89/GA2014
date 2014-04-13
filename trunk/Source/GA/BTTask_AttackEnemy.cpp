// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "BTTask_AttackEnemy.h"
#include "GAEnemy.h"


UBTTask_AttackEnemy::UBTTask_AttackEnemy(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}


EBTNodeResult::Type UBTTask_AttackEnemy::ExecuteTask(class UBehaviorTreeComponent* OwnerComp, uint8* NodeMemory) const
{
	APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	AAIController* MyAI = Cast<AAIController>(OwnerComp->GetOwner());
	if (PlayerPawn && MyAI && MyAI->GetPawn())
	{
		static int32 MAX_ATTACK_RANGE = 100;
		FVector PlayerLocation = PlayerPawn->GetActorLocation();
		FVector EnemyLocation = MyAI->GetPawn()->GetActorLocation();
		AGAEnemy* enemy = (AGAEnemy*) MyAI->GetCharacter();

		// Check If In Range			*** WIP ***
		if (abs(PlayerLocation.X - EnemyLocation.X) < MAX_ATTACK_RANGE && abs(PlayerLocation.Y - EnemyLocation.Y) < MAX_ATTACK_RANGE){
			if (enemy->DealDamage()) EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}