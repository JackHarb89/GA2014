// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
//#include "Engine.h"
#include "GA_UI_Area.h"
#include "GameFramework/HUD.h"
#include "GA_HUD.h"

AGA_HUD::AGA_HUD(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{
	IsInit = false;
	currentMenuID = 0;
	nextMenuID = 0;

	blinkChar = FString("_");
	}

void AGA_HUD::UpdateValues() {
	prevMouseHeld = mouseHeld;
	mouseHeld = playerController->IsInputKeyDown(EKeys::LeftMouseButton);

	prevMouseLocation = mouseLocation;
	playerController->GetMousePosition(mouseLocation.X, mouseLocation.Y);

	if (!prevMouseHeld && mouseHeld)
		clickMouseLocation = mouseLocation;
}

//////////////////////////////////////////////////////////////////////////
//////////////////// TMap Interface-Hack /////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// returns the new value 0/1(false/true) or -1 on failure
int32 AGA_HUD::toggleSection(FString name, bool newValue) {
	int32 entryID;
	if (!enabledSectionNames.Contains(name)) {
		return -1;
	}
	else {
		entryID = enabledSectionNames.Find(name);
		
		if (entryID == -1)
			return -1;
	}

	enabledSectionStates[entryID] = newValue;

	return enabledSectionStates[entryID] ? 1 : 0;
}

/**
	
*/
int32 AGA_HUD::getSection(FString name) {
	int32 entryID;
	if (!enabledSectionNames.Contains(name)) {
		return -1;
	}
	else {
		entryID = enabledSectionNames.Find(name);
		if (entryID == -1)
			return -1;
		else
			return enabledSectionStates[entryID] ? 1 : 0;
	}
}
//////////////////////////////////////////////////////////////////////////
////////////////// TMap Interface-Hack END ///////////////////////////////
//////////////////////////////////////////////////////////////////////////


void AGA_HUD::PostInitializeComponents() {
	Super::PostInitializeComponents();
}

void AGA_HUD::PostRender() {
	if (!IsInit){
		enabledSectionNames.Add("inventory");
		enabledSectionStates.Add(false);

		enabledSectionNames.Add("sessioninfo");
		enabledSectionStates.Add(false);

		enabledSectionNames.Add("escapemenu");
		enabledSectionStates.Add(false);

		enabledSectionNames.Add("storycontainer");
		enabledSectionStates.Add(false);

		enabledSectionNames.Add("worldmessages");
		enabledSectionStates.Add(false);

		enabledSectionNames.Add("iteminfo");
		enabledSectionStates.Add(false);

		// get player controller
		playerController = GetOwningPlayerController();

		currentSpawnedAreas.Empty();
		Spawn_CanvasItems();

		IsInit = true;
	}

	if (!GetWorld()->IsInSeamlessTravel()){
		UpdateValues();

		Draw();
	}
}

void AGA_HUD::Spawn_CanvasItems() {
	// spawn all main-objects
	for (UClass* area : currentAreas) {
		RunSpawnLogic(area, UI_CAT_MAIN);
	}

	// spawn all children
	for (AGA_UI_Area* area : currentSpawnedAreas) {
		if (area != NULL) {
			for (int j = 0; j < area->childAreas.Num(); j++) {
				RunSpawnLogic(area->childAreas[j], UI_CAT_HOVER, area->item_position, &(area->spawnedChildAreas), (area->zLayer + 1));
			}
		}
		else {
			//UE_LOG(LogClass, Log, TEXT("*** Areaamount 2 INVALID!!!! ***"));
		}
	}

	// sort them (by their Z-Index)
	currentSpawnedAreas.Sort();
}

void AGA_HUD::Draw_CanvasItems() {
	if (dropPhase == GA_UI_Dropphase::DROPPHASE_DROP_NEXT) {
		dropPhase = GA_UI_Dropphase::DROPPHASE_SEARCH_AREAS;
		dropArea = nullptr;
	}
	
	for (AGA_UI_Area* area : currentSpawnedAreas) {
		if (!area->Inactive && (area->SectionName != "" && getSection(area->SectionName) > 0) && currentMenuID == area->activeOnMenuID) {
			if (mouseHeld && area->posInButton(&mouseLocation))
				ActivateTypingArea(area);

			if (mouseHeld && !area->posInButton(&mouseLocation) && area == activeTypingArea)
				EndCurrentInput(false);

			if ((dropPhase == GA_UI_Dropphase::DROPPHASE_SEARCH_AREAS) && area->IsDropZone && area->posInButton(&mouseLocation))
				dropArea = area;

			if (area->update()) {
				// save the area, if it's currently being dragged
				dragArea = area;
			}
			else if (area == dragArea && dropPhase != GA_UI_Dropphase::DROPPHASE_SEARCH_AREAS) {
				// and change the dropphase, if it's not being dragged anymore, but matches the current area
				dropPhase = GA_UI_Dropphase::DROPPHASE_DROP_NEXT;
			}
		}

		RunDrawLogic(area);
	}

	if (dropPhase == GA_UI_Dropphase::DROPPHASE_SEARCH_AREAS) {
		if (dropArea != nullptr) {
			dropArea->OnReceivedDrop(dragArea);
		}

		dropArea = nullptr;
		dragArea = nullptr;
		dropPhase = GA_UI_Dropphase::DROPPHASE_NONE;
	}

	if (nextMenuID != currentMenuID && nextMenuID != -1) {
		currentMenuID = nextMenuID;
	}
}

void AGA_HUD::Draw() {
	if (GEngine)
		GEngine->GameViewport->GetViewportSize(currentRes);

	currentScale = FVector2D(currentRes[0] / originRes[0], currentRes[1] / originRes[1]);

	// IMPORTANT: Some area-classes don't allow to scale X and Y seperately
	// Y ([1]) is being used in that case

	if (!playerController->PlayerInput) return;

	// Blueprint hook
	ReceiveDrawHUD(Canvas->SizeX, Canvas->SizeY);

	Draw_CanvasItems();

	Draw_DragNDrop();

	Draw_Cursor();
}

void AGA_HUD::Draw_DragNDrop() {
	if (dragArea == nullptr)
		return;
	
	switch (dragArea->type) {
		case AREA_MATERIAL:
			DrawMaterialSimple(
				dragArea->item_backgroundMaterial,
				mouseLocation.X, mouseLocation.Y,
				dragArea->item_size.X * currentScale[0], dragArea->item_size.Y * currentScale[1],
				1, false
			);
			break;
		case AREA_IMAGE:
			// removed ClipTile Argument *** dragArea->item_size.Y ***
			Canvas->DrawTile(
				dragArea->item_backgroundImage,
				mouseLocation.X, mouseLocation.Y,
				dragArea->item_size.X * currentScale[0], dragArea->item_size.Y * currentScale[1],
				0, //texture offset X
				0, //texture offset Y
				dragArea->item_size.X, // don't scale the texture (would cause tiling)
				dragArea->item_size.Y, // don't scale the texture (would cause tiling)
				BLEND_Translucent
			);
			break;
		case AREA_COLOR:
			FCanvasTileItem RectItem(
				mouseLocation,
				dragArea->item_size * currentScale,
				*dragArea->current_backgroundColor
			);

			RectItem.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(RectItem);
			break;
	}
}

void AGA_HUD::Draw_Cursor() {
	if (Cursor_Normal){
	// removed ClipTile Argument *** Cursor_Normal->GetSurfaceHeight() ***
	Canvas->DrawTile(
		Cursor_Normal, mouseLocation.X, mouseLocation.Y,
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

void AGA_HUD::RunSpawnLogic(UClass* suppliedArea, GA_UI_Area_Category _category, FVector2D _parent_padding, TArray<AGA_UI_Area*>* spawnInfoList, int32 parent_zLayer) {
	FActorSpawnParameters SpawnInfo;

	SpawnInfo.Owner = this;
	//"spawn" interface area
	AGA_UI_Area* area =
		GetWorld()->SpawnActor<AGA_UI_Area>(
		suppliedArea,
		FVector(0, 0, 0),
		FRotator(0, 0, 0),
		SpawnInfo
	);
	if (area == NULL) {
		UE_LOG(LogClass, Log, TEXT("*** Area couldn't spawn. ***"));
		return;
	}
	area->init(_category, &clickMouseLocation, &prevMouseLocation, &mouseLocation, &currentScale, &mouseHeld, &prevMouseHeld, _parent_padding, parent_zLayer);

	if (spawnInfoList != nullptr)
		spawnInfoList->Add((AGA_UI_Area*)area);

	currentSpawnedAreas.Add((AGA_UI_Area*)area);

	//GEngine->GameUserSettings->ApplySettings

	//ApplySettings
}

void AGA_HUD::RunDrawLogic(AGA_UI_Area* suppliedArea) {
	suppliedArea->setButtonState(BUTTON_REGULAR);
	if (suppliedArea->initialized) {
		if (suppliedArea->Inactive) {
			suppliedArea->toggleChildren(false);
			return;
		}

		if (suppliedArea->SectionName != "") {
			switch (getSection(suppliedArea->SectionName)) {
				case -1:
					UE_LOG(LogClass, Log, TEXT("*** Area has section-name '%s', that isn't listed in the 'enabledSectionNames'. ***"), *suppliedArea->SectionName);
					break;
				case 0:
					suppliedArea->toggleChildren(false);
					return;
					break;
				case 1:
					break;
			}
		}

		// only execute draw-commands, if the menuID is currently active
		if (currentMenuID != suppliedArea->activeOnMenuID) {
			suppliedArea->toggleChildren(false);
			return;
		}

		suppliedArea->toggleChildren(true);

		suppliedArea->runBlueprintEvents();

		suppliedArea->OnBeingDrawn();

		FVector2D finalPos =
			suppliedArea->dontUseParentPadding ?
			FVector2D(suppliedArea->item_position.X, suppliedArea->item_position.Y) :
			FVector2D(suppliedArea->parent_padding.X + suppliedArea->item_position.X, suppliedArea->parent_padding.Y + suppliedArea->item_position.Y);

		FVector2D finalScale = { suppliedArea->item_size.X, suppliedArea->item_size.Y };

		// draw the background
		switch (suppliedArea->type) {
		case AREA_MATERIAL:
			DrawMaterialSimple(
				suppliedArea->current_backgroundMaterial,
				finalPos[0] * currentScale[0], finalPos[1] * currentScale[1],
				finalScale[0], finalScale[1],
				1, false
				);
			break;
		case AREA_IMAGE:
			// removed ClipTile Argument *** finalScale[1] ***
			Canvas->DrawTile(
				suppliedArea->current_backgroundImage,
				finalPos[0] * currentScale[0], finalPos[1] * currentScale[1], // z-position
				finalScale[0] * currentScale[0], finalScale[1] * currentScale[1],
				0, //texture start width
				0, //texture start height
				finalScale[0],  // don't scale the texture (would cause tiling)
				finalScale[1],  // don't scale the texture (would cause tiling)
				BLEND_Translucent
				);
			break;
		case AREA_COLOR:
			FCanvasTileItem RectItem(
				finalPos * currentScale,
				finalScale * currentScale,
				*suppliedArea->current_backgroundColor
			);

			RectItem.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(RectItem);
			break;
		}

		// draw the text (on top)
		if (suppliedArea->isTextArea || *suppliedArea->current_text != "") {

			// calculate the size for alignment
			FVector2D realPositon = FVector2D::ZeroVector;

			// text item that will be drawn
			FCanvasTextItem TextItem(
				realPositon,
				FText::FromString(*suppliedArea->current_text),
				suppliedArea->current_font,
				*suppliedArea->current_textColor
			);

			TextItem.HorizSpacingAdjust = suppliedArea->letterSpacing;

			Canvas->TextSize(
				suppliedArea->current_font,
				*suppliedArea->current_text,
				realPositon[0],
				realPositon[1],
				currentScale[0],
				currentScale[1]
			);

			// realPositon now contains the rendered size

			switch (suppliedArea->text_horizontalAlignment) {
				case GA_UI_Area_hTextAlign::TEXT_LEFT:
					realPositon.X = (finalPos[0] + suppliedArea->text_position.X);
					break;
				case GA_UI_Area_hTextAlign::TEXT_CENTER:
					realPositon.X = (finalPos[0] + (suppliedArea->item_size.X / 2));
					break;
				case GA_UI_Area_hTextAlign::TEXT_RIGHT:
					realPositon.X = (finalPos[0] + suppliedArea->item_size.X - realPositon.X) - suppliedArea->text_position.X;
					break;
			}

			switch (suppliedArea->text_verticalAlignment) {
				case GA_UI_Area_vTextAlign::TEXT_TOP:
					realPositon.Y = (finalPos[1] + suppliedArea->text_position.Y);
					break;
				case GA_UI_Area_vTextAlign::TEXT_MIDDLE:
					realPositon.Y = (finalPos[1] + (suppliedArea->item_size.Y / 2));
					break;
				case GA_UI_Area_vTextAlign::TEXT_BOTTOM:
					realPositon.Y = ((finalPos[1] + suppliedArea->item_size.Y - realPositon.Y) - suppliedArea->text_position.Y);
					break;
			}

			// realPosition now contains the correct position

			// handle text-area and regular input
			FText textToDraw = FText::GetEmpty();
			if (!suppliedArea->isTextArea)
				textToDraw = FText::FromString(*suppliedArea->current_text);
			else if (suppliedArea == activeTypingArea)
				textToDraw = FText::FromString(
					suppliedArea->item_text
				+
					(
						((FDateTime::Now()).GetMillisecond() > 500) ? blinkChar : ""
					)
				);

			TextItem = FCanvasTextItem(
				realPositon * currentScale,
				textToDraw,
				suppliedArea->current_font,
				*suppliedArea->current_textColor
			);

			if (suppliedArea->text_horizontalAlignment == GA_UI_Area_hTextAlign::TEXT_CENTER)
				TextItem.bCentreX = true;

			if (suppliedArea->text_verticalAlignment == GA_UI_Area_vTextAlign::TEXT_MIDDLE)
				TextItem.bCentreY = true;

			TextItem.HorizSpacingAdjust = suppliedArea->letterSpacing;
			TextItem.Scale = currentScale;

			Canvas->DrawItem(TextItem);
		}
	}
	else {
		suppliedArea->init(UI_CAT_MAIN, &clickMouseLocation, &prevMouseLocation, &mouseLocation, &currentScale, &mouseHeld, &prevMouseHeld, { 0, 0 }, 0);
	}
}

void AGA_HUD::ActivateTypingArea(AGA_UI_Area* suppliedArea) {
	if (!suppliedArea->isTextArea)
		return;

	activeTypingArea = suppliedArea;
	oldContent = activeTypingArea->item_text;
	currentContent = activeTypingArea->item_text;
}

void AGA_HUD::EndCurrentInput(bool sendContent) {
	FString objName = activeTypingArea == nullptr ? "NULL" : activeTypingArea->GetName();

	UE_LOG(LogClass, Log, TEXT("*** %s current content. ('%s' in '%s') ***"),
		sendContent ? *FString("Accepting") : *FString("Throwing away"),
		*currentContent,
		*objName
	);

	activeTypingArea->item_text = sendContent ? currentContent : oldContent;

	if (sendContent)
		activeTypingArea->OnConfirmInput();

	oldContent = "";
	currentContent = "";
	activeTypingArea = nullptr;
}

void AGA_HUD::ParseKeyInput(const FString& newChar) {
	//UE_LOG(LogClass, Log, TEXT("*** Current key: %d %s [d] in '%s' ***"), newChar[0], *newChar, *((activeTypingArea == nullptr) ? "NULL" : activeTypingArea->GetName()));
	
	// jump to the chat, if we're ingame and enter is pressed
	if (activeTypingArea == nullptr) {
		// TODO: implement "is ingame" check here

		if (newChar[0] == 13)
			for (AGA_UI_Area* area : currentSpawnedAreas) {
				if (area->isTextArea) {
					activeTypingArea = area;
					break;
				}
			}
		return;
	}

	// Handle Escape and Enter
	if (newChar[0] == 13) {			// Enter
		EndCurrentInput(true);
		return;
	}
	else if (newChar[0] == 27) {	// Escape
		EndCurrentInput(false);
		return;
	}
	else if (newChar[0] == 8) {		// Backspace
		if (currentContent.Len() >= 1)
			currentContent.RemoveAt(currentContent.Len() - 1, 1);
	}
	else {
		currentContent += newChar;
	}

	activeTypingArea->item_text = currentContent;
}