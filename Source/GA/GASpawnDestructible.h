// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GAAttackableCharacter.h"
#include "GASpawnDestructible.generated.h"

/**
 * 
 */
UCLASS()
class AGASpawnDestructible : public AGAAttackableCharacter
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Type")		bool IsMountainHeart;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Stats")		float HealtPoints;

	UFUNCTION(BlueprintImplementableEvent, Category = CombatEvent)					void DestructibleTookDamage();
	UFUNCTION(BlueprintImplementableEvent, Category = CombatEvent)					void DestructibleWasDestroyed();
	
	UFUNCTION(reliable, server, WithValidation)										void ServerTakeDamageByEnemy(float Damage);

	void TakeDamageByEnemy(float Damage) OVERRIDE;

};
