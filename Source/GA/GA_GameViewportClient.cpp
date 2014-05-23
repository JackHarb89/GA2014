#include "GA.h"
#include "GA_HUD.h"
#include "GA_GameViewportClient.h"

UGA_GameViewportClient::UGA_GameViewportClient(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{}

bool UGA_GameViewportClient::InputChar(FViewport* InViewport, int32 ControllerId, TCHAR Character) {

	bool result;
	result = Super::InputChar(InViewport, ControllerId, Character);

	// should probably just add a ctor to FString that takes a TCHAR
	FString CharacterString;
	CharacterString += Character;

	UE_LOG(LogClass, Log, TEXT("*** Current key: %s ***"), *CharacterString);

	/************************************************************************/
	/* CUSTOM                                                               */
	/************************************************************************/
	((AGA_HUD*)(GEngine->GetGamePlayer(GEngine->GameViewport, 0)->PlayerController->MyHUD))->ParseKeyInput(CharacterString);

	return result;
}