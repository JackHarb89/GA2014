// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "BTTask_FindNextDestination.h"
#include "GACharacter.h"


UBTTask_FindNextDestination::UBTTask_FindNextDestination(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}

EBTNodeResult::Type UBTTask_FindNextDestination::ExecuteTask(class UBehaviorTreeComponent* OwnerComp, uint8* NodeMemory)
{
	if (GetWorld()->GetFirstPlayerController() == NULL) return EBTNodeResult::Failed;

	AAIController* MyAI = Cast<AAIController>(OwnerComp->GetOwner());
	APawn* EnemyPawn = MyAI->GetPawn();
	if (MyAI && EnemyPawn){
		FName PlayerKeyID = "Player";
		uint8 BlackboardKeyID = OwnerComp->GetBlackboardComponent()->GetKeyID(PlayerKeyID);
		AGACharacter* ClosestPlayerPawn = Cast<AGACharacter>(OwnerComp->GetBlackboardComponent()->GetValueAsObject(BlackboardKeyID));
		

		FVector direction = ClosestPlayerPawn->GetActorLocation() - EnemyPawn->GetActorLocation();
		direction.Normalize();
		float originalDistance = FVector::Dist(EnemyPawn->GetActorLocation(), ClosestPlayerPawn->GetActorLocation());

		FVector tempDestination = EnemyPawn->GetActorLocation() + direction * originalDistance / 2;

		int32 Searchradius = 50;
		FVector destination = UNavigationSystem::GetRandomPointInRadius(MyAI, tempDestination, Searchradius);
		
		OwnerComp->GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), destination);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}



