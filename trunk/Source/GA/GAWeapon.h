

#pragma once

#include "GameFramework/Actor.h"
#include "GAWeapon.generated.h"

/**
 * 
 */
UCLASS()
class AGAWeapon : public AActor
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(Category = "Owner", BlueprintCallable)			void SetNewOwner(AActor* NewOwner);
	virtual														void ReceiveActorBeginOverlap(class AActor* OtherActor) OVERRIDE;
	void RemoveHitedActors();

	TArray<AActor*> HitedActors;
	UFUNCTION(reliable, server, WithValidation)					void ServerSetNewOwner(AActor* NewOwner);
};
