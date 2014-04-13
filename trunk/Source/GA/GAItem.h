// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GA.h"
#include "GameFramework/Actor.h"
#include "GAItem.generated.h"

/**
 * 
 */

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
};

UCLASS()
class AGAItem : public AActor
{
	GENERATED_UCLASS_BODY()

	FName ItemName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)			int32 ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AuraPlayer)		FPlayerAura AuraPlayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AuraGroup)		FGroupAura AuraGroup;
};
