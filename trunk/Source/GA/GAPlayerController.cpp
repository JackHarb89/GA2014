// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GAPlayerController.h"


AGAPlayerController::AGAPlayerController(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{
	bShowMouseCursor = false;	// currently set to false to allow regular playing - will be set by the UI in the future
}

// Connecting To Given Server IP. IP Example: "127.0.0.1:7777"
void AGAPlayerController::ConnectToServer(const FString& ip){
	ClientTravel((TEXT("%s"), *ip), TRAVEL_Absolute, false);
}

// Start Listening For Connections. Map Example: "Example_Map"
void AGAPlayerController::HostGameWithMap(const FString& mapName){
	GetWorld()->ServerTravel((TEXT("%s?Listen"), *mapName), true, true);
}