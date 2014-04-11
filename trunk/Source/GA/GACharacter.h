// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "GameFramework/SpringArmComponent.h"
#include "GACharacter.generated.h"

UCLASS(config=Game)
class AGACharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

	// Simple Attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SimpleAttack)	float SimpleAttackDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SimpleAttack)	float SimpleAttackCoolDown;
	float SimpleAttackCoolDownRestValue;
	bool SimpleAttackOnCoolDown;

	// Special Attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpecialAttack)	float SpecialAttackBaseDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpecialAttack)	float SpecialAttackMaxDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpecialAttack)	float SpecialAttackChargeInterval;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpecialAttack)	float SpecialAttackCoolDown;
	float SpecialAttackCoolDownRestValue;
	float SpecialAttackChargeTimer;
	int32 SpecialAttackTimesCharged;
	bool SpecialAttackOnCoolDown;
	bool SpecialAttackIsCharging;

	// Player Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerStats)	float HealthPoints;

	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	TSubobjectPtr<class USpringArmComponent> CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	TSubobjectPtr<class UCameraComponent> FollowCamera;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;
	
	void TakeDamageByEnemy(float Damage);
	UFUNCTION(BlueprintImplementableEvent, Category = CombatEvent) void CharacterTookDamage();


protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) OVERRIDE;
	// End of APawn interface


	
	void AttackSimple();
	void ReduceSimpleAttackCoolDown(float DeltaTime);

	void ChargeSpecial();
	void IncreaseChargeTime(float DeltaTime);
	void AttackSpecial();
	float CalculateSpecialAttackDamage();
	void ReduceSpecialAttackCoolDown(float DeltaTime);

	bool IsInRange(AActor* target);

	void CheckDeath();

	virtual void Tick(float DeltaTime) OVERRIDE;
};

