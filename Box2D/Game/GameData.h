//DirectX Framework - 
//Modified by Ryan Johnson 2017

#ifndef _GAME_DATA_H_
#define _GAME_DATA_H_

//=================================================================
//Data to be passed by game to all Game Objects via Tick
//=================================================================
#include <dinput.h>
#include <sstream>
#include "Keyboard.h"
#include "Mouse.h"
#include "GameState.h"
#include "haptics.h"
#include "cursor.h"
#include "UIManager.h"
#include "Water.h"
#include "ContactListener.h"

using namespace DirectX;


struct HapticButtons
{
	bool left;
	bool right;
	bool up;
	bool center;
};

struct GameData
{
	float m_dt;  //time step since last frame
	GameState m_GS; //global GameState

	std::ostringstream m_debugText; //Text drawn as debug text
	int m_nextObjectID;	//The ID of the next object created

	ID3D11Device* m_device; //Device

	//player input
	unsigned char* m_keyboardState;		//current state of the Keyboard
	unsigned char* m_prevKeyboardState;	//previous frame's state of the keyboard
	DIMOUSESTATE* m_mouseState;			//current state of the mouse
	DIMOUSESTATE* m_prevMouseState;		//previous state of the mouse

	bool* m_hapticButtonState;			//Haptic controller button state
	bool* m_prevHapticButtonState;		//Haptic controller previous button state

	int* m_hapticButtonsState;			//Haptic controller button state
	int* m_prevHapticButtonsState;		//Haptic controller previous button state

	HapticButtons m_hapticButtons;		//State for each button (only true frame after press)

	Cursor* m_cursor;					//Cursor object

	HapticsClass* m_haptics;			//Haptics class

	RigidBodyManager* m_bodyManager;	//Rigid Body Manager

	ContactListener* m_contactListener;

	UIManager* m_UIManager;

	BoxSize* m_boxSize;

	Water* m_water;

	HWND m_gameWindow;					//The game window
};

#endif
