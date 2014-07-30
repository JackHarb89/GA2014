// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GA_HUD.h"
#include "GAPlayerController.h"


AGAPlayerController::AGAPlayerController(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{
	ListenPort = 7777;
}

void AGAPlayerController::PlayerTick(float DeltaTime) {
	Super::PlayerTick(DeltaTime);
	if (((AGA_HUD*)MyHUD)->activeTypingArea == nullptr && !((AGA_HUD*)MyHUD)->getSection("inventory") && !((AGA_HUD*)MyHUD)->getSection("escapemenu")) {
		bShowMouseCursor = false;
	}
	else {
		bShowMouseCursor = true;
	}
}

// Connecting To Given Server IP. IP Example: "127.0.0.1:7777"
void AGAPlayerController::ConnectToServer(const FString& ip){
	ClientTravel((TEXT("%s"), *ip), TRAVEL_Absolute,true);
}

// Start Listening For Connections. Map Example: "Example_Map" *** CHANGE CURRENT MAP DID NOT WORK ATM ***
void AGAPlayerController::HostGameWithMap(const FString& mapName){
	FURL url = GetWorld()->URL;
	url.Map = "/Game/Maps/"+mapName;
	url.Port = ListenPort;
	GetWorld()->Listen(url);
}

void AGAPlayerController::SetListenPort(int32 newPort){
	ListenPort = newPort;
}