// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GA_UI_Enums.h"
#include "GA_UI_Area.generated.h"


// A different namespace is required for each enum that should be Blueprint-editable
/********************* TEXT ALIGN ********************/

/**
* UI_Area horizontal text align
* Determines the horizontal alignment of a text
*/
UENUM(BlueprintType)
namespace GA_UI_Area_hTextAlign {
	enum GA_UI_Area_hTextAlign {
		TEXT_LEFT		UMETA(DisplayName = "Left"),
		TEXT_CENTER		UMETA(DisplayName = "Center"),
		TEXT_RIGHT		UMETA(DisplayName = "Right"),
	};
}

/**
* UI_Area vertical text align
* Determines the vertical alignment of a text

*/
UENUM(BlueprintType)
namespace GA_UI_Area_vTextAlign {
	enum GA_UI_Area_vTextAlign {
		TEXT_TOP		UMETA(DisplayName = "Top"),
		TEXT_MIDDLE		UMETA(DisplayName = "Middle"),
		TEXT_BOTTOM		UMETA(DisplayName = "Bottom"),
	};
}

UCLASS()
class AGA_UI_Area : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	bool initialized = false;

	GA_UI_Area_Category		category;


	bool					posInButton(FVector2D* pos);
	// only vaild answer, if .update() has been called on this object at this frame
	bool					mouseInButton;

	// prevent certain changes
	UPROPERTY(EditAnywhere, Transient, BlueprintReadWrite, Category = "Avalible events")
	bool					preventHover;
	UPROPERTY(EditAnywhere, Transient, BlueprintReadWrite, Category = "Avalible events")
	bool					preventActive;
	UPROPERTY(EditAnywhere, Transient, BlueprintReadWrite, Category = "Avalible events")
	bool					IsDraggable;
	UPROPERTY(EditAnywhere, Transient, BlueprintReadWrite, Category = "Avalible events")
	bool					IsDropZone;
	UPROPERTY(EditAnywhere, Transient, BlueprintReadWrite, Category = "Avalible events")
	bool					isTextArea;

	GA_UI_Area_type	type;

	// TODO 10: Rewrite this, so it not only supports IDs, but different Events (and is Blueprint compatible)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IDs")
	int32 activeOnMenuID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IDs")
	int32 switchToMenuID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fonts")
	UFont*					item_font;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fonts")
	UFont*					hover_font;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fonts")
	UFont*					active_font;

	UFont*					current_font;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Texts")
	FString					item_text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Texts")
	FString					hover_text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Texts")
	FString					active_text;

	FString*				current_text;
	// TODO 500: After that : Add positioning / scaling of the text

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnBeingDrawn();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vectors")
	FVector2D				item_position;
	FVector2D				fin_item_position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vectors")
	FVector2D				item_size;
	FVector2D				fin_item_size;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vectors")
	FVector2D				text_position;
	FVector2D				fin_text_position;

	// optional - will only be used, if it's not [0,0]
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vectors")
	FVector2D				text_size;
	FVector2D				fin_text_size;

	// used to allow relative positioning to a parent
	FVector2D				parent_padding;
	FVector2D				fin_parent_padding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vectors")
	TEnumAsByte<GA_UI_Area_hTextAlign::GA_UI_Area_hTextAlign>	text_horizontalAlignment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vectors")
	TEnumAsByte<GA_UI_Area_vTextAlign::GA_UI_Area_vTextAlign>	text_verticalAlignment;
	// TODO 250: Add tooltip (will be used for items as well)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vectors")
	float					letterSpacing;
	// TODO 250: Add tooltip (will be used for items as well)


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
	FLinearColor			item_textColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
	FLinearColor			hover_textColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
	FLinearColor			active_textColor;

	FLinearColor*			current_textColor;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Background - Color")
	FLinearColor			item_backgroundColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Background - Color")
	FLinearColor			hover_backgroundColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Background - Color")
	FLinearColor			active_backgroundColor;

	FLinearColor*			current_backgroundColor;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Background - Textures")
	UTexture2D*				item_backgroundImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Background - Textures")
	UTexture2D*				hover_backgroundImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Background - Textures")
	UTexture2D*				active_backgroundImage;

	UTexture2D*				current_backgroundImage;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Background - Material")
	UMaterialInterface*		item_backgroundMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Background - Material")
	UMaterialInterface*		hover_backgroundMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Background - Material")
	UMaterialInterface*		active_backgroundMaterial;

	UMaterialInterface*		current_backgroundMaterial;


	GA_UI_Area_buttonState	old_buttonState;
	GA_UI_Area_buttonState	buttonState;
	void setButtonState(GA_UI_Area_buttonState new_buttonState);

	void init(GA_UI_Area_Category _category, FVector2D* _clickMouseLocation, FVector2D* _prevMouseLocation, FVector2D* _mouseLocation, bool* _mouseHeld, bool* _prevMouseHeld, FVector2D _parent_padding);
	void draw(UCanvas* canvasToUse);
	bool update();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Areas")
	TArray<UClass*>			childAreas;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Areas")
	TArray<AGA_UI_Area*>	spawnedChildAreas;

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	virtual void OnClick();

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	virtual void OnStartDrag();
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	virtual void OnStoppedDrag();

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	virtual void OnReceivedDrop(AGA_UI_Area* droppedArea);

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	virtual void OnConfirmInput();

	void toggleChildren(bool state);
	bool					active = true;

private:
	bool					currentlyDragged;

	bool*					prevMouseHeld;
	bool*					mouseHeld;

	FVector2D*				originalSize;
	FVector2D*				currentScreenSize;

	FVector2D*				clickMouseLocation;
	FVector2D*				prevMouseLocation;
	FVector2D*				mouseLocation;
};
