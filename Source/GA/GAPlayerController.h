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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, ReplicatedUsing = OnRep_GAUserName, Category = "Player Stats")
		FString	GAUserName;

	void SetLocalGAUsername(const FString& Username);

	UFUNCTION(server, reliable, WithValidation)
		void ServerSetLocalGAUsername(const FString& Username);

	UFUNCTION()
		void OnRep_GAUserName();

	/************************************************************************/
	/* Broadcasting Stuff                                                   */
	/************************************************************************/


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
