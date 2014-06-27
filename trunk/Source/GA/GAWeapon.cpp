#include "GA.h"
#include "GAWeapon.h"
#include "GACharacter.h"
#include "Net/UnrealNetwork.h"


AGAWeapon::AGAWeapon(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// Replicate to Server / Clients
	bReplicates = true;
	bAlwaysRelevant = true;
	PrimaryActorTick.bCanEverTick = true;
}

// If Trigger Overlaps With Another Actor *** OVERRIDE - Checks Actor With Tag "Orc" ***
void AGAWeapon::ReceiveActorBeginOverlap(class AActor* OtherActor){
	if (OtherActor->ActorHasTag("Orc") || OtherActor->ActorHasTag("Destructible")){
		if (!HitedActors.Contains(OtherActor)){
			((AGACharacter*)GetOwner())->DealDamage(OtherActor);
			HitedActors.Add(OtherActor);
		}
	}
}

void AGAWeapon::SetNewOwner(AActor* NewOwner){
	if (Role < ROLE_Authority){
		ServerSetNewOwner(NewOwner);
	}
	else{
		SetOwner(NewOwner);
		((AGACharacter*)GetOwner())->SetWeaponActor(this);
	}
}

bool AGAWeapon::ServerSetNewOwner_Validate(AActor* NewOwner){return true;}
void AGAWeapon::ServerSetNewOwner_Implementation(AActor* NewOwner){SetNewOwner(NewOwner);}

void AGAWeapon::RemoveHitedActors(){
	HitedActors.Empty();
}