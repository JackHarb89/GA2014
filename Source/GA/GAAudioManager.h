#pragma once

#include "GameFramework/Actor.h"
#include "GAPowerUp.h"
#include "GAAudioManager.generated.h"

/**
 * 
 */
UCLASS()
class AGAAudioManager : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	// UI
	UFUNCTION(BlueprintImplementableEvent, Category = "Audio Event") 				void UI_AreaClicked(AActor* area);
	UFUNCTION(BlueprintImplementableEvent, Category = "Audio Event") 				void UI_AreaHover(AActor* area);
	UFUNCTION(BlueprintImplementableEvent, Category = "Audio Event") 				void UI_MenuForward(AActor* area);
	UFUNCTION(BlueprintImplementableEvent, Category = "Audio Event") 				void UI_MenuBackward(AActor* area);
	UFUNCTION(BlueprintImplementableEvent, Category = "Audio Event") 				void UI_ChatMessage(AActor* area);
	UFUNCTION(BlueprintImplementableEvent, Category = "Audio Event") 				void UI_EscapeMenu(AActor* area);

	// Music
	UFUNCTION(BlueprintImplementableEvent, Category = "Audio Event") 				void FadeOutBattle();
	UFUNCTION(BlueprintImplementableEvent, Category = "Audio Event")				void FadeInBattle();

	// Sounds
	UFUNCTION(BlueprintImplementableEvent, Category = "Audio Event")				void EnemyDied(AGAEnemy* Enemy);
	UFUNCTION(BlueprintImplementableEvent, Category = "Audio Event")				void EnemyTookDamage(AGAEnemy* Enemy);
	UFUNCTION(BlueprintImplementableEvent, Category = "Audio Event")				void EnemyDropedGold(AGAEnemy* Enemy);
	UFUNCTION(BlueprintImplementableEvent, Category = "Audio Event")				void EnemyDropedItem(AGAEnemy* Enemy);

	UFUNCTION(BlueprintImplementableEvent, Category = "Audio Event")				void CharacterDied(AGACharacter* Character);
	UFUNCTION(BlueprintImplementableEvent, Category = "Audio Event")				void CharacterAttackedSimple(AGACharacter* Character);
	UFUNCTION(BlueprintImplementableEvent, Category = "Audio Event")				void CharacterAttackedSpecial(AGACharacter* Character);
	UFUNCTION(BlueprintImplementableEvent, Category = "Audio Event")				void CharacterUsedPotion(AGACharacter* Character);
	UFUNCTION(BlueprintImplementableEvent, Category = "Audio Event")				void CharacterBoughtPotion(AGACharacter* Character);
	UFUNCTION(BlueprintImplementableEvent, Category = "Audio Event")				void CharacterBoughtItem(AGACharacter* Character);
	UFUNCTION(BlueprintImplementableEvent, Category = "Audio Event")				void CharacterPickedUpItem(AGACharacter* Character);
	UFUNCTION(BlueprintImplementableEvent, Category = "Audio Event")				void CharacterDropedItem(AGACharacter* Character);
	UFUNCTION(BlueprintImplementableEvent, Category = "Audio Event")				void CharacterEquipedItem(AGACharacter* Character);
	UFUNCTION(BlueprintImplementableEvent, Category = "Audio Event")				void CharacterStartedShardChanneling(AGACharacter* Character);

	UFUNCTION(BlueprintImplementableEvent, Category = "Audio Event")				void PowerUpTaken(AGAPowerUp* PowerUp);

};
