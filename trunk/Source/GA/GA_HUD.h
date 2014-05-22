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
		DROPPHASE_DROP_NEXT ,
		DROPPHASE_SEARCH_AREAS
	};
}


UCLASS()
class AGA_HUD : public AHUD
{
	GENERATED_UCLASS_BODY()

	// PlayerController bridge
	APlayerController* playerController;

	// General
	/** ran, after all game elements are created */
	virtual void PostInitializeComponents() OVERRIDE;

	/** ran, after all graphical calls have happened */
	void PostRender();

	/** updates all input- and status-variables to fit the upcoming frame (mouseposition, currently shown menuID, etc.) */
	void UpdateValues();

	/** ran every possible frame */
	void Ticker();

	/** Total HUD scale */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = General)
	float HUDScale;


	// Fonts
	// Font size should be > 48 - downscaling is easier

	/** Regular Font */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Fonts)
	UFont* Font_NormalRegular;

	/** Special Font */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Fonts)
	UFont* Font_SpecialRegular;

	/** Font scale (relative to font file) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Fonts)
	float FontScale;


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

	/** Backgrounds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Backgrounds)
	UTexture2D* MainMenu_Tex;

	/** Background materials (used for advanced VFX) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Backgrounds_Materials)
	UMaterialInterface* MainMenu_Mat;

	/** Menu options? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Debug)
	int32 currentMenuID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Debug)
	int32 nextMenuID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Areas)
	TArray<UClass*> currentAreas;
	TArray<AGA_UI_Area*> currentSpawnedAreas;

	// --------------- FUNCTIONS ---------------
	// also includes not-shown variables only used in functions

	// Draw-functions
	// Scale

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resolution)
	FVector2D originRes;
	FVector2D currentRes;
	FVector2D currentScale;

	// Main				- manages everything (ingame-UI, menus, etc.)
	bool isIngame;
	bool openedMenu;	// being ingame and pressing ESC would toggle this

	void Draw();

	// Drag&Drop		- Logic and parameters for drag- and drop-operations
	TEnumAsByte<GA_UI_Dropphase::GA_UI_Dropphase> dropPhase = 0;	// demetermines the current phase of a dragNdrop-action
	FVector2D dragDelta;
	FVector2D clickMouseLocation;
	AGA_UI_Area* dragArea;
	AGA_UI_Area* dropArea;

	void Draw_DragNDrop();

	// Cursor			- Draws the correct cursor (position and state)
	FVector2D prevMouseLocation;
	FVector2D mouseLocation;
	bool prevMouseHeld = false;
	bool mouseHeld = false;
	void Draw_Cursor();

	// CanvasItems		- Everything, that can be specified in the Unreal Editor
	void Spawn_CanvasItems();
	void RunSpawnLogic(UClass* suppliedArea, GA_UI_Area_Category _category, FVector2D _parent_padding, TArray<AGA_UI_Area*>* spawnInfoList);
	void Draw_CanvasItems();
	void RunDrawLogic(AGA_UI_Area* suppliedArea);
};