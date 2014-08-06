

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

	virtual														void ReceiveActorBeginOverlap(class AActor* OtherActor) OVERRIDE;

	UPROPERTY(Replicated)										TArray<AActor*> HitedActors;
	
	void RemoveHitedActors();
	
	UFUNCTION(reliable, server, WithValidation)					
		void ServerRemoveHitedActors();

	UFUNCTION(reliable, server, WithValidation)					
		void ServerSetNewOwner(AActor* NewOwner);
	UFUNCTION(Category = "Owner", BlueprintCallable)			
		void SetNewOwner(AActor* NewOwner);
};
