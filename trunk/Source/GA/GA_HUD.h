// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GA.h"
//#include "Engine.h"
#include "GA_UI_Enums.h"
#include "GA_UI_Area.h"
#include "GameFramework/HUD.h"
#include "GA_HUD.generated.h"

////////// ////////// ////////// ////////// //////////

UENUM(BlueprintType)
namespace GA_UI_Dropphase {
	enum GA_UI_Dropphase {
		DROPPHASE_NONE,
		DROPPHASE_DROP_NEXT,
		DROPPHASE_SEARCH_AREAS
	};
}

UCLASS()
class AGA_HUD : public AHUD
{
	GENERATED_UCLASS_BODY()

private:
	/** prepare the sections used for the interface */
	void Setup_Sections();

public:

	// PlayerController bridge
	APlayerController* playerController;

	// General
	/** prepare all variables - ran before all graphical calls happen */
	void Setup();

	/** ran, after all game elements are created */
	virtual void PostInitializeComponents() OVERRIDE;

	virtual void DrawHUD() OVERRIDE;

	/** ran, after all graphical calls have happened */
	void PostRender();

	/** updates all input- and status-variables to fit the upcoming frame (mouseposition, currently shown menuID, etc.) */
	void UpdateValues();

	/** ran every possible frame */
	void Ticker();


	// Fonts
	// Font size should be > 48 - downscaling is easier

	/** Regular Font */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Fonts)
	UFont* Font_NormalRegular;

	/** Special Font */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Fonts)
	UFont* Font_SpecialRegular;


	// 2D Textures 
	/** Cursor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cursor)
	UTexture2D* Cursor_Normal;

	/** Hovering */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cursor)
	UTexture2D* Cursor_Hover;

	/** Active (mouse held down) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cursor)
	UTexture2D* Cursor_Active;

	/** Menu options? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Debug)
	int32 currentMenuID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Debug)
	int32 nextMenuID;

	// hack for TMap-garbage clean prevention
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Section)
	TArray<FString> enabledSectionNames;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Section)
	TArray<bool> enabledSectionStates;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Section)
	TArray<float> enabledSectionStartTime;			// Time used when fading the interface
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Section)
	float defaultFadeTime;

	UFUNCTION(BlueprintCallable, Category = Sections)
	float getSectionOpacity(FString name, float fadeDuration);

	UFUNCTION(BlueprintCallable, Category = Sections)
	int32 toggleSection(FString name, bool newValue);

	/**
	* Return state of interface-section
	*
	* Use "getSectionTime()" instead
	* 
	* @param Name of the section
	* @return -1 = section doesn't exist | 0 = section turned off | 1 section turned on
	*/
	UFUNCTION(BlueprintCallable, Category = Sections)
	int32 getSection(FString name);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Areas)
	TArray<UClass*> currentAreas;
	TArray<AGA_UI_Area*> currentSpawnedAreas;

	// --------------- FUNCTIONS ---------------
	// also includes not-shown variables only used in functions

	// Draw-functions
	// Scale

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = General)
	FVector2D originRes;
	FVector2D currentRes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = General)
	FVector2D currentScale;

	// Main				- manages everything (ingame-UI, menus, etc.)
	void Draw();

	// Drag&Drop		- Logic and parameters for drag- and drop-operations
	TEnumAsByte<GA_UI_Dropphase::GA_UI_Dropphase> dropPhase = 0;	// demetermines the current phase of a dragNdrop-action
	FVector2D dragDelta;
	FVector2D clickMouseLocation;
	AGA_UI_Area* dragArea;
	AGA_UI_Area* dropArea;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pointer)
	AGA_UI_Area* hoveredArea;

	void Draw_DragNDrop();

	// Cursor			- Draws the correct cursor (position and state)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pointer)
	FVector2D prevMouseLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pointer)
	FVector2D mouseLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pointer)
	bool prevMouseHeld;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pointer)
	bool mouseHeld;
	void Draw_Cursor();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	float GetMusicVolume();
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetMusicVolume(float volume);
	UFUNCTION(BlueprintCallable, Category = "Settings")
	float GetSfxVolume();
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetSfxVolume(float volume);

	// CanvasItems		- Everything, that can be specified in the Unreal Editor
	void Spawn_CanvasItems();
	void RunSpawnLogic(UClass* suppliedArea, GA_UI_Area_Category _category, FVector2D _parent_padding = { 0, 0 }, TArray<AGA_UI_Area*>* spawnInfoList = nullptr, int32 parent_zLayer = 0);
	void Draw_CanvasItems();
	void RunDrawLogic(AGA_UI_Area* suppliedArea);

	// Input			- Manages incoming keypresses and currently active input areas
	void ActivateTypingArea(AGA_UI_Area*);
	void EndCurrentInput(bool sendContent);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TextAreas)
	AGA_UI_Area* activeTypingArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TextAreas)
	FString blinkChar;									// char(s) that is/are being added, to indicate, that an area is selected

	void ParseKeyInput(const FString& newCharAsString);

	FString oldContent;
	FString currentContent;

	bool IsInit;
	UFUNCTION(BlueprintCallable, Category = "Chat") void UpdateChatLog();
};