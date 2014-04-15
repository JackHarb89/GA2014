// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "BTTask_FindEnemy.h"


UBTTask_FindEnemy::UBTTask_FindEnemy(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}

EBTNodeResult::Type UBTTask_FindEnemy::ExecuteTask(class UBehaviorTreeComponent* OwnerComp, uint8* NodeMemory) const
{
	APawn* PlayerPawn;
	if (GetWorld()->GetFirstPlayerController() == NULL) return EBTNodeResult::Failed;

	PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	AAIController* MyAI = Cast<AAIController>(OwnerComp->GetOwner());
	if (PlayerPawn && MyAI && MyAI->GetPawn())
	{
		static int32 MAX_DISTANCE = 200;
		FVector destination;
		FVector distance = PlayerPawn->GetActorLocation() - MyAI->GetPawn()->GetActorLocation();
		//MAX MOVEMENT IN THIS TASK
		if (distance.X > MAX_DISTANCE){ distance.X = MAX_DISTANCE; }
		if (distance.X < -MAX_DISTANCE){ distance.X = -MAX_DISTANCE; }
		if (distance.Y > MAX_DISTANCE){ distance.Y = MAX_DISTANCE; }
		if (distance.Y < -MAX_DISTANCE){ distance.Y = -MAX_DISTANCE; }

		destination = distance + MyAI->GetPawn()->GetActorLocation();
		OwnerComp->GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), destination);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}