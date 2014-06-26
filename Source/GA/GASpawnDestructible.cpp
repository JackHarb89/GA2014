// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GASpawnDestructible.h"
#include "Net/UnrealNetwork.h"
#include "GAEnemySpawn.h"
#include "GAGameState.h"


AGASpawnDestructible::AGASpawnDestructible(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	IsMountainHeart = false;
	HealtPoints = 100;
	bReplicates = true;
	bAlwaysRelevant = true;
	Tags.Add("Destructible");
}

void AGASpawnDestructible::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// General
	DOREPLIFETIME(AGASpawnDestructible, HealtPoints);
}

void AGASpawnDestructible::TakeDamageByEnemy(float Damage){
	if (Role < ROLE_Authority){
		ServerTakeDamageByEnemy(Damage);
	}
	else {
		HealtPoints -= Damage;
		DestructibleTookDamage();
		if (HealtPoints <= 0){
			Cast <AGAEnemySpawn>(GetOwner())->DestroySpawn();
			if (IsMountainHeart) {
				GetWorld()->GetGameState<AGAGameState>()->FinishGame();
			}
			else DestructibleWasDestroyed();
		}
	}
}

bool AGASpawnDestructible::ServerTakeDamageByEnemy_Validate(float Damage){return true;}
void AGASpawnDestructible::ServerTakeDamageByEnemy_Implementation(float Damage){TakeDamageByEnemy(Damage);}


