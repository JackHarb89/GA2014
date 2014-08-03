// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GA_HUD.h"
#include "GACharacter.h"
#include "GAWeapon.h"
#include "GAPlayerController.h"


AGAPlayerController::AGAPlayerController(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{
}

void AGAPlayerController::PlayerTick(float DeltaTime) {
	Super::PlayerTick(DeltaTime);/*
	if (((AGA_HUD*)MyHUD)->activeTypingArea == nullptr && !((AGA_HUD*)MyHUD)->getSection("inventory") && !((AGA_HUD*)MyHUD)->getSection("escapemenu")) {
		bShowMouseCursor = false;
	}
	else {
		bShowMouseCursor = true;
	}
	*/
}

// Connecting To Given Server IP. IP Example: "127.0.0.1:7777"
void AGAPlayerController::ConnectToServer(const FString& ip){
	UE_LOG(LogClass, Log, TEXT("*** TRYING TO CONNECT TO SERVER ***"));
	ClientTravel(ip, TRAVEL_Absolute, false);
}

void AGAPlayerController::ChangeMap(const FString& mapName){
	UE_LOG(LogClass, Log, TEXT("*** CHANGING MAP ***"));
	FString UrlString = TEXT("/Game/Maps/" + mapName);
	GetWorld()->ServerTravel(UrlString);
}

void AGAPlayerController::HostGameWithPort(int32 Port){
	UE_LOG(LogClass, Log, TEXT("*** START LISTENING ***"));
	FURL url;
	url.bDefaultsInitialized = true;
	url.Map = "/Game/Maps/SG_MainMenu";
	url.Port = Port;
	GetWorld()->Listen(url);
}

void AGAPlayerController::GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList){
	Super::GetSeamlessTravelActorList(bToEntry, ActorList);
	for (int32 i = 0; i < ((AGA_HUD*)MyHUD)->currentSpawnedAreas.Num(); i++){
		ActorList.Add(((AGA_HUD*)MyHUD)->currentSpawnedAreas[i]);
	}
}