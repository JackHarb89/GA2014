// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "GameFramework/SpringArmComponent.h"
#include "GAItem.h"
#include "GACharacter.generated.h"

USTRUCT(BlueprintType)
struct FEquipment
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")			AGAItem* Helm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")			AGAItem* Chest;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")			AGAItem* Weapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")			AGAItem* Trinket00;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")			AGAItem* Trinket01;
};

UCLASS(config=Game)
class AGACharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

	// Equip
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")				FEquipment EquipItems;

	// Inventory
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")				TArray<AGAItem*> InventoryItems;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")				int32 InventorySlots;


	// Simple Attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimpleAttack")		float SimpleAttackDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimpleAttack")		float SimpleAttackCoolDown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimpleAttack")		float SimpleAttackRange;

	// Special Attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpecialAttack")		float SpecialAttackBaseDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpecialAttack")		float SpecialAttackMaxDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpecialAttack")		float SpecialAttackChargeInterval;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpecialAttack")		float SpecialAttackCoolDown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpecialAttack")		float SpecialAttackRange;

	// Player Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")		float HealthPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")		float Armor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")		float OutOfCombatTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")		float RegenerationAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")		float RegenerationRate;

	// EVENTS
	UFUNCTION(BlueprintImplementableEvent, Category = "CombatEvent")			void CharacterAttackedSimple();
	UFUNCTION(BlueprintImplementableEvent, Category = "CombatEvent")			void CharacterAttackedSpecial();
	UFUNCTION(BlueprintImplementableEvent, Category = "CombatEvent")			void CharacterStartedCharging();
	UFUNCTION(BlueprintImplementableEvent, Category = "CombatEvent")			void CharacterIsCharging();
	UFUNCTION(BlueprintImplementableEvent, Category = "CombatEvent")			void CharacterTookDamage();
	UFUNCTION(BlueprintImplementableEvent, Category = "CombatEvent")			void CharacterRegenerated();
	
	// Cmaera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")			TSubobjectPtr<class USpringArmComponent> CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")			TSubobjectPtr<class UCameraComponent> FollowCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")			float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")			float BaseLookUpRate;

protected:

	// Input Settings
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) OVERRIDE;
	
	// Movement
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	// Combat
	void AttackSimple();
	void AttackSpecial();
	void ChargeSpecial();

	// General
	virtual void ReceiveActorBeginOverlap(class AActor* OtherActor) OVERRIDE;
};

