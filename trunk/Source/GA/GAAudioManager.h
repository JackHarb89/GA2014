#pragma once

#include "GameFramework/Actor.h"
#include "GAAudioManager.generated.h"

/**
 * 
 */
UCLASS()
class AGAAudioManager : public AActor
{
	GENERATED_UCLASS_BODY()

	bool BattleMusicIsActive;
	// Music
	UFUNCTION(BlueprintImplementableEvent, Category = "Audio Event") 				void FadeInSilence();
	UFUNCTION(BlueprintImplementableEvent, Category = "Audio Event")				void FadeInBattle();

	void CheckBattleMusicFade();


	virtual void Tick(float Delta) OVERRIDE;
};
