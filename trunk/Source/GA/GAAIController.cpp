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
	
	SearchInterval = 0.5;
	SearchTimer = 0;

	UnstuckInterval = 1.5;
	UnstuckTimer = 0;

	LastPosition == FVector::ZeroVector;


	//bWantsPlayerState = true;
	PrimaryActorTick.bCanEverTick = true;
}


void AGAAIController::Tick(float Delta){
	Super::Tick(Delta);
	SearchTimer += Delta;
	UnstuckTimer += Delta;
	if (SearchTimer >= SearchInterval){
		CheckAlternativeMovementWithLOS();
		SearchTimer = 0;
	}
	if (UnstuckTimer >= UnstuckInterval){
		if (GetPawn() && LastPosition == GetPawn()->GetActorLocation()){
			ForceMoveToPlayer();
		}
		UnstuckTimer = 0;
	}
	if (GetPawn()){
		LastPosition = GetPawn()->GetActorLocation();
	}
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

void AGAAIController::ForceMoveToPlayer(){
	if (GetWorld()->GetFirstPlayerController() == NULL) return;
	APawn* EnemyPawn = this->GetPawn();
	if (this && EnemyPawn && ((AGAEnemy*)EnemyPawn)->IsAlive){
		FName PlayerKeyID = "Player";
		uint8 BlackboardKeyID = BehaviorComp->GetBlackboardComponent()->GetKeyID(PlayerKeyID);
		AGACharacter* Player = (AGACharacter*)BehaviorComp->GetBlackboardComponent()->GetValueAsObject(BlackboardKeyID);
		if (Player == NULL) return;

		FName SplitMovementID = "SplitMovement";
		uint8 SplitMovementBlackboardKeyID = BehaviorComp->GetBlackboardComponent()->GetKeyID(SplitMovementID);
		BehaviorComp->GetBlackboardComponent()->SetValueAsBool(SplitMovementBlackboardKeyID, false);

		StopMovement();

	}
}

void AGAAIController::FindClosestPlayer(){
	if (GetWorld()->GetFirstPlayerController() == NULL) return;

	APawn* EnemyPawn = this->GetPawn();
	if (this && EnemyPawn && ((AGAEnemy*)EnemyPawn)->IsAlive){
		FName PlayerKeyID = "Player";
		AGACharacter* ClosestPlayerPawn = (AGACharacter*)GetWorld()->GetFirstPlayerController()->GetPawn();
		for (TActorIterator<AGACharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr){
			if (FVector::Dist(EnemyPawn->GetActorLocation(), ClosestPlayerPawn->GetActorLocation()) >
				FVector::Dist(EnemyPawn->GetActorLocation(), ActorItr->GetActorLocation()) &&
				!(*ActorItr)->HasDied){
				ClosestPlayerPawn = *ActorItr;
			}
		}
		if (ClosestPlayerPawn && ClosestPlayerPawn->HasDied){
			ClosestPlayerPawn = NULL;
		}
		else{
			RemoveTarget();
		}
		uint8 BlackboardKeyID = BehaviorComp->GetBlackboardComponent()->GetKeyID(PlayerKeyID);
		AGACharacter* Player = (AGACharacter*)BehaviorComp->GetBlackboardComponent()->GetValueAsObject(BlackboardKeyID);
		if (Player == NULL || Player != ClosestPlayerPawn){
			StopMovement();
			BehaviorComp->GetBlackboardComponent()->SetValueAsObject(PlayerKeyID, ClosestPlayerPawn);
		}
	}
}

void AGAAIController::CheckPlayerDeath(){
	if (GetWorld()->GetFirstPlayerController() == NULL) return;
	APawn* EnemyPawn = this->GetPawn();
	if (this && EnemyPawn && ((AGAEnemy*)EnemyPawn)->IsAlive){
		FName PlayerKeyID = "Player";
		uint8 BlackboardKeyID = BehaviorComp->GetBlackboardComponent()->GetKeyID(PlayerKeyID);
		AGACharacter* Player = (AGACharacter*)BehaviorComp->GetBlackboardComponent()->GetValueAsObject(BlackboardKeyID);
		if (Player && Player->HasDied) BehaviorComp->GetBlackboardComponent()->SetValueAsObject(PlayerKeyID, NULL);
	}
}

void AGAAIController::CheckIfReachedAltLocation(){
	if (GetWorld()->GetFirstPlayerController() == NULL) return;
	APawn* EnemyPawn = this->GetPawn();
	if (this && EnemyPawn && ((AGAEnemy*)EnemyPawn)->IsAlive){
		FName AltLocationID = "AltLocation";
		uint8 AltLocationBlackboardKeyID = BehaviorComp->GetBlackboardComponent()->GetKeyID(AltLocationID);
		FVector AltLocation = BehaviorComp->GetBlackboardComponent()->GetValueAsVector(AltLocationBlackboardKeyID);

		FName PlayerKeyID = "Player";
		uint8 BlackboardKeyID = BehaviorComp->GetBlackboardComponent()->GetKeyID(PlayerKeyID);
		AGACharacter* Player = (AGACharacter*)BehaviorComp->GetBlackboardComponent()->GetValueAsObject(BlackboardKeyID);
		if (Player == NULL) return;

		if (FVector::Dist(AltLocation, EnemyPawn->GetActorLocation()) < 150){
			BehaviorComp->GetBlackboardComponent()->SetValueAsVector(AltLocationBlackboardKeyID, FVector::ZeroVector);
			FName SplitMovementID = "SplitMovement";
			uint8 SplitMovementBlackboardKeyID = BehaviorComp->GetBlackboardComponent()->GetKeyID(SplitMovementID);
			BehaviorComp->GetBlackboardComponent()->SetValueAsBool(SplitMovementBlackboardKeyID, false);
			StopMovement();
		}

		if ((Player && (FVector::Dist(Player->GetActorLocation(), EnemyPawn->GetActorLocation()) < 150))){
			BehaviorComp->GetBlackboardComponent()->SetValueAsVector(AltLocationBlackboardKeyID, FVector::ZeroVector);
			FName SplitMovementID = "SplitMovement";
			uint8 SplitMovementBlackboardKeyID = BehaviorComp->GetBlackboardComponent()->GetKeyID(SplitMovementID);
			BehaviorComp->GetBlackboardComponent()->SetValueAsBool(SplitMovementBlackboardKeyID, false);
			StopMovement();
		}
	}
}

void AGAAIController::CheckAlternativeMovementWithLOS(){
	if (GetWorld()->GetFirstPlayerController() == NULL) return;
	APawn* EnemyPawn = this->GetPawn();
	if (this && EnemyPawn && ((AGAEnemy*)EnemyPawn)->IsAlive){
		FName SplitMovementID = "SplitMovement";
		uint8 SplitMovementBlackboardKeyID = BehaviorComp->GetBlackboardComponent()->GetKeyID(SplitMovementID);
		if (BehaviorComp->GetBlackboardComponent()->GetValueAsBool(SplitMovementBlackboardKeyID)){
			return;
		}
		FName PlayerKeyID = "Player";
		uint8 BlackboardKeyID = BehaviorComp->GetBlackboardComponent()->GetKeyID(PlayerKeyID);
		AGACharacter* Player = (AGACharacter*)BehaviorComp->GetBlackboardComponent()->GetValueAsObject(BlackboardKeyID);
		if (Player == NULL) return;
		UNavigationSystem* navsystem = GetWorld()->GetNavigationSystem();

		if (FVector::Dist(EnemyPawn->GetActorLocation(), Player->GetActorLocation()) < 150) {
			navsystem->SimpleMoveToActor(this, Player);
			return;
		}

		FHitResult outActor;
		FCollisionQueryParams params;
		params.AddIgnoredActor(EnemyPawn);
		params.AddIgnoredActor(Player);
		FVector eyeHeight = FVector(0, 0, 10);
		GetWorld()->LineTraceSingle(outActor, EnemyPawn->GetActorLocation() + eyeHeight, Player->GetActorLocation() + eyeHeight, ECollisionChannel::ECC_Pawn, params);
		//DrawDebugLine(GetWorld(), EnemyPawn->GetActorLocation() + eyeHeight, Player->GetActorLocation() + eyeHeight, FColor::Blue, false, 5);
		if (outActor.GetActor() && outActor.GetActor()->ActorHasTag("Orc")){
			float SearchRadius = 400;
			FNavLocation destination;
			FVector origin = EnemyPawn->GetActorLocation() + ((Player->GetActorLocation() - EnemyPawn->GetActorLocation()).SafeNormal())*SearchRadius;
			navsystem->GetRandomPointInRadius(origin, SearchRadius/2, destination);

			GetWorld()->LineTraceSingle(outActor, destination.Location + eyeHeight, Player->GetActorLocation() + eyeHeight, ECollisionChannel::ECC_Pawn, params);
			DrawDebugLine(GetWorld(), EnemyPawn->GetActorLocation() + eyeHeight, destination.Location + eyeHeight, FColor::Red, false, 5);
			
			GetWorld()->LineTraceSingle(outActor, destination.Location + FVector(0, 0, 500), destination.Location + FVector(0, 0, -500), ECollisionChannel::ECC_WorldStatic, params);

			FName AltLocationID = "AltLocation";
			uint8 AltLocationBlackboardKeyID = BehaviorComp->GetBlackboardComponent()->GetKeyID(AltLocationID);
			BehaviorComp->GetBlackboardComponent()->SetValueAsVector(AltLocationBlackboardKeyID, outActor.Location);
			

			BehaviorComp->GetBlackboardComponent()->SetValueAsBool(SplitMovementBlackboardKeyID, true);

			StopMovement();
			navsystem->SimpleMoveToLocation(this, destination.Location);
		}
	}
}

void AGAAIController::RemoveTarget(){
	FName TargetID = "Target";
	BehaviorComp->GetBlackboardComponent()->SetValueAsObject(TargetID, NULL);
}