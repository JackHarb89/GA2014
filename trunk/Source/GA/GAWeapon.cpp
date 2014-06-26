

#include "GA.h"
#include "GAWeapon.h"
#include "GACharacter.h"


AGAWeapon::AGAWeapon(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	
}

// If Trigger Overlaps With Another Actor *** OVERRIDE - Checks Actor With Tag "Orc" ***
void AGAWeapon::ReceiveActorBeginOverlap(class AActor* OtherActor){
	if (OtherActor->ActorHasTag("Orc")){
		((AGACharacter*)GetOwner())->DealDamage();
		UE_LOG(LogClass, Log, TEXT("*** WEAPON :: TOUCHED ACTOR ORC ***"));
	}
}


void AGAWeapon::SetNewOwner(AActor* NewOwner){
	SetOwner(NewOwner);
}