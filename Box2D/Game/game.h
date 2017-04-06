//DirectX Framework - 
//Modified by Ryan Johnson 2017

#ifndef _GAME_H_
#define _GAME_H_

//=================================================================
//Basic Game Manager Class
//=================================================================

#include <windows.h>
#include <list>
#include <vector>
#include <dinput.h>
#include "Effects.h"
#include "CommonStates.h"
#include "SpriteBatch.h"

#include "Keyboard.h"

#include "TextGO2D.h"
#include "SpriteFont.h"

//RJ
#include "haptics.h"

#include <Box2D/Box2D.h>
#include "ContactListener.h"

#include "Box.h"
#include "cursor.h"
//



using std::list;
using std::unique_ptr;
using namespace DirectX;

// Forward declarations
class GameObject;
class GameObject2D;
class Camera;
class TPSCamera;
struct GameData;
struct DrawData;
struct DrawData2D;
class Light;

class Game
{
public:

	Game(ID3D11Device* _pd3dDevice, HWND _hWnd, HINSTANCE _hInstance);
	~Game();

	bool Update(); //tick the game state

	void Render(ID3D11DeviceContext* _pd3dImmediateContext); //render the current game state

protected:
	DWORD m_playTime; //amount of time since the game started

	Camera* m_cam; //principle camera
	TPSCamera* m_TPScam;//TPS cam
	Light* m_light; //base light

	//required for the CMO model rendering system
	CommonStates* m_states;
	IEffectFactory* m_fxFactory;

	list<GameObject *> m_GameObjects; //data structure storing all GameObjects of this Game
	list<GameObject2D *> m_GameObject2Ds;//ditto 2D objects


	//direct input stuff
	bool ReadInput(); //Get current Mouse and Keyboard states
	IDirectInput8*			m_pDirectInput;
	IDirectInputDevice8*	m_pKeyboard;
	IDirectInputDevice8*	m_pMouse;
	unsigned char			m_keyboardState[256];
	unsigned char			m_prevKeyboardState[256];
	DIMOUSESTATE			m_mouseState;
	DIMOUSESTATE			m_prevMouseState;

	GameData* m_GD;			//Data to be shared to all Game Objects as they are ticked
	DrawData* m_DD;			//Data to be shared to all Game Objects as they are drawn
	DrawData2D * m_DD2D;	//Data to be passed by game to all 2D Game Objects via Draw 

	HWND m_hWnd;



	std::unique_ptr< b2World > m_world;			//Box2D Physics world
	ContactListener m_contactListener;			//Customised Box2D contact listener
	std::vector<ContactID* > m_contactIDs;		//List of all contacts in the world

	TextGO2D* m_debugText;		//Debug text in top left

	bool m_hapticButtonState;			//Haptic controller button state
	bool m_prevHapticButtonState;		//Haptic controller previous button state

	int m_hapticButtonsState;			//Haptic controller button state
	int m_prevHapticButtonsState;		//Haptic controller previous button state

	bool m_hapticNotHomed;			//Has the haptic device been homed
	bool m_prevHapticNotHomed;		//Was the haptic device homed last frame

	Cursor* m_cursor;

	ImageGO2D* m_bg;

	//Haptics
	HapticsClass m_Haptics;
	void initHaptics();


	//FPS Counter
	float m_fps_nextUpdate;
	int m_fps_frameCount;
	float m_fps;
	float m_fps_updateRate;

};


#endif
