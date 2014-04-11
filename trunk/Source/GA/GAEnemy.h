// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GAEnemy.generated.h"

/**
 * 
 */


UCLASS()
class AGAEnemy : public ACharacter
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category = Behavior)	class UBehaviorTree* EnemyBehavior;
	UPROPERTY(EditAnywhere, Category = General)		int32 HealthPoints;

	// Simple Attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SimpleAttack)	float SimpleAttackDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SimpleAttack)	float SimpleAttackCoolDown;
	float SimpleAttackCoolDownRestValue;
	bool SimpleAttackOnCoolDown;

	void TakeDamageByEnemy(float Damage);
	bool DealDamage();
	void AttackSimple();
	void ReduceSimpleAttackCoolDown(float DeltaTime);

	bool IsInRange(AActor* target);

	virtual void Tick(float DeltaTime) OVERRIDE;
	void SpawnAIController();
	
	UFUNCTION(BlueprintImplementableEvent, Category = CombatEvent) void CharacterTookDamage();
};
