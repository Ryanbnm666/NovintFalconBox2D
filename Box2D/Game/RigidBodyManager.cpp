// (c) Ryan Johnson 2017

#include "RigidBodyManager.h"
#include "GameData.h"
#include "Box.h"
#include <iostream>


RigidBodyManager::RigidBodyManager(b2World* _world , GameData* _GD)
{
	m_world = _world;
	m_buildSize = BOX_4X4;
	m_buildAngle = 0;

	_GD->m_boxSize = &m_buildSize;
	m_cursorTool = CursorTool::PLACE;

	m_jointStarted = false;

	m_boxes.reserve(2000);

	//Set cursor sprite
	_GD->m_cursor->setCursor(m_cursorTool, _GD->m_cursor->getJoined());
}

RigidBodyManager::~RigidBodyManager()
{
	//Draw all boxes
	for (auto b : m_boxes)
	{
		delete b;
	}
}


void RigidBodyManager::Tick(GameData* _GD)
{

	//Update spawn properties
	updateInput(_GD);

	//Get mouse and haptic press bools
	bool mousePressed	= _GD->m_mouseState->rgbButtons[0] && !(_GD->m_prevMouseState->rgbButtons[0]);


	//If haptic button OR mouse button are pressed
	if (mousePressed || _GD->m_hapticButtons.center)
	{

		//Calculate coords of cursor in the world
		float screenX = _GD->m_cursor->getPos().x;
		float screenY = _GD->m_cursor->getPos().y;

		float worldX = screenX / 100.0f;
		float worldY = (screenY * -1) / 100.0f;


		//Perform tool action
		switch (m_cursorTool)
		{
		case PLACE: //Place new box
			placeBox(_GD, Vector2(worldX, worldY));
			break;

		case LINK: //Link two boxes
			linkBoxes(_GD, Vector2(worldX, worldY));
			break;

		case GRAB: //Grab a box
			grabBox(_GD, Vector2(worldX, worldY));
			break;

		default:
			break;
		}		
		
	}

	//Update debug text
	updateDebug(_GD);

	//Tick all boxes
	for (auto b : m_boxes)
	{
		b->Tick(_GD);
	}

}

void RigidBodyManager::Draw(DrawData2D* _DD)
{
	//Draw all boxes
	for (auto b : m_boxes)
	{
		if (b->getAlive())
		{
			b->Draw(_DD);
		}
		
	}

}

//Update tool properties based on key or haptic press
void RigidBodyManager::updateInput(GameData* _GD)
{

	//Change tool when haptic up button pressed
	if (_GD->m_hapticButtons.up)
	{
		incTool(_GD);
	}

	//Change build size up when haptic right button pressed
	if (_GD->m_hapticButtons.right)
	{
		switch (m_buildSize)
		{
		case BOX_1X1:
			m_buildSize = BOX_2X2;
			break;
		case BOX_2X2:
			m_buildSize = BOX_4X4;
			break;
		default:
			break;
		}
		_GD->m_cursor->setCursor(m_cursorTool, _GD->m_cursor->getJoined());
	}

	//Change build size down when haptic left button pressed
	if (_GD->m_hapticButtons.left)
	{
		switch (m_buildSize)
		{
		case BOX_2X2:
			m_buildSize = BOX_1X1;
			break;
		case BOX_4X4:
			m_buildSize = BOX_2X2;
			break;
		default:
			break;
		}
		//Set cursor sprite
		_GD->m_cursor->setCursor(m_cursorTool, _GD->m_cursor->getJoined());
	}

	//Increment or decrement build angle
	if (_GD->m_keyboardState[DIK_NUMPADPLUS] && !(_GD->m_prevKeyboardState[DIK_NUMPADPLUS]))		{ m_buildAngle++; }
	if (_GD->m_keyboardState[DIK_NUMPADMINUS] && !(_GD->m_prevKeyboardState[DIK_NUMPADMINUS]))	{ m_buildAngle--; }

	//Reset to 0 if 360 or more
	if (m_buildAngle >= 360){ m_buildAngle = 0; }
}


//Add tool status to debug stream
void RigidBodyManager::updateDebug(GameData* _GD)
{
	switch (m_cursorTool)
	{
	case PLACE:
		_GD->m_debugText << "Tool: Place - Press the middle button to create a block\n";
		break;

	case GRAB:
		_GD->m_debugText << "Tool: Grab - Press the middle button to grab a block\n";
		break;

	case LINK:
		_GD->m_debugText << (m_jointStarted ?
			"Tool: Link - Press the middle button to select the SECOND object to join\n"
			: "Tool: Link - Press the middle button to select the first object to join\n");
		break;

	default:
		_GD->m_debugText << "Tool: None\n";
		break;

	}
}

//Place a new box at specified position
void RigidBodyManager::placeBox(GameData* _GD, Vector2 position)
{
	Box* newBox = new Box(_GD->m_device);
	newBox->init(m_world, position, Vector2(3.2f, 3.2f), m_buildSize, m_buildAngle, _GD);
	m_boxes.push_back(newBox);
}

void RigidBodyManager::linkBoxes(GameData* _GD, Vector2 position)
{
	if (!m_jointStarted)
	{
		Box* selectedBox = findBoxAtPosition(position);

		if (selectedBox)
		{
			m_jointBodyA = selectedBox->getBody();
			m_jointStarted = true;
		}
	}
	else
	{
		Box* selectedBox = findBoxAtPosition(position);

		if (selectedBox)
		{
			//New joint length is current distance in world
			float32 length = b2Distance(m_jointBodyA->GetPosition(), selectedBox->getBody()->GetPosition());

			//Create joint between boxes
			b2DistanceJoint* joint = createJoint(m_jointBodyA, selectedBox->getBody(), length, 4.0f);
			m_jointStarted = false;
		}
	}

}

void RigidBodyManager::grabBox(GameData* _GD, Vector2 position)
{
	//Ungrab a box if it exists, return if a joint was removed
	if (unGrabBox(_GD))
	{
		return;
	}

	Box* selectedBox = findBoxAtPosition(position);

	if (selectedBox)
	{
		float32 length = 0.3f;
		float32 frequency = 2.0f;

		//For large boxes, set higher spring length, loser spring frequency
		if (selectedBox->m_boxSize == BoxSize::BOX_4X4)
		{
			length = 0.6f;
			frequency = 1.0f;
		}

		b2DistanceJoint* joint = createJoint(_GD->m_cursor->getBody(), selectedBox->getBody(), length, frequency);
		_GD->m_cursor->setJoint(joint);
		_GD->m_cursor->setCursor(m_cursorTool, true);

	}


}

//Remove a cursor joint if it exists (returns true if a joint was removed)
bool RigidBodyManager::unGrabBox(GameData* _GD)
{
	//If a joint exists remove it
	if (_GD->m_cursor->getJoined())
	{
		_GD->m_cursor->setCursor(m_cursorTool, false);
		_GD->m_cursor->destroyJoint(m_world);
		return true;
	}
	return false;
}

void RigidBodyManager::setTool(CursorTool _tool, GameData* _GD)
{
	m_cursorTool = _tool;
	//Set cursor sprite
	_GD->m_cursor->setCursor(m_cursorTool, _GD->m_cursor->getJoined());
}

void RigidBodyManager::incTool(GameData* _GD)
{

	switch (m_cursorTool)
	{
	case PLACE:
		m_cursorTool = CursorTool::LINK;
		break;
	case GRAB:
		m_cursorTool = CursorTool::PLACE;
		break;
	case LINK:
		m_cursorTool = CursorTool::GRAB;
		break;
	default:
		m_cursorTool = CursorTool::PLACE;
		break;
	}

	//Set cursor sprite
	_GD->m_cursor->setCursor(m_cursorTool, _GD->m_cursor->getJoined());

	//If the tool is changed, cancel any started joints, ungrab the cursor
	m_jointStarted = false;
	unGrabBox(_GD);

}

void RigidBodyManager::setSize(BoxSize _size, GameData* _GD)
{
	m_buildSize = _size;

	//Set cursor sprite
	_GD->m_cursor->setCursor(m_cursorTool, _GD->m_cursor->getJoined());
}

//Find the first box at a specific world position
Box*  RigidBodyManager::findBoxAtPosition(Vector2 position)
{
	for (Box* box : m_boxes)
	{
		//Get fixture for box to be tested
		b2Fixture* boxFixture = box->getFixture();

		//Test if point collides with box fixture
		bool pointCollide = boxFixture->TestPoint(b2Vec2(position.x, position.y));

		if (pointCollide)
		{
			return box;
		}
	}

	return nullptr;
}

//Create a spring joint between two bodies, specifying length and frequency 
b2DistanceJoint* RigidBodyManager::createJoint(b2Body* bodyA, b2Body* bodyB, float32 length, float32 frequency)
{
	//Create a new joint
	b2DistanceJointDef jointDef;
	jointDef.bodyA = bodyA;
	jointDef.bodyB = bodyB;
	jointDef.length = length;
	jointDef.dampingRatio = 0.75f;
	jointDef.frequencyHz = frequency;
	jointDef.collideConnected = true;

	//Add joint to world
	b2DistanceJoint* joint = (b2DistanceJoint*)m_world->CreateJoint(&jointDef);

	//Make sure both bodies are awake
	bodyA->SetAwake(true);
	bodyB->SetAwake(true);

	return joint;
}