#pragma once
/********************** UI STATES *********************/

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
* UI_Area button state
* Indicates the current state of a UI_Area
* _Not called an "area state", because the prefix of the values would conflict with ::GA_UI_Area_type
*/
enum GA_UI_Area_buttonState {
	BUTTON_REGULAR,
	BUTTON_HOVER,
	BUTTON_ACTIVE
};

enum GA_UI_Area_Category {
	UI_CAT_MAIN,
	UI_CAT_HOVER,
	UI_CAT_DRAGDROP
};