// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GAItem.h"
#include "GAAttackableCharacter.h"
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
	
	TSubclassOf<class AActor> FGameItem::getItemClass(){
		return (UClass*)Item->GeneratedClass;
	}
};



UCLASS()
class AGAEnemy : public AGAAttackableCharacter
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Behavior)				class UBehaviorTree* EnemyBehavior;

	// Loottable
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Items)					TArray<FGameItem> LootTable;

	// Enemy Stats
	UPROPERTY(Replicated, EditAnywhere, Replicated, BlueprintReadWrite, Category = General)		int32 HealthPoints;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = General)					int32 Armor;
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_HasTookDamage)								bool HasTookDamage;

	// Simple Attack
	UPROPERTY(Replicated, Transient)															bool AllowedToAttack;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Simple Attack")			float SimpleAttackDamageMin;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Simple Attack")			float SimpleAttackDamageMax;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Simple Attack")			float SimpleAttackCoolDown;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Simple Attack")			float SimpleAttackRange;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_SimpleAttackOnCoolDown)						bool SimpleAttackOnCoolDown;
	UPROPERTY(Replicated)																		float SimpleAttackCoolDownRestValue;

	// Events
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat Event")							void CharacterAttackedSimple();

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat Event")							void CharacterTookDamage();
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")						void CharacterDied();

	UFUNCTION(reliable, server, WithValidation)													void ServerAttackSimple();
	UFUNCTION(reliable, server, WithValidation)													void ServerReduceSimpleAttackCoolDown(float Delta);

	UFUNCTION(reliable, server, WithValidation)													void ServerCheckDeath();

	UFUNCTION(reliable, server, WithValidation)													void ServerTakeDamageByEnemy(float Damage);
	UFUNCTION(unreliable, server, WithValidation)												void ServerResetHasTookDamage();

	UFUNCTION(reliable, server, WithValidation)													void ServerCheckItemDrop();

	UFUNCTION()																					void OnRep_HasTookDamage();
	UFUNCTION()																					void OnRep_SimpleAttackOnCoolDown();
	
	UFUNCTION(Category = "Damage", BlueprintCallable)											void ApplyDamage(float Damage);


	void TakeDamageByEnemy(float Damage) OVERRIDE;
	bool DealDamage();
	void AttackSimple();
	void ReduceSimpleAttackCoolDown(float DeltaTime);

	bool IsInRange(AActor* target);
	void CheckItemDrop();
	void DropItem(TSubclassOf<class AActor> item);

	// Init
	void InitPlayer();
	bool IsInit;
	bool IsAlive;

	virtual void Tick(float DeltaTime) OVERRIDE;
	void SpawnAIController();
protected:
	void CheckDeath();
};
