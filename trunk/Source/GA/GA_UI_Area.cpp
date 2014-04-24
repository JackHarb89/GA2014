// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GA.h"
#include "GA_UI_Area.h"

AGA_UI_Area::AGA_UI_Area(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGA_UI_Area::init(FVector2D* _mouseLocation, GA_UI_Area_mouseState* _mouseState) {
	if (initialized)
		return;

	mouseLocation = _mouseLocation;
	mouseState = _mouseState;

	hover_font = hover_font ? hover_font : item_font;
	active_font = active_font ? active_font : item_font;

	hover_text = hover_text != "" ? hover_text : item_text;
	active_text = active_text != "" ? active_text : item_text;

	hover_textColor = hover_textColor != FLinearColor(0, 0, 0, 0) ? hover_textColor : item_textColor;
	active_textColor = active_textColor != FLinearColor(0, 0, 0, 0) ? active_textColor : item_textColor;

	hover_backgroundColor = hover_backgroundColor != FLinearColor(0, 0, 0, 0) ? hover_backgroundColor : item_backgroundColor;
	active_backgroundColor = active_backgroundColor != FLinearColor(0, 0, 0, 0) ? active_backgroundColor : item_backgroundColor;

	hover_backgroundImage = hover_backgroundImage ? hover_backgroundImage : item_backgroundImage;
	active_backgroundImage = active_backgroundImage ? active_backgroundImage : item_backgroundImage;

	hover_backgroundMaterial = hover_backgroundMaterial ? hover_backgroundMaterial : item_backgroundMaterial;
	active_backgroundMaterial = active_backgroundMaterial ? active_backgroundMaterial : item_backgroundMaterial;


	if (item_backgroundMaterial)
		type = AREA_MATERIAL;
	else if (item_backgroundImage)
		type = AREA_IMAGE;
	else
		type = AREA_COLOR;

	current_font = item_font;
	current_text = &item_text;
	current_textColor = &item_textColor;

	current_backgroundColor = &item_backgroundColor;
	current_backgroundImage = item_backgroundImage;
	current_backgroundMaterial = item_backgroundMaterial;

	initialized = true;

	update();
}

void AGA_UI_Area::setButtonState(GA_UI_Area_buttonState new_buttonState) {
	if (old_buttonState == new_buttonState)
		return;

	old_buttonState = buttonState;
	buttonState = new_buttonState;
}

void AGA_UI_Area::update() {
	// refresh the mouseInButton-shortcut (shortens code)
	mouseInButton = (
		mouseLocation->X > item_position.X && mouseLocation->X < item_position.X + item_size.X
		&&
		mouseLocation->Y > item_position.Y && mouseLocation->Y < item_position.Y + item_size.Y
	);

	// Update buttonState
	switch (*mouseState) {
		case MOUSE_REGULAR:
			if (mouseInButton) {
				setButtonState(preventHover ? BUTTON_REGULAR : BUTTON_HOVER);
			}
			else {
				setButtonState(BUTTON_REGULAR);
			}
			break;
		case MOUSE_RELEASED:
			if (mouseInButton) {
				hasBeenClicked = true;
				// TODO: Add event handler (switch to new menu or run other function)
			}
			setButtonState(BUTTON_REGULAR);
			break;
		case MOUSE_CLICKED:
		case MOUSE_HELD:
			if (mouseInButton) {
				setButtonState(preventActive? BUTTON_REGULAR : BUTTON_ACTIVE);
			}
			else {
				setButtonState(BUTTON_REGULAR);
			}
			break;
	}

	// return if the button state hasn't changed
	if (buttonState == old_buttonState)
		return;

	// render text and change color only, if there is text set
	if (item_text != "") {
		switch (buttonState) {
			case BUTTON_REGULAR:
				current_text = &item_text;
				break;
			case BUTTON_HOVER:
				current_text = &hover_text;
				break;
			case BUTTON_ACTIVE:
				current_text = &active_text;
				break;
		}

		switch (buttonState) {
			case BUTTON_REGULAR:
				current_textColor = &item_textColor;
				break;
			case BUTTON_HOVER:
				current_textColor = &hover_textColor;
				break;
			case BUTTON_ACTIVE:
				current_textColor = &active_textColor;
				break;
		}
	}

	// render corresponding background
	switch (type) {
		case AREA_MATERIAL:
			switch (buttonState) {
				case BUTTON_REGULAR:
					current_backgroundMaterial = item_backgroundMaterial;
					break;
				case BUTTON_HOVER:
					current_backgroundMaterial = hover_backgroundMaterial;
					break;
				case BUTTON_ACTIVE:
					current_backgroundMaterial = active_backgroundMaterial;
					break;
			}
			break;
		case AREA_IMAGE:
			switch (buttonState) {
				case BUTTON_REGULAR:
					current_backgroundImage = item_backgroundImage;
					break;
				case BUTTON_HOVER:
					current_backgroundImage = hover_backgroundImage;
					break;
				case BUTTON_ACTIVE:
					current_backgroundImage = active_backgroundImage;
					break;
			}
			break;
		case AREA_COLOR:
			switch (buttonState) {
				case BUTTON_REGULAR:
					current_backgroundColor = &item_backgroundColor;
					break;
				case BUTTON_HOVER:
					current_backgroundColor = &hover_backgroundColor;
					break;
				case BUTTON_ACTIVE:
					current_backgroundColor = &active_backgroundColor;
					break;
			}
			break;
	}
}