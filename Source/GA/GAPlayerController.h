// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GAPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class AGAPlayerController : public APlayerController
{
	GENERATED_UCLASS_BODY()

	/************************************************************************/
	/* HUD                                                                  */
	/************************************************************************/


	UPROPERTY(EditAnywhere, noclear, BlueprintReadWrite, Category = "HUD Classes", meta = (DisplayName = "Main Menu HUD Class"))
		TSubclassOf<class AGA_HUD> MainMenuHud;
	UPROPERTY(EditAnywhere, noclear, BlueprintReadWrite, Category = "HUD Classes", meta = (DisplayName = "Transition HUD Class"))
		TSubclassOf<class AGA_HUD> TransitionHud;
	UPROPERTY(EditAnywhere, noclear, BlueprintReadWrite, Category = "HUD Classes", meta = (DisplayName = "Game HUD Class"))
		TSubclassOf<class AGA_HUD> GameHud;

	/************************************************************************/
	/* Replication Stuff                                                    */
	/************************************************************************/

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Player Stats")
		FString	UserName;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Chat")
		TArray<FString> ChatLog;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated, ReplicatedUsing = OnRep_ChatMessage, Category = "Chat")
		FString ChatMessage;

	UFUNCTION(exec, Category = "Chat", BlueprintCallable)
		void SendChatMessage(const FString& Message);

	UFUNCTION(Category = "Chat", BlueprintCallable)
		void AddMessageToChatLog(const FString& Message);

	// Server Function
	UFUNCTION(reliable, server, WithValidation)	
		void ServerAddMessageToChatLog(const FString& Message);

	// RepNotification
	UFUNCTION()	
		void OnRep_ChatMessage();

	/************************************************************************/
	/* Network Stuff                                                        */
	/************************************************************************/

	UFUNCTION(BlueprintCallable, Category = Matchmaking, exec) void ConnectToServer(const FString& ip);
	UFUNCTION(BlueprintCallable, Category = Matchmaking, exec) void ChangeMap(const FString& mapName);
	UFUNCTION(BlueprintCallable, Category = Matchmaking, exec) void HostGameWithPort(int32 Port);
	
	virtual void PreClientTravel(const FString & PendingURL, ETravelType TravelType, bool bIsSeamlessTravel);

	virtual void AGAPlayerController::PlayerTick(float DeltaTime);
	virtual void GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList) OVERRIDE;
};
