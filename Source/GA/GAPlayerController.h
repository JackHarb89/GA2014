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
	
	virtual void AGAPlayerController::PlayerTick(float DeltaTime);

	// Client
	UFUNCTION(BlueprintCallable, Category = Matchmaking, exec) void ConnectToServer(const FString& ip);

	// Server
	UFUNCTION(BlueprintCallable, Category = Matchmaking, exec) void ChangeMap(const FString& mapName);

	UFUNCTION(BlueprintCallable, Category = Matchmaking, exec) void HostGameWithPort(int32 Port);

	virtual void GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList) OVERRIDE;
};
