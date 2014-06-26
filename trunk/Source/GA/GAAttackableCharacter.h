

#pragma once

#include "GameFramework/Character.h"
#include "GAAttackableCharacter.generated.h"

/**
 * 
 */
UCLASS()
class AGAAttackableCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

	virtual void TakeDamageByEnemy(float Damage);
};
