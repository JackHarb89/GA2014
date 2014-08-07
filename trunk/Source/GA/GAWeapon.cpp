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
	Super::ReceiveActorBeginOverlap(OtherActor);
	if (OtherActor->ActorHasTag("Orc")){
		if (!HitedActors.Contains(OtherActor)){
			((AGACharacter*)GetOwner())->DealDamage(OtherActor);
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
	if (Role < ROLE_Authority){
		ServerRemoveHitedActors();
	}
	else{
		HitedActors.Empty();
	}
}


bool AGAWeapon::ServerRemoveHitedActors_Validate(){ return true; }
void AGAWeapon::ServerRemoveHitedActors_Implementation(){ RemoveHitedActors(); }

// Replicates All Replicated Properties
void AGAWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Spectating
	DOREPLIFETIME(AGAWeapon, HitedActors);
}