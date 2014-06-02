// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GACharacter.h"
#include "GAAIController.h"
#include "GAEnemy.h"


AGAAIController::AGAAIController(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	BlackboardComp = PCIP.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackBoardComp"));

	BehaviorComp = PCIP.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));
	
	//bWantsPlayerState = true;
	//PrimaryActorTick.bCanEverTick = true;
}

void AGAAIController::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);

	AGAEnemy* Enemy = Cast<AGAEnemy>(InPawn);
	// start behavior
	if (Enemy && Enemy->EnemyBehavior)
	{
		BlackboardComp->InitializeBlackboard(Enemy->EnemyBehavior->BlackboardAsset);
		BehaviorComp->StartTree(Enemy->EnemyBehavior);
	}
}

void AGAAIController::FindClosestPlayer(){
	if (GetWorld()->GetFirstPlayerController() == NULL) return;

	APawn* EnemyPawn = this->GetPawn();
	if (this && EnemyPawn){
		float AggroRange = 1500;
		FName PlayerKeyID = "Player";
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
		else{
			RemoveTarget();
		}
		BehaviorComp->GetBlackboardComponent()->SetValueAsObject(PlayerKeyID, ClosestPlayerPawn);
	}
}

void AGAAIController::RemoveTarget(){
	FName TargetID = "Target";
	BehaviorComp->GetBlackboardComponent()->SetValueAsObject(TargetID, NULL);
}