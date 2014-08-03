#include "GA.h"
#include "GA_HUD.h"
#include "GACharacter.h"
#include "GA_GameViewportClient.h"

UGA_GameViewportClient::UGA_GameViewportClient(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{}

bool UGA_GameViewportClient::InputChar(FViewport* InViewport, int32 ControllerId, TCHAR Character) {

	bool result;
	result = Super::InputChar(InViewport, ControllerId, Character);

	FString CharacterString;
	CharacterString += Character;

	/************************************************************************/
	/* CUSTOM                                                               */
	/************************************************************************/
	((AGA_HUD*)(GEngine->GetGamePlayer(GEngine->GameViewport, 0)->PlayerController->MyHUD))->ParseKeyInput(CharacterString);

	return result;
}