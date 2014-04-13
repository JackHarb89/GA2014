// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GA_HUD.h"


////////// ////////// GA_UI_Area ////////// //////////

void GA_UI_Area::init(FString _text, FString _tooltipText, FVector2D _position, FVector2D _size, FColor _textColor, FColor _hover_textColor, FColor _backgroundColor, FColor _hover_backgroundColor, UTexture2D* _backgroundImage, UTexture2D* _hover_backgroundImage) {
	if (initialized)
		return;

	text = _text;
	tooltipText = _tooltipText;

	position = _position;
	size = _size;

	textColor = _textColor;
	hover_textColor = _hover_textColor;

	backgroundColor = _backgroundColor;
	hover_backgroundColor = _hover_backgroundColor;

	backgroundImage = _backgroundImage;
	hover_backgroundImage = _hover_backgroundImage;

	initialized = true;
}

bool GA_UI_Area::isInArea(int32 mouseX, int32 mouseY) {
	return
		mouseX > position.X && mouseX < position.X + size.X
		&&
		mouseY > position.Y && mouseY < position.Y + size.Y;
}

bool GA_UI_Area::isInArea(FVector2D mouse) {
	return
		mouse.X > position.X && mouse.X < position.X + size.X
		&&
		mouse.Y > position.Y && mouse.Y < position.Y + size.Y;
}

bool GA_UI_Area::hasTooltip() {
	return tooltipText != NULL;
}
bool GA_UI_Area::hasImage() {
	return backgroundImage != NULL;
}

GA_UI_Area::GA_UI_Area(
	FString text, FString tooltipText,
	FVector2D position, FVector2D size,
	FColor textColor, FColor hover_textColor,
	FColor backgroundColor, FColor hover_backgroundColor
	) {
	init(text, tooltipText, position, size, textColor, hover_textColor, backgroundColor, hover_backgroundColor, NULL, NULL);
};

GA_UI_Area::GA_UI_Area(
	FString text, FString tooltipText,
	FVector2D position, FVector2D size,
	FColor textColor, FColor hover_textColor,
	UTexture2D* backgroundImage, UTexture2D* hover_backgroundImage
	) {
	init(text, tooltipText, position, size, textColor, hover_textColor, FColor(0, 0, 0, 0), FColor(0, 0, 0, 0), backgroundImage, hover_backgroundImage);
};

////////// ////////// ////////// ////////// //////////



////////// //////// Canvas Helper ///////// //////////

void AGA_HUD::DrawTexture(UTexture2D* texture, float x, float y, float w, float h, float xT, float yT, float wT, float hT, EBlendMode BlendMode) {
	Canvas->DrawTile(
		texture, x, y, 0,
		w, h,
		xT, yT,
		wT, hT,
		BlendMode
		);
}

void AGA_HUD::DrawTexture_FullSize(UTexture2D* texture, float x, float y) {
	DrawTexture(
		texture, x, y,
		texture->GetSurfaceWidth(), texture->GetSurfaceHeight(),
		0, 0,
		texture->GetSurfaceWidth(), texture->GetSurfaceHeight(),
		EBlendMode::BLEND_Translucent
		);
}

////////// ////////// ////////// ////////// //////////



AGA_HUD::AGA_HUD(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{
	HUDScale = 1.0f;
	FontScale = 1.0f;
	currentMenuID = 0;
	nextMenuID = 0;
	isIngame = false;
	openedMenu = false;
}

void AGA_HUD::PostRender() {
	UpdateValues();

	Draw();
}

void AGA_HUD::PostInitializeComponents() {
	Super::PostInitializeComponents();

	// get player controller
	playerController = GetOwningPlayerController();
}

void AGA_HUD::UpdateValues() {
	playerController->GetMousePosition(CurrentMouseLocation.X, CurrentMouseLocation.Y);
}

void AGA_HUD::Draw() {
	if (!playerController->PlayerInput) return;

	AGA_HUD::Draw_MainMenus();
}

void AGA_HUD::Init_Cursor() {
	if (initializedSections.Contains("cursor"))
		return;
	initializedSections.Add("cursor");
}

void AGA_HUD::Draw_Cursor() {
	if (!initializedSections.Contains("cursor"))
		Init_Cursor();

	Canvas->DrawTile(
		Cursor_Normal, CurrentMouseLocation.X, CurrentMouseLocation.Y, 0,
		Cursor_Normal->GetSurfaceWidth(),
		Cursor_Normal->GetSurfaceHeight(),
		0,
		0,
		Cursor_Normal->GetSurfaceWidth(),
		Cursor_Normal->GetSurfaceHeight(),
		BLEND_Translucent
		);
}

void AGA_HUD::Init_MainMenus() {
	if (initializedSections.Contains("mainmenus"))
		return;
	initializedSections.Add("mainmenus");


}

void AGA_HUD::Draw_MainMenus() {
	if (!initializedSections.Contains("mainmenus"))
		Init_MainMenus();

	AGA_HUD::Draw_Cursor();
}

void AGA_HUD::Init_IngameStats() {
	if (initializedSections.Contains("ingamestats"))
		return;
	initializedSections.Add("ingamestats");


}

void AGA_HUD::Draw_IngameStats() {
	if (!initializedSections.Contains("ingamestats"))
		Init_IngameStats();
}

void AGA_HUD::Init_IngameInterface() {
	if (initializedSections.Contains("ingameinterface"))
		return;
	initializedSections.Add("ingameinterface");


}

void AGA_HUD::Draw_IngameInterface() {
	if (!initializedSections.Contains("ingameinterface"))
		Init_IngameInterface();

	AGA_HUD::Draw_Cursor();
}

void AGA_HUD::Init_IngameMenus() {
	if (initializedSections.Contains("ingamemenus"))
		return;
	initializedSections.Add("ingamemenus");


}

void AGA_HUD::Draw_IngameMenus() {
	if (!initializedSections.Contains("ingamemenus"))
		Init_IngameMenus();

	AGA_HUD::Draw_Cursor();
}