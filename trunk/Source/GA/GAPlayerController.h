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
	UFUNCTION(exec) void ConnectToServer(const FString& ip);

	// Server
	UFUNCTION(exec) void HostGameWithMap(const FString& mapName);
	UFUNCTION(exec) void SetListenPort(int32 newPort);

private:

	int32 ListenPort;
};
