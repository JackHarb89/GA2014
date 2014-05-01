// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "GameFramework/SpringArmComponent.h"
#include "GAItem.h"
#include "GAShop.h"
#include "GACharacter.generated.h"

USTRUCT(BlueprintType)
struct FEquipment
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")						AGAItem* Head;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")						AGAItem* Chest;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")						AGAItem* Weapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")						AGAItem* Trinket00;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")						AGAItem* Trinket01;
};

UCLASS()
class AGACharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

	// Chat
	UPROPERTY(Replicated, Transient, ReplicatedUsing = OnRep_ChatMessages)					TArray<FString> ChatLog;

	// Movement
	UPROPERTY(Replicated)																	float BaseMovementSpeed;

	// Shop
	UClass* ShopClass;
	UPROPERTY(Replicated)																	AGAShop* Shop;
	UPROPERTY(Replicated)																	AGAItem* ShopItem;

	// Aura
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_HasActivatedAura, BlueprintReadWrite, Category = "Aura")							bool HasActivatedAura;
	UPROPERTY(Replicated)																	FGroupAura AuraBonus;
	UPROPERTY(Replicated, Transient)														TArray<AGACharacter*> AuraPlayer;
	UPROPERTY(Replicated)																	FGroupAura OtherPlayerAura;

	// Items
	UPROPERTY(Replicated)																	float ItemDamage;
	UPROPERTY(Replicated)																	float ItemHealth;

	// Potion
	UPROPERTY(Replicated, Transient, EditAnywhere, BlueprintReadWrite, Category = "Potion")	int32 Potions;
	UPROPERTY(Replicated, Transient, EditAnywhere, BlueprintReadWrite, Category = "Potion")	float PotionCoolDown;
	UPROPERTY(Replicated)																	float PotionCoolDownResetValue;

	// Attack Speed
	UPROPERTY(Replicated)																	float AttackSpeed;

	// Crit Chance
	UPROPERTY(Replicated, Transient, VisibleAnywhere, Category = "Simple Attack")			float Critical;

	// Armor
	UPROPERTY(Replicated, Transient, VisibleAnywhere, BlueprintReadWrite, Category = "Player Stats")			float Armor;
	UPROPERTY(Replicated, Transient, VisibleAnywhere, Category = "Player Stats")			float ArmorReduction;
	UPROPERTY(Replicated)																	float ArmorReductionPercent;
	UPROPERTY(Replicated, Transient)														float ArmorResetValue;

	// Ressource
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Ressource")			float Ressource;

	// Equip
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Item")				FEquipment EquipItems;

	// Inventory
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Item")				TArray<AGAItem*> InventoryItems;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Item")				int32 InventorySlots;


	// Simple Attack
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Simple Attack")		float SimpleAttackDamage;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Simple Attack")		float SimpleAttackCoolDown;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Simple Attack")		float SimpleAttackRange;
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_SimpleAttackOnCoolDown)					bool SimpleAttackOnCoolDown;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Simple Attack")					float SimpleAttackCoolDownResetValue;

	// Special Attack
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Special Attack")	float SpecialAttackBaseDamage;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Special Attack")	float SpecialAttackMaxCharges;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Special Attack")	float SpecialAttackChargeInterval;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Special Attack")	float SpecialAttackCoolDown;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Special Attack")	float SpecialAttackRange;

	UPROPERTY(Replicated, BlueprintReadWrite, ReplicatedUsing = OnRep_SpecialAttackTimesCharged, Category = "Special Attack")	int32 SpecialAttackTimesCharged;
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_SpecialAttackOnCoolDown)					bool SpecialAttackOnCoolDown;
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_SpecialAttackIsCharging)					bool SpecialAttackIsCharging;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Special Attack")					float SpecialAttackCoolDownResetValue;
	UPROPERTY(Replicated)																	float SpecialAttackChargeTimer;

	// Player Stats
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Player Stats")		float HealthPoints;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Player Stats")		float OutOfCombatTime;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Player Stats")		float RegenerationAmount;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Player Stats")		float RegenerationRate;

	UPROPERTY(Replicated)																	float RegenerationTimer;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Player Stats")					float MaxHealth;
	UPROPERTY(Replicated)																	float HealthResetValue;
	UPROPERTY(Replicated)																	float RegenerationTime;
	UPROPERTY(Replicated)																	bool RegenerationAnimationIsRunning;
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_AllowedToRegenerate)						bool AllowedToRegenerate;
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_HasTookDamage)							bool HasTookDamage;
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_HasDied)									bool HasDied;
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_HasPickedUpItem)							bool HasPickedUpItem;
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_HasEquipedItem)							bool HasEquipedItem;
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_HasUsedPotion)							bool HasUsedPotion;
	UPROPERTY(Replicated)																	AGAItem* TouchedItem;
	
	// EVENTS
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")					void CharacterAttackedSimple();
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")					void CharacterAttackedSpecial();
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")					void CharacterStartedCharging();
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")					void CharacterIsCharging();
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")					void CharacterTookDamage();
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")					void CharacterStartedRegeneration();
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")					void CharacterFinishedRegeneration();
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")					void CharacterPickedUpItem();
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")					void CharacterEquipedItem();
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")					void CharacterUsedPotion();
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")					void CharacterActivatedAura();
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")					void CharacterDeactivatedAura();
	
	// Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")						TSubobjectPtr<class USpringArmComponent> CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")						TSubobjectPtr<class UCameraComponent> FollowCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")						float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")						float BaseLookUpRate;

	// Server Simple Attack
	UFUNCTION(reliable, server, WithValidation)												void ServerAttackSimple();
	UFUNCTION(reliable, server, WithValidation)												void ServerReduceSimpleAttackCoolDown(float Delta);

	// Server Special Attack
	UFUNCTION(reliable, server, WithValidation)												void ServerAttackSpecial();
	UFUNCTION(reliable, server, WithValidation)												void ServerReduceSpecialAttackCoolDown(float Delta);
	UFUNCTION(reliable, server, WithValidation)												void ServerChargeSpecial();
	UFUNCTION(reliable, server, WithValidation)												void ServerIncreaseChargeTime(float Delta);

	// Server Regeneration
	UFUNCTION(reliable, server, WithValidation)												void ServerRegenerateHealth(float Delta);
	UFUNCTION(reliable, server, WithValidation)												void ServerTakeDamageByEnemy(float Damage);

	// Server Check Death
	UFUNCTION(reliable, server, WithValidation)												void ServerCheckDeath();

	// Server Items
	UFUNCTION(reliable, server, WithValidation)												void ServerPickUpItem(AGAItem* item);
	UFUNCTION(reliable, server, WithValidation)												void ServerEquipItem(AGAItem* item);
	UFUNCTION(reliable, server, WithValidation)												void ServerCalculateItems();
	UFUNCTION(reliable, server, WithValidation)												void ServerResetHasPickedUpItem();
	UFUNCTION(reliable, server, WithValidation)												void ServerResetHasEquipedItem();

	// Server Potions
	UFUNCTION(reliable, server, WithValidation)												void ServerUsePotion();
	UFUNCTION(reliable, server, WithValidation)												void ServerReducePotionCoolDown(float Delta);

	// Server Shop
	UFUNCTION(reliable, server, WithValidation)												void ServerBuyItem();
	UFUNCTION(reliable, server, WithValidation)												void ServerSellItem(AGAItem* item);

	// Server Aura
	UFUNCTION(reliable, server, WithValidation)												void ServerActivateAura();
	UFUNCTION(reliable, server, WithValidation)												void ServerDeactivateAura();
	UFUNCTION(reliable, server, WithValidation)												void ServerCalculateAura();
	UFUNCTION(reliable, server, WithValidation)												void ServerCheckPlayerInAuraRange();

	// Server Chat
	UFUNCTION(reliable, server, WithValidation)												void ServerSendChatMessage(const FString& Message);

	
	// Replication Notify Functions
	UFUNCTION()																				void OnRep_SimpleAttackOnCoolDown();
	UFUNCTION()																				void OnRep_SpecialAttackOnCoolDown();
	UFUNCTION()																				void OnRep_SpecialAttackIsCharging();
	UFUNCTION()																				void OnRep_SpecialAttackTimesCharged();
	UFUNCTION()																				void OnRep_AllowedToRegenerate();
	UFUNCTION()																				void OnRep_HasTookDamage();
	UFUNCTION()																				void OnRep_HasDied();
	UFUNCTION()																				void OnRep_HasPickedUpItem();
	UFUNCTION()																				void OnRep_HasEquipedItem(); 
	UFUNCTION()																				void OnRep_HasUsedPotion();
	UFUNCTION()																				void OnRep_HasActivatedAura();
	UFUNCTION()																				void OnRep_ChatMessages();

	// Public Function To Call To Take Damage
	void TakeDamageByEnemy(float Damage);


protected:
	// TMP DUE TO NO UI
	void SendMessage();

	// Chat
	void SendChatMessage(const FString& Message);
	void AddMessageToChatLog(const FString& Message);

	// Aura
	void ActivateAura();
	void DeactivateAura();
	void CheckPlayerInAuraRange();
	void CalculateAura();

	// Items
	void EquipItem(AGAItem* item);
	void PickUpItem(AGAItem* item);
	void CalculateItems();
	void BuyItem();
	void SellItem(AGAItem* item);

	// *** TEMPORARY DUE TO NO UI ***
	void SellLastItem();

	// Potion
	void UsePotion();
	void ReducePotionCoolDown(float Delta);

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

	// Regeneration
	void RegenerateHealth(float Delta);

	// Simple Attack
	void ReduceSimpleAttackCoolDown(float Delta);

	// Special Attack
	void IncreaseChargeTime(float Delta);
	void ReduceSpecialAttackCoolDown(float Delta);
	float CalculateSpecialAttackDamage();
	bool isAllowedToMove();

	// Attack Range
	bool IsInRange(AActor* target, float attackRange);

	// Init
	void InitPlayer();
	bool isInit;

	// General
	void CheckDeath();
	virtual void ReceiveActorBeginOverlap(class AActor* OtherActor) OVERRIDE;
	virtual void Tick(float Delta) OVERRIDE;
};

