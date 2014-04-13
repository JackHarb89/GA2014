// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GAAIController.h"
#include "GAEnemy.h"


AGAAIController::AGAAIController(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	BlackboardComp = PCIP.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackBoardComp"));

	BehaviorComp = PCIP.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));
	
	bWantsPlayerState = true;
	PrimaryActorTick.bCanEverTick = true;
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