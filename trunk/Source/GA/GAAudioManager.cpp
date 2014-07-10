#include "GA.h"
#include "GAEnemy.h"
#include "GAAudioManager.h"


AGAAudioManager::AGAAudioManager(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	BattleMusicIsActive = false;
	PrimaryActorTick.bCanEverTick = true;
}

void AGAAudioManager::Tick(float Delta){
	Super::Tick(Delta);
	CheckBattleMusicFade();
}

// Checks if Enemies are on Map and Alive. If so Fade Into Battle Music. If not Fade Into Silence Music
void AGAAudioManager::CheckBattleMusicFade(){
	bool EnemyOnMap = false;
	for (TActorIterator<AGAEnemy> ActorItr(GetWorld()); ActorItr; ++ActorItr){
		if ((*ActorItr)->IsAlive){
			EnemyOnMap = true;
			break;
		}
	}
	if (EnemyOnMap && !BattleMusicIsActive){
		for (TActorIterator<AGAAudioManager> ActorItr(GetWorld()); ActorItr; ++ActorItr){
			(*ActorItr)->FadeInBattle();
		}
		BattleMusicIsActive = true;
	}
	else if (!EnemyOnMap && BattleMusicIsActive){
		for (TActorIterator<AGAAudioManager> ActorItr(GetWorld()); ActorItr; ++ActorItr){
			(*ActorItr)->FadeInSilence();
		}
		BattleMusicIsActive = false;
	}
}