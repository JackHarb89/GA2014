// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GA_UI_Enums.h"
#include "GA_UI_Area.generated.h"

UCLASS()
class AGA_UI_Area : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	bool initialized;

	// only vaild answers, if .update() has been called on this object at this frame
	bool					mouseInButton;
	bool					hasBeenClicked;

	// prevent certain changes
	bool					preventHover = false;
	bool					preventActive = false;

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


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vectors")
	FVector2D				item_position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vectors")
	FVector2D				item_size;
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

	GA_UI_Area_buttonState old_buttonState;
	GA_UI_Area_buttonState buttonState;
	void setButtonState(GA_UI_Area_buttonState new_buttonState);

	void init(FVector2D* _mouseLocation, GA_UI_Area_mouseState* _mouseState);
	void draw(UCanvas* canvasToUse);
	void update();

private:
	GA_UI_Area_mouseState*	mouseState;
	FVector2D*				mouseLocation;
};
