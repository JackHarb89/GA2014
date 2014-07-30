

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

	UPROPERTY(Replicated)																	bool IsPowerUpActive;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Type")				TEnumAsByte<EGAPowerUp::Type> PowerUpType;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Type")				bool IsRandomPowerUp;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Type")				bool IsAffectingAll;


	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "State")				float CoolDown;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "State")							float CurrentCoolDown;

	// EVENTS
	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUp Event")						void PowerUpTaken();
	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUp Event")						void PowerUpFinishedCoolDown();

protected:

	virtual void ReceiveActorBeginOverlap(class AActor* OtherActor) OVERRIDE;
	virtual void Tick(float DeltaTime) OVERRIDE;

private:

	UFUNCTION(server, reliable, WithValidation)												void ServerActivatePowerUpEffect();
	void ActivePowerUpEffect();
	void ReduceCoolDown(float DeltaTime);
};
