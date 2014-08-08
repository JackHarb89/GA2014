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
	/* Replication Stuff                                                    */
	/************************************************************************/


	/************************************************************************/
	/* Broadcasting Stuff                                                   */
	/************************************************************************/

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Chat")
		TArray<FString> ChatLog;

	UFUNCTION(exec, Category = "Chat", BlueprintCallable)
		void SendChatMessage(const FString& Message);

	UFUNCTION(server, reliable, WithValidation)
		void ServerSendChatMessage(const FString& Message);
	

	virtual void ClientTeamMessage_Implementation(class APlayerState * SenderPlayerState, const FString & S, FName Type, float MsgLifeTime) OVERRIDE;

	/************************************************************************/
	/* Network Stuff                                                        */
	/************************************************************************/

	UFUNCTION(BlueprintCallable, Category = Matchmaking, exec) void ConnectToServer(const FString& ip);
	UFUNCTION(BlueprintCallable, Category = Matchmaking, exec) void ChangeMap(const FString& mapName);
	UFUNCTION(BlueprintCallable, Category = Matchmaking, exec) bool HostGameWithPort(int32 Port);
	UFUNCTION(BlueprintCallable, Category = Matchmaking, exec) void Disconnect();

	virtual void OnActorChannelOpen(class FInBunch & InBunch, class UNetConnection * Connection) OVERRIDE;

	virtual void PreClientTravel(const FString & PendingURL, ETravelType TravelType, bool bIsSeamlessTravel);

	virtual void AGAPlayerController::PlayerTick(float DeltaTime);
	virtual void GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList) OVERRIDE;

	virtual void ClientRestart_Implementation(class APawn * NewPawn);

};
