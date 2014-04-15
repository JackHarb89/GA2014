// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerController.h"
#include "GAItem.h"
#include "GACharacter.h"
#include "GAEnemy.h"
#include "GAPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class AGAPlayerController : public APlayerController
{
	GENERATED_UCLASS_BODY()

public:

	// Items
	void EquipItem(AGAItem* item);
	void PickUpItem(AGAItem* item);

	// Combat
	void AttackSimple();
	void AttackSpecial();
	void ChargeSpecial();
	void TakeDamageByEnemy(float Damage);
	bool isAllowedToMove();

protected:

	// Items
	float ItemDamage;
	float ItemHealth;

	void CalculateItem(AGAItem* item);

	// Simple Attack
	float SimpleAttackCoolDownRestValue;
	bool SimpleAttackOnCoolDown;

	void ReduceSimpleAttackCoolDown(float DeltaTime);

	// Special Attack
	float SpecialAttackCoolDownRestValue;
	float SpecialAttackChargeTimer;
	int32 SpecialAttackTimesCharged;
	bool SpecialAttackOnCoolDown;
	bool SpecialAttackIsCharging;

	void IncreaseChargeTime(float DeltaTime);
	void ReduceSpecialAttackCoolDown(float DeltaTime);
	float CalculateSpecialAttackDamage();

	// Attack Range
	bool IsInRange(AActor* target);

	// Playerstats
	bool AllowedToRegenerate;
	float RegenerationTimer;
	float MaxHP;
	float RegenerationTime;

	void RegenerateHP(float DeltaTime);
	void CheckDeath();

	// Init Controller
	bool isInit;
	void InitPlayer();

	// General
	virtual void Tick(float DeltaTime) OVERRIDE;

	// Character Reference
	AGACharacter* gaCharacter;

};
