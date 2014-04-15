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

UCLASS()
class AGAItem : public AActor
{
	GENERATED_UCLASS_BODY()

	FName ItemName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)			int32 ItemID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)			int32 Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)			TEnumAsByte<EGARarity::Type> Rarity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)			TEnumAsByte<EGASlot::Type> Slot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AuraPlayer)		FPlayerAura AuraPlayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AuraGroup)		FGroupAura AuraGroup;

	UFUNCTION(BlueprintCallable, Category = "Drop Animation")				void SetDropAnimationFinished();
	bool finishedDropAnimation;
};
