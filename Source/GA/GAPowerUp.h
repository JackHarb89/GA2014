

#pragma once

#include "GameFramework/Actor.h"
#include "GAPowerUp.generated.h"


// PowerUp Types
UENUM(BlueprintType)
namespace EGAPowerUp
{
	enum Type
	{
		GAHealthBoost			UMETA(DisplayName = "Health"),
		GAInvulnerability	 	UMETA(DisplayName = "Invulnerability"),
		GAAttackBoost			UMETA(DisplayName = "One Attack Kill"),
	};
}

UCLASS()
class AGAPowerUp : public AActor
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_IsPowerUpActive)							bool IsPowerUpActive;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Type")				TEnumAsByte<EGAPowerUp::Type> PowerUpType;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Type")				bool IsRandomPowerUp;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Type")				bool IsAffectingAll;

	UPROPERTY(Replicated, Transient, EditAnywhere, BlueprintReadWrite, Category = "State")	float HealAmount;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "State")				float CoolDown;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "State")				float EffectDuration;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "State")							float CurrentCoolDown;

	// EVENTS
	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUp Event")						void PowerUpTaken();
	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUp Event")						void PowerUpFinishedCoolDown();

	UFUNCTION()																				void OnRep_IsPowerUpActive();

protected:

	virtual void ReceiveActorBeginOverlap(class AActor* OtherActor) OVERRIDE;
	virtual void Tick(float DeltaTime) OVERRIDE;

private:

	UFUNCTION(server, reliable, WithValidation)												void ServerActivatePowerUpEffect(class AActor* OtherActor);
	void ActivatePowerUpEffect(class AActor* OtherActor);
	void ReduceCoolDown(float DeltaTime);
	void InitPowerUp();

	bool IsInit;
};
