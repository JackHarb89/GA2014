// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GA.h"
#include "GameFramework/Actor.h"
#include "GAItem.generated.h"

/**
 * 
 */
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

UENUM(BlueprintType)
namespace EGASlot
{
	enum Type
	{
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

	FGroupAura(){
		PercentDamage = 0;
		PercentArmor = 0;
		PercentHealth = 0;
		PercentAttackSpeed = 0;
		PercentMovementSpeed = 0;
		EffectRadius = 0;
	}
};

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

	FPlayerAura(){
		PercentDamage = 0;
		PercentArmor = 0;
		PercentHealth = 0;
		PercentAttackSpeed = 0;
		PercentMovementSpeed = 0;
	}
};

USTRUCT(BlueprintType)
struct FStats
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")						float Armor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")						float Attack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")						float AttackSpeed;
};

UCLASS()
class AGAItem : public AActor
{
	GENERATED_UCLASS_BODY()

	FName ItemName;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Item")			int32 ItemID;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Item")			TEnumAsByte<EGASlot::Type> Slot; 
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Item")			bool IsMoney;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Item")			TEnumAsByte<EGARarity::Type> Rarity;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Item")			int32 Value;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Aura Player")	FPlayerAura AuraPlayer;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Aura Group")	FGroupAura AuraGroup;

	UPROPERTY(Replicated)																bool finishedDropAnimation;

	UFUNCTION(BlueprintCallable, Category = "Drop Animation")							void SetDropAnimationFinished();
	UFUNCTION(reliable, server, WithValidation)											void ServerSetDropAnimationFinished();
};
