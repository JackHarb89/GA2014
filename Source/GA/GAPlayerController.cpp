// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GA_HUD.h"
#include "GACharacter.h"
#include "GAWeapon.h"
#include "GAGameMode.h"
#include "GAPlayerController.h"


AGAPlayerController::AGAPlayerController(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{

	GAUserName = "Anonymous";
	PrimaryActorTick.bCanEverTick = true;
}

void AGAPlayerController::PlayerTick(float DeltaTime) {
	Super::PlayerTick(DeltaTime);
	if (GetLevel()->OwningWorld->GetName().Contains("SG_MainMenu") && ((AGA_HUD*)MyHUD)->getSection("escapemenu")){
		bShowMouseCursor = true;
	}
	else{
		bShowMouseCursor = false;
	}
}

#pragma region Networking

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

bool AGAPlayerController::HostGameWithPort(int32 Port){
	bool IsHosting = false;
	UE_LOG(LogClass, Log, TEXT("*** START LISTENING ***"));
	FURL url;
	url.bDefaultsInitialized = true;
	url.Map = "/Game/Maps/SG_MainMenu";
	url.Port = Port;
	IsHosting = GetWorld()->Listen(url);
	return IsHosting;
}

void AGAPlayerController::GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList){
	Super::GetSeamlessTravelActorList(bToEntry, ActorList);
}
void AGAPlayerController::PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel){
	MyHUD->Destroy();
}


#pragma endregion


#pragma region Textchat

void AGAPlayerController::SendChatMessage(const FString& Message){
	if (Role < ROLE_Authority){
		ServerSendChatMessage(Message);
	}
	else{
		GetWorld()->GetAuthGameMode()->Broadcast(this, Message, "Message");
	}

	ChatLog.Insert(Message, 0);
	AGA_HUD* GAHUD = Cast<AGA_HUD>(GetHUD());
	if (GAHUD){
		GAHUD->UpdateChatLog();
	}
}

bool AGAPlayerController::ServerSendChatMessage_Validate(const FString& Message){ return true; }
void AGAPlayerController::ServerSendChatMessage_Implementation(const FString& Message){ SendChatMessage(Message); }

#pragma endregion

#pragma region Textchat

void AGAPlayerController::SetGAUsername(const FString& Username){
	if (Role < ROLE_Authority){
		ServerSetGAUsername(Username);
	}
	else{
		GetWorld()->GetAuthGameMode()->Broadcast(this, Username, "Username");
	}

	GAUserName = Username;
	AGA_HUD* GAHUD = Cast<AGA_HUD>(GetHUD());
	if (GAHUD){
		GAHUD->UpdateChatLog();
	}
}

bool AGAPlayerController::ServerSetGAUsername_Validate(const FString& Username){ return true; }
void AGAPlayerController::ServerSetGAUsername_Implementation(const FString& Username){ SetGAUsername(Username); }

#pragma endregion

void AGAPlayerController::ClientTeamMessage_Implementation(APlayerState* SenderPlayerState, const FString& S, FName Type, float MsgLifeTime){
	Super::ClientTeamMessage_Implementation(SenderPlayerState, S, Type, MsgLifeTime);
	AGA_HUD* GameHUD = Cast<AGA_HUD>(GetHUD());
	if (GameHUD){
		if (SenderPlayerState != PlayerState){
			if (Type == "Message"){
				ChatLog.Insert(S, 0);
				GameHUD->UpdateChatLog();
			}
			else if (Type == "Username"){
				GAUserName = S;
			}
		}
	}
}