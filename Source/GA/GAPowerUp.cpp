

#include "GA.h"
#include "GAPowerUp.h"
#include "Net/UnrealNetwork.h"


AGAPowerUp::AGAPowerUp(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	IsPowerUpActive = false;

	// Replicate to Server / Clients
	bReplicates = true;
	bAlwaysRelevant = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AGAPowerUp::Tick(float DeltaTime){
	Super::Tick(DeltaTime);
	ReduceCoolDown(DeltaTime);
}

void AGAPowerUp::ReduceCoolDown(float DeltaTime){
	if (Role == ROLE_Authority){
		CurrentCoolDown -= DeltaTime;
		if (CurrentCoolDown <= 0){
			PowerUpFinishedCoolDown();
			IsPowerUpActive = true;
		}
	}
}

void AGAPowerUp::ReceiveActorBeginOverlap(class AActor* OtherActor){
	Super::ReceiveActorBeginOverlap(OtherActor);
	if (OtherActor->ActorHasTag("Player")){
		ActivePowerUpEffect();
	}
}

void AGAPowerUp::ActivePowerUpEffect(){
	if (Role < ROLE_Authority){
		ServerActivatePowerUpEffect();
	}
	else{
		CurrentCoolDown = CoolDown;
		IsPowerUpActive = false;
		PowerUpTaken();
	}
}

bool AGAPowerUp::ServerActivatePowerUpEffect_Validate(){return true;}
void AGAPowerUp::ServerActivatePowerUpEffect_Implementation(){ ActivePowerUpEffect(); }

// Replicates Replicated Attributes
void AGAPowerUp::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AGAPowerUp, IsPowerUpActive);
	DOREPLIFETIME(AGAPowerUp, PowerUpType);
	DOREPLIFETIME(AGAPowerUp, IsRandomPowerUp);
	DOREPLIFETIME(AGAPowerUp, IsAffectingAll);
	DOREPLIFETIME(AGAPowerUp, CoolDown);
	DOREPLIFETIME(AGAPowerUp, CurrentCoolDown);
}
