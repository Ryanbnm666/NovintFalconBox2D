//DirectX Framework - 
//Modified by Ryan Johnson 2017

#include "game.h"
#include "GameData.h"
#include "drawdata.h"
#include "DrawData2D.h"
#include "gameobject.h"
#include "ObjectList.h"
#include "helper.h"
#include <windows.h>
#include <time.h>
#include "DDSTextureLoader.h"
#include <d3d11shader.h>
#include <iostream>
#include <iomanip>
#include <windows.h>
#include <d3d11_1.h>
#include <directxcolors.h>
#include "resource.h"


using namespace DirectX;




Game::Game(ID3D11Device* _pd3dDevice, HWND _hWnd, HINSTANCE _hInstance) :m_playTime(0), m_fxFactory(nullptr), m_states(nullptr)
{
	
	//Create DirectXTK spritebatch stuff
	ID3D11DeviceContext* pd3dImmediateContext;
	_pd3dDevice->GetImmediateContext(&pd3dImmediateContext);
	m_DD2D = new DrawData2D();
	m_DD2D->m_Sprites.reset(new SpriteBatch(pd3dImmediateContext));
	m_DD2D->m_Font.reset(new SpriteFont(_pd3dDevice, L"couriernew.spritefont"));

	//set up DirectXTK Effects system
	m_fxFactory = new EffectFactory(_pd3dDevice);

	//Tell the fxFactory to look to the correct build directory to pull stuff in from
#ifdef DEBUG
	((EffectFactory*)m_fxFactory)->SetDirectory(L"../Debug");
#else
	((EffectFactory*)m_fxFactory)->SetDirectory(L"../Release");
#endif

	//create DrawData struct and populate its pointers
	m_DD = new DrawData;
	m_DD->m_pd3dImmediateContext = nullptr;

	// Create other render resources here
	m_states = new CommonStates(_pd3dDevice);
	m_DD->m_states = m_states;

	//seed the random number generator
	srand((UINT)time(NULL));	


	//find how big my window is to correctly calculate my aspect ratio
	RECT rc;
	GetClientRect(m_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;
	float AR = (float)width / (float)height;

	//create a base camera
	m_cam = new Camera(0.25f * XM_PI, AR, 1.0f, 10000.0f, Vector3::UnitY, Vector3::Zero);
	m_cam->SetPos(Vector3(0.0f, 100.0f, 100.0f));
	m_GameObjects.push_back(m_cam);
	m_DD->m_cam = m_cam;

	//create a base light
	m_light = new Light(Vector3(0.0f, 100.0f, 160.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.4f, 0.1f, 0.1f, 1.0f));
	m_GameObjects.push_back(m_light);
	m_DD->m_light = m_light;

	

	//Direct Input Stuff
	m_hWnd = _hWnd;
	m_pKeyboard = nullptr;
	m_pDirectInput = nullptr;

	HRESULT hr = DirectInput8Create(_hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDirectInput, NULL);
	hr = m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, NULL);
	hr = m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = m_pKeyboard->SetCooperativeLevel(m_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	hr = m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouse, NULL);
	hr = m_pMouse->SetDataFormat(&c_dfDIMouse);
	hr = m_pMouse->SetCooperativeLevel(m_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);



	//create GameData struct and populate its pointers
	m_GD = new GameData;
	m_GD->m_keyboardState = m_keyboardState;
	m_GD->m_prevKeyboardState = m_prevKeyboardState;
	m_GD->m_GS = GS_PLAY_MAIN_CAM;
	m_GD->m_mouseState = &m_mouseState;
	m_GD->m_prevMouseState = &m_prevMouseState;
	m_GD->m_gameWindow = _hWnd;
	m_GD->m_device = _pd3dDevice;
	m_GD->m_nextObjectID = 0;
	m_GD->m_haptics = &m_Haptics;
	m_GD->m_hapticButtonState = &m_hapticButtonState;
	m_GD->m_prevHapticButtonState = &m_prevHapticButtonState;
	m_GD->m_hapticButtonsState = &m_hapticButtonsState;
	m_GD->m_prevHapticButtonsState = &m_prevHapticButtonsState;
	




	

	//Create world with gravity
	b2Vec2 gravity(0.0f, -9.81);
	m_world = std::make_unique< b2World >( gravity );
	m_world->SetContactListener(&m_contactListener);	//Specify custom conact listener
	m_GD->m_contactListener = &m_contactListener;

	//Add Ground Rigid Body
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, -11.58f);
	b2Body* groundBody = m_world->CreateBody( &groundBodyDef );
	b2PolygonShape groundBox;
	groundBox.SetAsBox(500.0f, 5.75f);
	groundBody->CreateFixture(&groundBox, 0.0f);
	
	//Set object type and reference back to the body
	ContactID* groundID = new ContactID;
	groundID->objectType = "ground";
	groundID->object = groundBody;
	groundBody->SetUserData(groundID);




	//Set up cursor
	m_cursor = new Cursor(_pd3dDevice);
	m_cursor->init(m_world.get(), Vector2(1.0f, 1.0f), m_GD);
	m_GD->m_cursor = m_cursor;


	RigidBodyManager* bodyManager = new RigidBodyManager(m_world.get(), m_GD);
	m_GameObject2Ds.push_back(bodyManager);
	m_GD->m_bodyManager = bodyManager;

	Water* water = new Water(_pd3dDevice);
	water->init(m_world.get(), Vector2(7.0f, -12.0f), m_GD, gravity);
	m_GD->m_water = water;
	m_GameObject2Ds.push_back(water);

	m_bg = new ImageGO2D("bg-blur", _pd3dDevice);
	m_bg->SetPos(Vector2(600.0f, 300.0f));


	UIManager* uiManager = new UIManager(m_GD);
	uiManager->addButton(Vector2(100.0f, 100.0f), "btnGrab");
	uiManager->addButton(Vector2(100.0f, 240.0f), "btnPlace");
	uiManager->addButton(Vector2(100.0f, 380.0f), "btnLink");
	uiManager->addButton(Vector2(100.0f, 520.0f), "btnWater");

	uiManager->addButton(Vector2(1100.0f, 100.0f), "btnSmall");
	uiManager->addButton(Vector2(1100.0f, 240.0f), "btnMedium");
	uiManager->addButton(Vector2(1100.0f, 380.0f), "btnLarge");
	m_GameObject2Ds.push_back(uiManager);
	m_GD->m_UIManager = uiManager;


	m_debugText = new TextGO2D("Debug");
	m_debugText->SetPos(Vector2(10, 10));
	m_debugText->SetColour(Color((float*)&Colors::Blue));
	m_debugText->SetScale(0.4f);
	m_GameObject2Ds.push_back(m_debugText);


	//Init FPS
	m_fps_nextUpdate = (float)GetTickCount() / 1000.0f;
	m_fps_frameCount = 0;
	m_fps = 0.0f;
	m_fps_updateRate = 0.25f;

	m_hapticNotHomed = true;
	m_prevHapticNotHomed = true;
	initHaptics();


}

Game::~Game()
{
	//delete Game Data & Draw Data
	delete m_GD;
	delete m_DD;


	//tidy away Direct Input Stuff
	if (m_pKeyboard)
	{
		m_pKeyboard->Unacquire();
		m_pKeyboard->Release();
	}
	if (m_pMouse)
	{
		m_pMouse->Unacquire();
		m_pMouse->Release();
	}
	if (m_pDirectInput)
	{
		m_pDirectInput->Release();
	}

	//get rid of the game objects here
	for (list<GameObject *>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		delete (*it);
	}
	m_GameObjects.clear();


	//and the 2D ones
	for (list<GameObject2D *>::iterator it = m_GameObject2Ds.begin(); it != m_GameObject2Ds.end(); it++)
	{
		delete (*it);
	}
	m_GameObject2Ds.clear();


	
	delete m_cursor;
	delete m_bg;

	//clear away CMO render system
	delete m_states;
	delete m_fxFactory;

	delete m_DD2D;
}

bool Game::Update()
{

	//Update FPS
	m_fps_frameCount++;
	if (((float)m_playTime) / 1000 > m_fps_nextUpdate)
	{
		m_fps_nextUpdate += m_fps_updateRate;
		m_fps = m_fps_frameCount / m_fps_updateRate;
		m_fps_frameCount = 0;
	}

	m_GD->m_debugText << "FPS: " << m_fps << "\n";

	if (!m_Haptics.isDeviceCalibrated())
	{
		m_GD->m_debugText << "HAPTIC DEVICE IS NOT CALIBRATED!\nPlease home the haptic device by pulling the handle all the way out\nand then pushing it all the way in";
	}

	//Start centering the controller once the haptic device has been homed
	m_hapticNotHomed = !m_Haptics.isDeviceCalibrated();
	if (!m_hapticNotHomed && m_prevHapticNotHomed)
	{
		m_GD->m_cursor->centerCursor();
	}
	m_prevHapticNotHomed = m_hapticNotHomed;
	

	//Poll Keyboard & Mouse & Haptic Controller
	ReadInput();

	//Upon pressing escape QUIT
	if (m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return false;
	}


	//Update debug text
	m_debugText->setText(m_GD->m_debugText.str());	//Display text
	m_GD->m_debugText.str("");						//Clear stream
	m_GD->m_debugText.clear();						//Clear error flags

	//calculate frame time-step dt for passing down to game objects
	DWORD currentTime = GetTickCount();
	m_GD->m_dt = min((float)(currentTime - m_playTime) / 1000.0f, 0.1f);
	m_playTime = currentTime;

	//update all objects

	for (list<GameObject2D *>::iterator it = m_GameObject2Ds.begin(); it != m_GameObject2Ds.end(); it++)
	{
		(*it)->Tick(m_GD);
	}


	//Tick the world
	m_world->Step(m_GD->m_dt, 6, 2);

	m_cursor->Tick(m_GD);


	return true;
}

void Game::Render(ID3D11DeviceContext* _pd3dImmediateContext)
{
	//set immediate context of the graphics device
	m_DD->m_pd3dImmediateContext = _pd3dImmediateContext;

	//set which camera to be used
	m_DD->m_cam = m_cam;

	// Draw sprite batch stuff 
	m_DD2D->m_Sprites->Begin();

	m_bg->Draw(m_DD2D);
	
	for (list<GameObject2D *>::iterator it = m_GameObject2Ds.begin(); it != m_GameObject2Ds.end(); it++)
	{
		(*it)->Draw(m_DD2D);
	}

	m_cursor->Draw(m_DD2D);

	
	m_DD2D->m_Sprites->End();

	//drawing text screws up the Depth Stencil State, this puts it back again!
	_pd3dImmediateContext->OMSetDepthStencilState(m_states->DepthDefault(), 0);
	
}

bool Game::ReadInput()
{	
	//copy over old keyboard state
	memcpy(m_prevKeyboardState, m_keyboardState, sizeof(unsigned char)*256 );
	memcpy(&m_prevMouseState, &m_mouseState, sizeof(DIMOUSESTATE));

	//clear out previous state
	ZeroMemory(&m_keyboardState, sizeof(unsigned char) * 256);
	ZeroMemory(&m_mouseState, sizeof(DIMOUSESTATE));

	// Read the keyboard device.
	HRESULT hr = m_pKeyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(hr))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			m_pKeyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	// Read the Mouse device.
	


	hr = m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(hr))
	{
		// If the Mouse lost focus or was not acquired then try to get control back.
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			m_pMouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	m_GD->m_haptics->synchFromServo();
	m_prevHapticButtonState = m_hapticButtonState;
	m_hapticButtonState = m_Haptics.isButtonDown();

	m_prevHapticButtonsState = m_hapticButtonsState;
	m_hapticButtonsState = m_Haptics.getButtons();


	//Get haptic button presses
	m_GD->m_hapticButtons.center = (*m_GD->m_hapticButtonsState & 1) && !(*m_GD->m_prevHapticButtonsState & 1);
	m_GD->m_hapticButtons.left = (*m_GD->m_hapticButtonsState & 2) && !(*m_GD->m_prevHapticButtonsState & 2);
	m_GD->m_hapticButtons.up = (*m_GD->m_hapticButtonsState & 4) && !(*m_GD->m_prevHapticButtonsState & 4);
	m_GD->m_hapticButtons.right = (*m_GD->m_hapticButtonsState & 8) && !(*m_GD->m_prevHapticButtonsState & 8);


	return true;
}

void Game::initHaptics()
{
	// Start Haptics
	m_Haptics.init(5.0f, 500.0f);

	// Some time is required between gHaptics.init() and checking status,
	// for the device to initialize and stabilize.  In a complex
	// application, this time can be consumed in the scene setup
	// function.  Here, it is simulated with Sleep().
	Sleep(100);

	m_hapticNotHomed = !m_Haptics.isDeviceCalibrated();

}




