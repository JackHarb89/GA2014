// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GAItem.h"
#include "GAEnemy.generated.h"

/**
 * 
 */
USTRUCT()
struct FGameItem
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)			float DropChance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)			UBlueprint* Item;
	
	TSubclassOf<class AActor> getItemClass(){
		return (UClass*)Item->GeneratedClass;
	}
};



UCLASS()
class AGAEnemy : public ACharacter
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Behavior)		class UBehaviorTree* EnemyBehavior;

	// Loottable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items)			TArray<FGameItem> LootTable;

	// Enemy Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = General)			int32 HealthPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = General)			int32 Armor;

	// Simple Attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SimpleAttack)	float SimpleAttackDamageMin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SimpleAttack)	float SimpleAttackDamageMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SimpleAttack)	float SimpleAttackCoolDown;
	float SimpleAttackCoolDownRestValue;
	bool SimpleAttackOnCoolDown;

	void TakeDamageByEnemy(float Damage);
	bool DealDamage();
	void AttackSimple();
	void ReduceSimpleAttackCoolDown(float DeltaTime);

	bool IsInRange(AActor* target);
	void CheckItemDrop();
	void DropItem(TSubclassOf<class AActor> item);

	virtual void Tick(float DeltaTime) OVERRIDE;
	void SpawnAIController();
	
	UFUNCTION(BlueprintImplementableEvent, Category = CombatEvent) void CharacterTookDamage();
};
