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

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Behavior)				class UBehaviorTree* EnemyBehavior;

	// Loottable
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Items)					TArray<FGameItem> LootTable;

	// Enemy Stats
	UPROPERTY(Replicated, EditAnywhere, Replicated, BlueprintReadWrite, Category = General)		int32 HealthPoints;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = General)					int32 Armor;
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_HasTookDamage)								bool HasTookDamage;

	// Simple Attack
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = SimpleAttack)			float SimpleAttackDamageMin;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = SimpleAttack)			float SimpleAttackDamageMax;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = SimpleAttack)			float SimpleAttackCoolDown;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_SimpleAttackOnCoolDown)						bool SimpleAttackOnCoolDown;
	UPROPERTY(Replicated)																		float SimpleAttackCoolDownRestValue;

	// Events
	UFUNCTION(BlueprintImplementableEvent, Category = CombatEvent)								void CharacterAttackedSimple();
	UFUNCTION(BlueprintImplementableEvent, Category = CombatEvent)								void CharacterTookDamage();

	UFUNCTION(reliable, server, WithValidation)													void ServerAttackSimple();
	UFUNCTION(reliable, server, WithValidation)													void ServerReduceSimpleAttackCoolDown(float Delta);

	UFUNCTION(reliable, server, WithValidation)													void ServerTakeDamageByEnemy(float Damage);
	UFUNCTION(unreliable, server, WithValidation)												void ServerResetHasTookDamage();

	UFUNCTION(reliable, server, WithValidation)													void ServerCheckItemDrop();

	UFUNCTION()																					void OnRep_HasTookDamage();
	UFUNCTION()																					void OnRep_SimpleAttackOnCoolDown();



	void TakeDamageByEnemy(float Damage);
	bool DealDamage();
	void AttackSimple();
	void ReduceSimpleAttackCoolDown(float DeltaTime);

	bool IsInRange(AActor* target);
	void CheckItemDrop();
	void DropItem(TSubclassOf<class AActor> item);

	virtual void Tick(float DeltaTime) OVERRIDE;
	void SpawnAIController();
};
