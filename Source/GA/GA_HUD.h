// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/HUD.h"
#include "GA_HUD.generated.h"

////////// ////////// ////////// ////////// //////////

class GA_UI_Area {
public:
	FString		text;
	FString		tooltipText;

	FVector2D	position;
	FVector2D	size;

	FColor		textColor;
	FColor		backgroundColor;
	UTexture2D*	backgroundImage;

	FColor		hover_textColor;
	FColor		hover_backgroundColor;
	UTexture2D*	hover_backgroundImage;

	GA_UI_Area(
		FString text, FString tooltipText,
		FVector2D position, FVector2D size,
		FColor textColor, FColor hover_textColor,
		FColor backgroundColor, FColor hover_backgroundColor
		);
	GA_UI_Area(
		FString text, FString tooltipText,
		FVector2D position, FVector2D size,
		FColor textColor, FColor hover_textColor,
		UTexture2D* backgroundImage, UTexture2D* hover_backgroundImage
		);

	void init(
		FString text, FString tooltipText,
		FVector2D position, FVector2D size,
		FColor textColor, FColor hover_textColor,
		FColor backgroundColor, FColor hover_backgroundColor,
		UTexture2D* backgroundImage, UTexture2D* hover_backgroundImage
		);

	bool initialized;

	bool isInArea(int32 mouseX, int32 mouseY);
	bool isInArea(FVector2D mouse);

	bool hasTooltip();
	bool hasImage();
};

////////// ////////// ////////// ////////// //////////

UCLASS()
class AGA_HUD : public AHUD
{
	GENERATED_UCLASS_BODY()

	// PlayerController bridge
	APlayerController* playerController;


	// DrawHelper
	/** Peronal shortcut for the canvas */
	void DrawTexture(UTexture2D* texture, float x, float y, float w, float h, float xT, float yT, float wT, float hT, EBlendMode BlendMode);
	/** helpful shortcut for elements which don't have to be scaled or cropped */
	void DrawTexture_FullSize(UTexture2D* texture, float x, float y);


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
		int8 currentMenuID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Debug)
		int8 nextMenuID;

	// --------------- FUNCTIONS ---------------
	// also includes not-shown variables only used in functions

	// Draw-functions
	// Main				- manages everything (ingame-UI, menus, etc.)
	bool isIngame;
	bool openedMenu;	// being ingame and pressing ESC would toggle this
	TArray<FString> initializedSections;
	TArray<GA_UI_Area> currentAreas;

	void Draw();

	// Cursor			- Draws the correct cursor (position and state)
	FVector2D CurrentMouseLocation;
	bool cursorAboveArea;
	bool mouseClicked;
	void Init_Cursor();
	void Draw_Cursor();

	// Menu				- WITH CURSOR - only called, when no game is running
	void Init_MainMenus();
	void Draw_MainMenus();

	// IngameStats		- only calls items, which don't require interaction (healthbars, cooldowns, map, etc.)
	float* hp;
	FVector2D* player2DPos;
	void Init_IngameStats();
	void Draw_IngameStats();

	// IngameInterface	- WITH CURSOR - only calls items, which allow interaction (shop, inventory)
	void Init_IngameInterface();
	void Draw_IngameInterface();

	// Menu				- WITH CURSOR - only called, when a game is paused
	void Init_IngameMenus();
	void Draw_IngameMenus();

};