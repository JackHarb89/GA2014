// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "GameFramework/SpringArmComponent.h"
#include "GAItem.h"
#include "GAShop.h"
#include "GAWeapon.h"
#include "GAAttackableCharacter.h"
#include "GAAudioManager.h"
#include "GAPowerUp.h"
#include "GACharacter.generated.h"

// actual inventory
USTRUCT()
struct FInventoryRow {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<AGAItem*> column;

	FInventoryRow(int32 avalibleColumns) {
		for (int i = 0; i < avalibleColumns; i++) {
			column.Add(nullptr);
		}
	}

	FInventoryRow() {}
};

USTRUCT()
struct FGA_Inventory {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<FInventoryRow> rows;

	int32 columnAmount;
	int32 rowAmount;

	FGA_Inventory () {
		columnAmount = 5;
		rowAmount = 3;
		for (int i = 0; i < rowAmount; i++) {
			rows.Add(FInventoryRow(columnAmount));
		}
	}

 	bool newSlot(int32 &row, int32 &col) {
		for (int i = 0; i < rowAmount; i++) {
			for (int j = 0; j < columnAmount; j++) {
				if (rows[i].column[j] == nullptr) {
					row = i;
					col = j;
					return true;
				}
			}
		}

		return false;
	}

	void registerElement(AGAItem* newItem) {
		// get first free slot
		int32 row = 0;
		int32 col = 0;
		if (newSlot(row, col))
			rows[row].column[col] = newItem;
		else 
			UE_LOG(LogClass, Log, TEXT("*** Found no new slot! (That's kinda bad - or the inventory is simply full...) ***"));
	}

	bool setItem(int32 row, int32 col, AGAItem* item) {
		if (row < rowAmount && col < columnAmount) {
			rows[row].column[col] = item;
			return true;
		}

		return false;
	}

	bool clearElement(int row, int col) {
		bool returnThis = rows[row].column[col] != nullptr;

		// garbage clean the actual "item" array, since we only delete the reference
		rows[row].column[col] = nullptr;

		return returnThis;
	}
};
// actual inventory end

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
class AGACharacter : public AGAAttackableCharacter
{
	GENERATED_UCLASS_BODY()

	// Power UP
	UPROPERTY(Replicated)																	bool IsInvulnerable;
	UPROPERTY(Replicated)																	bool IsOneAttackKill;

	// Shard
	UPROPERTY(Replicated)																	bool ShardAvailable;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Shard")				float ShardCoolDown;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Shard")							float ShardCurrentCoolDown;
	
	// Inventory
	UPROPERTY(BlueprintReadWrite, Category = "Inventory management")
	FGA_Inventory inventory;
	
	// Chat
	UPROPERTY(BlueprintReadWrite, Replicated, Transient, ReplicatedUsing = OnRep_UserName, Category = "Chat")					FString UserName;
	UPROPERTY(BlueprintReadWrite, Replicated, Transient, ReplicatedUsing = OnRep_ChatMessages, Category = "Chat")				TArray<FString> ChatLog;

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
	UPROPERTY(Replicated)																	AGAWeapon* WeaponActor;

	// Inventory
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Item")				TArray<AGAItem*> InventoryItems;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Item")				int32 InventorySlots;


	// Simple Attack
	UPROPERTY(Replicated)																	bool IsSimpleAttacking;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Simple Attack")		float SimpleAttackDamage;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Simple Attack")		float SimpleAttackCoolDown;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Simple Attack")		float SimpleAttackRange;
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_SimpleAttackOnCoolDown)					bool SimpleAttackOnCoolDown;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Simple Attack")					float SimpleAttackCoolDownResetValue;

	// Special Attack
	UPROPERTY(Replicated)																	bool IsSpecialAttacking;
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
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_HasBeenHealed)							bool HasBeenHealed;
	UPROPERTY(Replicated)																	AGAItem* TouchedItem;
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_IsPowerUpActive)							bool IsPowerUpActive;
	UPROPERTY(Replicated)																	TEnumAsByte<EGAPowerUp::Type> ActivePowerUp;
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_PowerUpDuration)							float PowerUpDuration;
	
	// EVENTS
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")					void CharacterAppliedSimpleForce();
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")					void CharacterAppliedSpecialForce();
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")					void CharacterAttackedSimple();
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")					void CharacterAttackedSpecial();
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")					void CharacterStartedCharging();
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")					void CharacterIsCharging();
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")					void CharacterStartedRegeneration();
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")					void CharacterFinishedRegeneration();
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")					void CharacterPickedUpItem();
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")					void CharacterEquipedItem();
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")					void CharacterActivatedAura();
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")					void CharacterDeactivatedAura();
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")					void CharacterChangedName();
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")					void CharacterDied();
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")					void CharacterTookDamage();
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")					void CharacterActivatedPowerUp(EGAPowerUp::Type PowerUpType);
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Event")					void CharacterDeactivatedPowerUp();
	UFUNCTION(BlueprintImplementableEvent, Category = "Game Event")							void CharacterActivatedShard();
	UFUNCTION(BlueprintImplementableEvent, Category = "Game Event")							void CharacterLostGame();
	UFUNCTION(BlueprintImplementableEvent, Category = "Game Event")							void CharacterWonGame();
	
	// Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")						TSubobjectPtr<class USpringArmComponent> CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")						TSubobjectPtr<class UCameraComponent> FollowCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")						float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")						float BaseLookUpRate;

	// Server Attack
	UFUNCTION(reliable, server, WithValidation)												void ServerDealDamage(class AActor* OtherActor);

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

	// Server Shop
	UFUNCTION(reliable, server, WithValidation)												void ServerBuyItem();
	UFUNCTION(reliable, server, WithValidation)												void ServerSellItem(AGAItem* item);

	// Server Aura
	UFUNCTION(reliable, server, WithValidation)												void ServerActivateAura();
	UFUNCTION(reliable, server, WithValidation)												void ServerDeactivateAura();
	UFUNCTION(reliable, server, WithValidation)												void ServerCalculateAura();
	UFUNCTION(reliable, server, WithValidation)												void ServerCheckPlayerInAuraRange();

	// Server Chat
	UFUNCTION(Category = "Chat", BlueprintCallable, reliable, server, WithValidation)		void ServerSendChatMessage(const FString& Message);
	UFUNCTION(Category = "Chat", BlueprintCallable, reliable, server, WithValidation)		void ServerChangeUserName(const FString& Message);

	// Equip Weapon
	UFUNCTION(reliable, server, WithValidation)												void ServerSetWeaponActor(AGAWeapon *Weapon);

	// Power Ups
	UFUNCTION(reliable, server, WithValidation)												void ServerActivatePowerUp(EGAPowerUp::Type PowerUpType, float EffectDuration);
	UFUNCTION(reliable, server, WithValidation)												void ServerDeactivatePowerUp();
	UFUNCTION(reliable, server, WithValidation)												void ServerHealPlayer(float HealAmount);

	// Shard Usage
	UFUNCTION(reliable, server, WithValidation)												void ServerActivateShard();
	
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
	UFUNCTION()																				void OnRep_HasBeenHealed();
	UFUNCTION()																				void OnRep_HasActivatedAura();
	UFUNCTION()																				void OnRep_ChatMessages();
	UFUNCTION()																				void OnRep_UserName();
	UFUNCTION()																				void OnRep_IsPowerUpActive();
	UFUNCTION()																				void OnRep_PowerUpDuration();

	// Public Function To Call To Take Damage
	void TakeDamageByEnemy(float Damage) OVERRIDE;

	// Chat
	UFUNCTION(exec)																			void SendChatMessage(const FString& Message);
	
	// have to be public to be called by blueprint classes
	UFUNCTION(Category = "Shop", BlueprintCallable)											void BuyItem();
	UFUNCTION(Category = "Shop", BlueprintCallable)											void SellItem(AGAItem* item);
	UFUNCTION(Category = "Chat", BlueprintCallable)											void ChangeUserName(const FString& Message);

	UFUNCTION(Category = "Combat", BlueprintCallable)										bool IsCharging();
	
	UFUNCTION(Category = "Damage", BlueprintCallable)										void ApplyDamage(float Damage);

	UFUNCTION(Category = "Damage", BlueprintCallable)										void SetIsSimpleAttackingTo(bool NewState);
	UFUNCTION(Category = "Damage", BlueprintCallable)										void SetIsSpecialAttackingTo(bool NewState);
	UFUNCTION(Category = "Inventory", BlueprintCallable)									void UnequipItem(EGASlot::Type itemType);
	UFUNCTION(Category = "Inventory", BlueprintCallable)									void EquipItem(AGAItem* item);
	UFUNCTION(Category = "Inventory", BlueprintCallable)									void SetItemSlot(int32 row, int32 col, AGAItem* item);
	UFUNCTION(Category = "Inventory", BlueprintCallable)									bool ClearItemSlot(int32 row, int32 col);

	void DealDamage(class AActor* OtherActor);
	void SetWeaponActor(AGAWeapon *Weapon);
	void ActivatePowerUp(EGAPowerUp::Type PowerUpType, float EffectDuration);
	void HealPlayer(float HealAmount);

protected:

	// Power Up
	void ReducePowerUpDuration(float DeltaTime);
	void DeactivatePowerUp();

	// Shard
	void ActivateShard();
	void ReduceShardCoolDown(float DeltaTime);

	// Chat
	// made this function public, so the UI can launch them
	void AddMessageToChatLog(const FString& Message);

	// Aura
	void ActivateAura();
	void DeactivateAura();
	void CheckPlayerInAuraRange();
	void CalculateAura();

	// Items
	void PickUpItem(AGAItem* item);
	void CalculateItems();

	// *** TEMPORARY DUE TO NO UI ***
	void SellLastItem();
	
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

