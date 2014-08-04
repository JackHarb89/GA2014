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
	/* Broadcasting Stuff                                                   */
	/************************************************************************/

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Stats")
		FString	GAUserName;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Chat")
		TArray<FString> ChatLog;

	UFUNCTION(exec, Category = "Chat", BlueprintCallable)
		void SendChatMessage(const FString& Message);

	UFUNCTION(server, reliable, WithValidation)
		void ServerSendChatMessage(const FString& Message);


	UFUNCTION(exec, Category = "Chat", BlueprintCallable)
		void SetGAUsername(const FString& Username);

	UFUNCTION(server, reliable, WithValidation)
		void ServerSetGAUsername(const FString& Username);

	virtual void ClientTeamMessage_Implementation(class APlayerState * SenderPlayerState, const FString & S, FName Type, float MsgLifeTime) OVERRIDE;

	/************************************************************************/
	/* Network Stuff                                                        */
	/************************************************************************/

	UFUNCTION(BlueprintCallable, Category = Matchmaking, exec) void ConnectToServer(const FString& ip);
	UFUNCTION(BlueprintCallable, Category = Matchmaking, exec) void ChangeMap(const FString& mapName);
	UFUNCTION(BlueprintCallable, Category = Matchmaking, exec) bool HostGameWithPort(int32 Port);
	
	virtual void PreClientTravel(const FString & PendingURL, ETravelType TravelType, bool bIsSeamlessTravel);

	virtual void AGAPlayerController::PlayerTick(float DeltaTime);
	virtual void GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList) OVERRIDE;
};
