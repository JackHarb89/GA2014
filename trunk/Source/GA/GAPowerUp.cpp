

#include "GA.h"
#include "GAPowerUp.h"
#include "GACharacter.h"
#include "Net/UnrealNetwork.h"


AGAPowerUp::AGAPowerUp(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	IsInit = false;
	IsPowerUpActive = false;
	EffectRadius = 1000;

	IsAffectingAll = false;
	IsAffectingOne = false;

	// Replicate to Server / Clients
	bReplicates = true;
	bAlwaysRelevant = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AGAPowerUp::Tick(float DeltaTime){
	Super::Tick(DeltaTime);
	if(!IsInit) InitPowerUp();
	ReduceCoolDown(DeltaTime);
}

void AGAPowerUp::InitPowerUp(){
	if (Role == ROLE_Authority){
		if (IsRandomPowerUp){
			PowerUpType = (TEnumAsByte<EGAPowerUp::Type>) FMath::RandRange(1, 2);
		}
		IsInit = true;
	}
}

void AGAPowerUp::ReduceCoolDown(float DeltaTime){
	if (Role == ROLE_Authority){
		if (!IsPowerUpActive){
			CurrentCoolDown -= DeltaTime;
			if (CurrentCoolDown <= 0){
				PowerUpFinishedCoolDown();
				IsPowerUpActive = true;
			}
		}
	}
}

void AGAPowerUp::OnRep_IsPowerUpActive(){
	if (IsPowerUpActive){
		PowerUpFinishedCoolDown();
	}
	else{
		PowerUpTaken();
	}
}

void AGAPowerUp::ReceiveActorBeginOverlap(class AActor* OtherActor){
	Super::ReceiveActorBeginOverlap(OtherActor);
	if (OtherActor->ActorHasTag("Player") && IsPowerUpActive){
		ActivatePowerUpEffect(OtherActor);
	}
}

void AGAPowerUp::ActivatePowerUpEffect(class AActor* OtherActor){
	if (Role < ROLE_Authority){
		ServerActivatePowerUpEffect(OtherActor);
	}
	else{
		if (IsAffectingAll){
			for (TActorIterator<AGACharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr){
				if (PowerUpType == EGAPowerUp::Type::GAHealthBoost){
					ActorItr->HealPlayer(HealAmount);
				}
				else {
					ActorItr->ActivatePowerUp(PowerUpType, EffectDuration);
				}
			}
		}
		else if (IsAffectingOne){
			if (PowerUpType == EGAPowerUp::Type::GAHealthBoost){
				((AGACharacter*)OtherActor)->HealPlayer(HealAmount);
			}
			else {
				((AGACharacter*)OtherActor)->ActivatePowerUp(PowerUpType, EffectDuration);
			}
		}
		else{
			for (TActorIterator<AGACharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr){
				if (FVector::Dist(ActorItr->GetActorLocation(), GetActorLocation()) <= EffectRadius){
					if (PowerUpType == EGAPowerUp::Type::GAHealthBoost){
						ActorItr->HealPlayer(HealAmount);
					}
					else {
						ActorItr->ActivatePowerUp(PowerUpType, EffectDuration);
					}
				}
			}
		}

		CurrentCoolDown = CoolDown;
		IsPowerUpActive = false;
		PowerUpTaken();

		if (IsRandomPowerUp){
			PowerUpType = (TEnumAsByte<EGAPowerUp::Type>) FMath::RandRange(1, 2);
		}
	}
}

bool AGAPowerUp::ServerActivatePowerUpEffect_Validate(class AActor* OtherActor){return true;}
void AGAPowerUp::ServerActivatePowerUpEffect_Implementation(class AActor* OtherActor){ActivatePowerUpEffect(OtherActor);}

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
