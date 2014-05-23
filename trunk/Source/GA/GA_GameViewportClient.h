#pragma once

#include "Engine/GameViewportClient.h"
#include "GA_GameViewportClient.generated.h"

/**
 * 
 */
UCLASS()
class UGA_GameViewportClient : public UGameViewportClient
{
	GENERATED_UCLASS_BODY()

	/**
		Overwriting this function, to also return the value of a key, that was pressed.

		(Used for HUD-input fields [i.e. 
	*/
	virtual bool InputChar(FViewport* Viewport, int32 ControllerId, TCHAR Character) OVERRIDE;
	
};