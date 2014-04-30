// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GA.h"
#include "GameFramework/Actor.h"
#include "GAItem.generated.h"

// Rarity Types
UENUM(BlueprintType)
namespace EGARarity
{
	enum Type
	{
		GACommon		UMETA(DisplayName = "Common"),
		GARare	 		UMETA(DisplayName = "Rare"),
		GAEpic			UMETA(DisplayName = "Epic"),
	};
}

// Slot Types
UENUM(BlueprintType)
namespace EGASlot
{
	enum Type
	{
		GAMoney			UMETA(DisplayName = "Money"),
		GAHead 			UMETA(DisplayName = "Head"),
		GAChest 		UMETA(DisplayName = "Chest"),
		GAWeapon		UMETA(DisplayName = "Weapon"),
		GATrinket		UMETA(DisplayName = "Trinket"),
	};
}

// Aura For Group
USTRUCT(BlueprintType)
struct FGroupAura
{
	GENERATED_USTRUCT_BODY()

	// Values
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AuraGroup)		float PercentDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AuraGroup)		float PercentArmor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AuraGroup)		float PercentHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AuraGroup)		float PercentAttackSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AuraGroup)		float PercentMovementSpeed;

	// Radius
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AuraGroup)		float EffectRadius;

	// Return If Any Value > 0 is Set
	bool FGroupAura::hasAura(){
		if (PercentDamage > 0 || PercentArmor > 0 || PercentHealth > 0 || PercentAttackSpeed > 0 || PercentMovementSpeed > 0){
			return true;
		}
		return false;
	}

	// Construction With 0
	FGroupAura(){
		PercentDamage = 0;
		PercentArmor = 0;
		PercentHealth = 0;
		PercentAttackSpeed = 0;
		PercentMovementSpeed = 0;
		EffectRadius = 0;
	}
};

// Aura For Player Only
USTRUCT(BlueprintType)
struct FPlayerAura
{
	GENERATED_USTRUCT_BODY()

	// Values
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AuraPlayer)		float PercentDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AuraPlayer)		float PercentArmor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AuraPlayer)		float PercentHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AuraPlayer)		float PercentAttackSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AuraPlayer)		float PercentMovementSpeed;

	// Construction With 0
	FPlayerAura(){
		PercentDamage = 0;
		PercentArmor = 0;
		PercentHealth = 0;
		PercentAttackSpeed = 0;
		PercentMovementSpeed = 0;
	}
};

// Item Stats
USTRUCT(BlueprintType)
struct FStats
{
	GENERATED_USTRUCT_BODY()

	// Attack
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")					float Attack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")						float AttackLowRoll;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")						float AttackLowHigh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")						float AttackSpeedInPercent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")						float CriticalInPercent;

	// Armor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")					float Armor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")						float ArmorLowRoll;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")						float ArmorLowHigh;

	// Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")						float Health;
	
	// Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")						float MovementInPercent;

	// Set Attack To A Random Between Min And Max Attack
	void CalculateAttack(){
		Attack = FMath::RandRange(AttackLowRoll, AttackLowHigh);
	}

	// Set Armor To A Random Between Min And Max Armor
	void CalculateArmor(){
		Armor = FMath::RandRange(ArmorLowRoll, ArmorLowHigh);
	}

	// Construction With 0
	FStats(){
		// Attack
		Attack = 0;
		AttackLowRoll = 0;
		AttackLowHigh = 0;
		AttackSpeedInPercent = 0;
		CriticalInPercent = 0;

		// Armor
		Armor = 0;
		ArmorLowRoll = 0;
		ArmorLowHigh = 0;

		// Health
		Health = 0;

		// Movement
		MovementInPercent = 0;
	}

};

UCLASS()
class AGAItem : public AActor
{
	GENERATED_UCLASS_BODY()

	// General
	FName ItemName;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Item")			int32 ItemID;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Item")			TEnumAsByte<EGASlot::Type> Slot;
	
	// Value
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Item")			TEnumAsByte<EGARarity::Type> Rarity;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Item")			float Value;

	// Item Stats
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Stats")			FStats ItemStats;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Aura Player")	FPlayerAura AuraPlayer;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Aura Group")	FGroupAura AuraGroup;

	// Drop Finished
	UPROPERTY(Replicated, Transient)													bool finishedDropAnimation;
	
	// Server Drop Finished
	UFUNCTION(reliable, server, WithValidation)											void ServerSetDropAnimationFinished();

	// Blueprint Call Drop Finished
	UFUNCTION(BlueprintCallable, Category = "Drop Animation")							void SetDropAnimationFinished();
};
