// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GA_HUD.h"
#include "GACharacter.h"
#include "GAWeapon.h"
#include "GAGameMode.h"
#include "GAPlayerController.h"
#include "Net/UnrealNetwork.h"


AGAPlayerController::AGAPlayerController(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{

	static ConstructorHelpers::FObjectFinder<UBlueprint> HUD_Game(TEXT("/Game/UI/Classes/GA_HUD_BP"));
	static ConstructorHelpers::FObjectFinder<UBlueprint> HUD_Menu(TEXT("/Game/UI/Classes/GA_MainMenu.GA_MainMenu"));
	static ConstructorHelpers::FObjectFinder<UBlueprint> HUD_Transition(TEXT("/Game/UI/Classes/GA_TransitionHUD.GA_TransitionHUD"));

	MainMenuHud = (UClass*)HUD_Menu.Object->GeneratedClass;
	GameHud = (UClass*)HUD_Game.Object->GeneratedClass;
	TransitionHud = (UClass*)HUD_Transition.Object->GeneratedClass;

	ChatMessage = "";
	bReplicates = true;
	bAlwaysRelevant = true;
	PrimaryActorTick.bCanEverTick = true;
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
	/*for (int32 i = 0; i < ((AGA_HUD*)MyHUD)->currentSpawnedAreas.Num(); i++){
		ActorList.Add(((AGA_HUD*)MyHUD)->currentSpawnedAreas[i]);
	}*/
}
void AGAPlayerController::PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel){
	MyHUD->Destroy();
	//ClientSetHUD(TransitionHud);
}


#pragma region Textchat

void AGAPlayerController::SendChatMessage(const FString& Message){
	AddMessageToChatLog(Message);
	AGA_HUD* GAHUD = Cast<AGA_HUD>(GetHUD());
	if (GAHUD)
	{
		GAHUD->UpdateChatLog();
	}
}

void AGAPlayerController::AddMessageToChatLog(const FString& Message){
	if (Role < ROLE_Authority){
		ServerAddMessageToChatLog(Message);
		/*
		UE_LOG(LogClass, Log, TEXT("*** *** *** *** ***"), *GetName(), *ChatLog[0]);
		UE_LOG(LogClass, Log, TEXT("*** CLIENT CALL ***"), *GetName(), *ChatLog[0]);
		UE_LOG(LogClass, Log, TEXT("*** %s :: %s ***"), *GetName(), *ChatLog[0]);
		UE_LOG(LogClass, Log, TEXT("*** *** *** *** ***"), *GetName(), *ChatLog[0]);
		*/
	}
	else if (Role == ROLE_Authority){
		ChatMessage = Message;
		ChatLog.Insert(ChatMessage, 0);
		UE_LOG(LogClass, Log, TEXT("*** *** *** *** ***"), *GetName(), *ChatLog[0]);
		UE_LOG(LogClass, Log, TEXT("*** SERVER CALL ***"), *GetName(), *ChatLog[0]);
		UE_LOG(LogClass, Log, TEXT("*** %s :: %s ***"), *GetName(), *ChatLog[0]);
		UE_LOG(LogClass, Log, TEXT("*** *** *** *** ***"), *GetName(), *ChatLog[0]);
	}
}


void AGAPlayerController::OnRep_ChatMessage(){
	ChatLog.Insert(ChatMessage, 0);
	UE_LOG(LogClass, Log, TEXT("*** *** *** *** ***"), *GetName(), *ChatLog[0]);
	UE_LOG(LogClass, Log, TEXT("*** OnRepNotify ***"), *GetName(), *ChatLog[0]);
	UE_LOG(LogClass, Log, TEXT("*** %s :: %s ***"), *GetName(), *ChatLog[0]);
	UE_LOG(LogClass, Log, TEXT("*** *** *** *** ***"), *GetName(), *ChatLog[0]);
}

bool AGAPlayerController::ServerAddMessageToChatLog_Validate(const FString& Message){ return true; }
void AGAPlayerController::ServerAddMessageToChatLog_Implementation(const FString& Message){ AddMessageToChatLog(Message); }

#pragma endregion

// Replicates All Replicated Properties
void AGAPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Chat
	//DOREPLIFETIME_CONDITION(AGAPlayerController, ChatMessage, COND_SimulatedOrPhysics);
	DOREPLIFETIME(AGAPlayerController, ChatMessage);
}
