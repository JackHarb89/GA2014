// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GA_HUD.h"
#include "GACharacter.h"
#include "GAWeapon.h"
#include "GAGameMode.h"
#include "GAGameState.h"
#include "GAPlayerController.h"
#include "Net/UnrealNetwork.h"


AGAPlayerController::AGAPlayerController(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{
	bReplicates = true;
	bAlwaysRelevant = true;

	bNetLoadOnClient = true;

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
	const FString usernameip = ip;
	UE_LOG(LogClass, Log, TEXT("*** TRYING TO CONNECT TO SERVER ***"));
	ClientTravel(usernameip, TRAVEL_Absolute, false);
	
}

void AGAPlayerController::ChangeMap(const FString& mapName){
	UE_LOG(LogClass, Log, TEXT("*** CHANGING MAP ***"));
	FString UrlString = TEXT("/Game/Maps/" + mapName);
	GetWorld()->ServerTravel(UrlString);

}

void AGAPlayerController::Disconnect(){
	if (GetNetDriver()){
		GetNetDriver()->Shutdown();
	}
}

bool AGAPlayerController::HostGameWithPort(int32 Port){
	bool IsHosting = false;
	UE_LOG(LogClass, Log, TEXT("*** START LISTENING ***"));
	FURL url;
	url.bDefaultsInitialized = true;
	url.Map = "/Game/Maps/SG_MainMenu";
	url.Port = Port;
	url.Protocol = "unreal";
	IsHosting = GetWorld()->Listen(url);
	return IsHosting;
}

void AGAPlayerController::GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList){
	Super::GetSeamlessTravelActorList(bToEntry, ActorList);
}
void AGAPlayerController::PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel){
	if (GetCharacter()){
		((AGACharacter*)GetCharacter())->SaveUserNameFromData();
	}
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

void AGAPlayerController::ClientTeamMessage_Implementation(APlayerState* SenderPlayerState, const FString& S, FName Type, float MsgLifeTime){
	Super::ClientTeamMessage_Implementation(SenderPlayerState, S, Type, MsgLifeTime);
	AGA_HUD* GameHUD = Cast<AGA_HUD>(GetHUD());
	if (GameHUD){
		if (SenderPlayerState != PlayerState){
			if (Type == "Message"){
				ChatLog.Insert(S, 0);
				GameHUD->UpdateChatLog();
			}
		}
		else{
			if (Type == "Username"){
				if (GetCharacter()){
					((AGACharacter*)GetCharacter())->SetLocalGAUsername(S);
				}
			}
		}
	}
}



void AGAPlayerController::OnActorChannelOpen(FInBunch& InBunch, UNetConnection* Connection){
	Super::OnActorChannelOpen(InBunch, Connection);
}

void AGAPlayerController::ClientRestart_Implementation(APawn* NewPawn)
{
	UE_LOG(LogClass, Log, TEXT("ClientRestart_Implementation %s"), *GetNameSafe(NewPawn));

	ResetIgnoreInputFlags();
	AcknowledgedPawn = NULL;

	SetPawn(NewPawn);
	if ((GetPawn() != NULL) && GetPawn()->bTearOff)
	{
		UnPossess();
		SetPawn(NULL);
		AcknowledgePossession(GetPawn());
		((AGACharacter*)GetCharacter())->LoadUserNameFromData();
		return;
	}

	if (GetPawn() == NULL)
	{
		return;
	}

	// Only acknowledge non-null Pawns here. ClientRestart is only ever called by the Server for valid pawns,
	// but we may receive the function call before Pawn is replicated over, so it will resolve to NULL.
	AcknowledgePossession(GetPawn());

	GetPawn()->Controller = this;
	GetPawn()->PawnClientRestart();

	if (Role < ROLE_Authority)
	{
		if (bAutoManageActiveCameraTarget)
		{
			SetViewTarget(GetPawn());
			ResetCameraMode();
		}

		ChangeState(NAME_Playing);
	}
}