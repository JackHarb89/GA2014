#pragma once
/**
* UI_Area type
* Type, that declares, which background-type is being used for a GA_UI_Area
*/
enum GA_UI_Area_type {
	AREA_MATERIAL,
	AREA_IMAGE,
	AREA_COLOR
};

/**
* UI_Area mouse state
* Indicates the current status of a mouse button
*/
enum GA_UI_Area_mouseState {
	MOUSE_REGULAR,
	MOUSE_CLICKED,
	MOUSE_HELD,
	MOUSE_RELEASED
};

/**
* UI_Area button state
* Indicates the current state of a UI_Area
* _Not called an "area state", because the prefix of the values would conflict with ::GA_UI_Area_type
*/
enum GA_UI_Area_buttonState {
	BUTTON_REGULAR,
	BUTTON_HOVER,
	BUTTON_ACTIVE
};