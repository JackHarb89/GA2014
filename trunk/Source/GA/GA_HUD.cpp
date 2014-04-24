// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
//#include "Engine.h"
#include "GA_UI_Area.h"
#include "GameFramework/HUD.h"
#include "GA_HUD.h"

AGA_HUD::AGA_HUD(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{
	currentMenuID = 0;
	nextMenuID = 0;
	isIngame = false;
	openedMenu = false;
}

void AGA_HUD::UpdateValues() {
	bool mouseDown = playerController->IsInputKeyDown(EKeys::LeftMouseButton);

	switch (mouseState) {
	case MOUSE_REGULAR:
		if (mouseDown)
			mouseState = MOUSE_CLICKED;
		break;
	case MOUSE_HELD:
		if (!mouseDown)
			mouseState = MOUSE_RELEASED;
		break;
	case MOUSE_CLICKED:
		if (mouseDown)
			mouseState = MOUSE_HELD;
		else
			mouseState = MOUSE_RELEASED;
		break;
	case MOUSE_RELEASED:
		if (mouseDown)
			mouseState = MOUSE_CLICKED;
		else
			mouseState = MOUSE_REGULAR;
		break;
	}
	playerController->GetMousePosition(mouseLocation.X, mouseLocation.Y);
}

void AGA_HUD::PostInitializeComponents() {
	Super::PostInitializeComponents();

	// get player controller
	playerController = GetOwningPlayerController();
}

void AGA_HUD::PostRender() {
	UpdateValues();

	Draw();
}

void AGA_HUD::Draw() {
	if (!playerController->PlayerInput) return;

	AGA_HUD::Draw_CanvasItems();

	AGA_HUD::Draw_Cursor();
}

void AGA_HUD::Draw_Cursor() {
	if (Cursor_Normal){
	Canvas->DrawTile(
		Cursor_Normal, mouseLocation.X, mouseLocation.Y, 0,
		Cursor_Normal->GetSurfaceWidth(),
		Cursor_Normal->GetSurfaceHeight(),
		0,
		0,
		Cursor_Normal->GetSurfaceWidth(),
		Cursor_Normal->GetSurfaceHeight(),
		BLEND_Translucent
		);
	}
}

void AGA_HUD::Spawn_CanvasItems() {
	if (currentAreas.Num() == currentSpawnedAreas.Num())
		return;

	for (int i = 0; i < currentAreas.Num(); i++) {
		FActorSpawnParameters SpawnInfo;

		//SpawnInfo.bNoCollisionFail = true;
		SpawnInfo.Owner = this;
		//spawn creature
		AGA_UI_Area* area =
			GetWorld()->SpawnActor<AGA_UI_Area>(
			currentAreas[i],
			FVector(0, 0, 0),
			FRotator(0, 0, 0),
			SpawnInfo
			);

		currentSpawnedAreas.Add(area);
	}
}

void AGA_HUD::Draw_CanvasItems() {
	Spawn_CanvasItems();

	for (int i = 0; i < currentSpawnedAreas.Num(); i++) {
		if (currentSpawnedAreas[i]->initialized) {
			// refresh the values
			currentSpawnedAreas[i]->update();

			// only execute draw-commands, if the menuID is currently active
			if (currentMenuID != currentSpawnedAreas[i]->activeOnMenuID)
				continue;

			// draw the background
			switch (currentSpawnedAreas[i]->type) {
			case AREA_MATERIAL:
				DrawMaterialSimple(
					currentSpawnedAreas[i]->current_backgroundMaterial,
					currentSpawnedAreas[i]->item_position.X, currentSpawnedAreas[i]->item_position.Y,
					currentSpawnedAreas[i]->item_size.X, currentSpawnedAreas[i]->item_size.Y,
					1, false
					);
				break;
			case AREA_IMAGE:
				Canvas->DrawTile(
					currentSpawnedAreas[i]->current_backgroundImage,
					currentSpawnedAreas[i]->item_position.X, currentSpawnedAreas[i]->item_position.Y, 0, //z pos
					currentSpawnedAreas[i]->item_size.X, currentSpawnedAreas[i]->item_size.Y,
					0, //texture start width
					0, //texture start height
					currentSpawnedAreas[i]->item_size.X, currentSpawnedAreas[i]->item_size.Y,
					BLEND_Translucent
					);
				break;
			case AREA_COLOR:
				FCanvasTileItem RectItem(
					currentSpawnedAreas[i]->item_position,
					currentSpawnedAreas[i]->item_size,
					*currentSpawnedAreas[i]->current_backgroundColor
					);

				RectItem.BlendMode = SE_BLEND_Translucent;
				Canvas->DrawItem(RectItem);
				break;
			}

			// draw the text (on top)
			if (currentSpawnedAreas[i]->item_text != "") {
				DrawText(
					*currentSpawnedAreas[i]->current_text,
					*currentSpawnedAreas[i]->current_textColor,
					currentSpawnedAreas[i]->item_position.X, currentSpawnedAreas[i]->item_position.Y,
					currentSpawnedAreas[i]->current_font
					);
			}

			// check if the button has been pressed and do the corresponding action
			if (currentSpawnedAreas[i]->hasBeenClicked) {
				if (currentSpawnedAreas[i]->switchToMenuID != -1) {
					nextMenuID = currentSpawnedAreas[i]->switchToMenuID;
				}
				currentSpawnedAreas[i]->hasBeenClicked = false;
			}
		}
		else {
			currentSpawnedAreas[i]->init(&mouseLocation, &mouseState);
		}
	}

	if (nextMenuID != currentMenuID && nextMenuID != -1) {
		currentMenuID = nextMenuID;
	}

}