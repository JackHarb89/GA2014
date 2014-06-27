

#include "GA.h"
#include "GAWeapon.h"
#include "GACharacter.h"


AGAWeapon::AGAWeapon(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	
}

// If Trigger Overlaps With Another Actor *** OVERRIDE - Checks Actor With Tag "Orc" ***
void AGAWeapon::ReceiveActorBeginOverlap(class AActor* OtherActor){
	if (OtherActor->ActorHasTag("Orc") || OtherActor->ActorHasTag("Destructible")){
		if (!HitedActors.Contains(OtherActor)){
			((AGACharacter*)GetOwner())->DealDamage(OtherActor);
			HitedActors.Add(OtherActor);
			UE_LOG(LogClass, Log, TEXT("*** WEAPON :: DEALT DAMAGE ***"));
		}
	}
}

void AGAWeapon::SetNewOwner(AActor* NewOwner){
	SetOwner(NewOwner);
	((AGACharacter*)GetOwner())->SetWeaponActor(this);
}

void AGAWeapon::RemoveHitedActors(){
	HitedActors.Empty();
}