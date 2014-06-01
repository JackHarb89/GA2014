// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GAGameState.h"

AStrategyMiniMapCapture::AStrategyMiniMapCapture(const class FPostConstructInitializeProperties& PCIP) : Super(PCIP)
{
	CaptureComponent2D->bCaptureEveryFrame = false;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	MiniMapWidth = 256;
	MiniMapHeight = 256;
	RootComponent->SetWorldRotation(FRotator(-90, 0, 0));
	AudioListenerGroundLevel = 500.0f;
	bUseAudioListenerOrientation = false;
	bTextureChanged = true;
}

void AStrategyMiniMapCapture::BeginPlay()
{
	// @todo clean up
	Super::BeginPlay();

	if (!CaptureComponent2D->TextureTarget || MiniMapWidth != CaptureComponent2D->TextureTarget->GetSurfaceWidth()
		|| MiniMapHeight != CaptureComponent2D->TextureTarget->GetSurfaceHeight())
	{
		MiniMapView = NewObject<UTextureRenderTarget2D>();
		MiniMapView->InitAutoFormat(MiniMapWidth, MiniMapHeight);
		CaptureComponent2D->TextureTarget = MiniMapView;
		bTextureChanged = true;
	}

	FRotator OriginalRotation = RootComponent->GetComponentRotation();
	RootComponent->SetWorldRotation(FRotator(-90.f, 0, OriginalRotation.Roll));
	AGAGameState* const MyGameState = GetWorld()->GetGameState<AGAGameState>();
	if (MyGameState != NULL)
	{
		MyGameState->MiniMapCamera = this;
	}
	CachedFOV = CaptureComponent2D->FOVAngle;
	CachedLocation = RootComponent->GetComponentLocation();
	UpdateWorldBounds();
}

void AStrategyMiniMapCapture::UpdateWorldBounds()
{
	AGAGameState* const MyGameState = GetWorld()->GetGameState<AGAGameState>();
	if (MyGameState != NULL)
	{
		TArray<FVector> Points;
		FVector const CamLocation = RootComponent->GetComponentLocation();
		float DistanceFromGround = CamLocation.Z - GroundLevel;
		float Alpha = FMath::DegreesToRadians(CaptureComponent2D->FOVAngle / 2);
		float MaxVisibleDistance = (DistanceFromGround / FMath::Cos(Alpha)) * FMath::Sin(Alpha);

		Points.Add(FVector(CamLocation.X + MaxVisibleDistance, CamLocation.Y + MaxVisibleDistance, GroundLevel));
		Points.Add(FVector(CamLocation.X - MaxVisibleDistance, CamLocation.Y - MaxVisibleDistance, GroundLevel));

		MyGameState->WorldBounds = FBox(Points);
		CaptureComponent2D->UpdateContent();
	}
}

void AStrategyMiniMapCapture::Tick(float DeltaSeconds) {
	// Blueprint code outside of the construction script should not run in the editor
	// Allow tick if we are not a dedicated server, or we allow this tick on dedicated servers
	if (GetWorldSettings() != NULL && (bAllowReceiveTickEventOnDedicatedServer || !IsRunningDedicatedServer())) {
		ReceiveTick(DeltaSeconds);
	}

	if (CachedFOV != CaptureComponent2D->FOVAngle || CachedLocation != RootComponent->GetComponentLocation() || bTextureChanged)
	{
		bTextureChanged = false;
		CachedFOV = CaptureComponent2D->FOVAngle;
		CachedLocation = RootComponent->GetComponentLocation();
		UpdateWorldBounds();
	}
}

#if WITH_EDITOR


void AStrategyMiniMapCapture::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	UProperty* PropertyThatChanged = PropertyChangedEvent.Property;
	FName PropertyName = PropertyThatChanged != NULL ? PropertyThatChanged->GetFName() : NAME_None;

	if (PropertyName == FName(TEXT("RelativeRotation")))
	{
		FRotator ChangedRotation = RootComponent->GetComponentRotation();
		RootComponent->SetWorldRotation(FRotator(-90, 0, ChangedRotation.Roll));
	}
}

void AStrategyMiniMapCapture::EditorApplyRotation(const FRotator& DeltaRotation, bool bAltDown, bool bShiftDown, bool bCtrlDown)
{
	FRotator FiltredRotation(0, DeltaRotation.Yaw, 0);
	Super::EditorApplyRotation(FiltredRotation, bAltDown, bShiftDown, bCtrlDown);
}

#endif
