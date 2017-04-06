// (c) Ryan Johnson 2017

#include "cursor.h"

#include "helper.h"
#include "DDSTextureLoader.h"
#include "GameData.h"
#include <iostream>
#include "ContactListener.h"
#include "Time.h"

Cursor::Cursor(ID3D11Device* _GD) : ImageGO2D("cursor", _GD)
{
	m_joined = false;
	m_center = false;
	m_centerForce = 0.0f;
	m_GD = _GD;
}

Cursor::~Cursor()
{

}

void Cursor::init(b2World* world, const Vector2& _position, GameData* _GD)
{
	//Make the box body
	b2BodyDef bodyDef;
	bodyDef.type = b2_kinematicBody;
	bodyDef.position.Set(_position.x, _position.y);
	m_body = world->CreateBody(&bodyDef);

	//Set object type and reference back to the body
	ContactID* boxID = new ContactID;
	boxID->objectType = "cursor";
	boxID->object = this;
	m_body->SetUserData(boxID);

	m_pos = _position;

	//Set material for the cursor
	setTexture("cursor", _GD->m_device);


	float unit = (((16.0f) / 100.0f) / 2.0f); //Block size is 16, physics world is /10 scale, units are 1/2 units so /2

	b2PolygonShape boxShape;

	boxShape.SetAsBox(2.0f * unit, 2.0f * unit);

	m_centerForce = 0.0f;

	m_gameData = _GD;


}

void Cursor::Tick(GameData* _GD)
{
	//Update cursor position from haptic class
	double hapPos[3];
	_GD->m_haptics->getPosition(hapPos);
	m_pos.x = hapPos[0];
	m_pos.y = hapPos[1];
	m_body->SetTransform(b2Vec2((m_pos.x) / 100.0f, (m_pos.y * -1) / 100.0f), m_body->GetAngle());


	Button* hitButton;
	bool buttonbool = _GD->m_UIManager->pointCollides(m_pos, hitButton);

	if (m_joined)
	{
		//Apply cursor link forces to haptic device
		b2Vec2 forcesCursor = m_cursorJoint->GetReactionForce(_GD->m_dt);

		if (forcesCursor.x != 0.0f)
		{

			b2Vec2 differenceVector = m_cursorJoint->GetBodyA()->GetPosition() - m_cursorJoint->GetBodyB()->GetPosition();
			float dist = differenceVector.Length();

			float forceX = 0.0f;
			float forceY = 0.0f;

			//Don't apply force if spring is shorter then it's resting length
			if (dist >= m_cursorJoint->GetLength())
			{

				forceX = (forcesCursor.x / _GD->m_dt) * -500;
				forceY = (forcesCursor.y / _GD->m_dt) * -500;

			}

			//Apply forces to haptic device
			_GD->m_haptics->setForce(forceX, 0);
			_GD->m_haptics->setForce(forceY, 1);

		}

		_GD->m_haptics->setForce(hapPos[2] * m_centerForce * -1, 2);
	}
	else if (buttonbool)
	{
		//Cursor is over a button
		hitButton->setHaptics(hapPos);
	}
	else
	{
		//Cursor is not over a button or grabbing
		_GD->m_haptics->setForce(0.0f, 0);
		_GD->m_haptics->setForce(0.0f, 1);
		_GD->m_haptics->setForce(hapPos[2] * m_centerForce * -1, 2);
	}

	//Slowly increase the centering force to 100
	//This prevents very sudden movements of the handle when first launched
	if (m_centerForce < 200.0f && m_center)
	{
		m_centerForce += _GD->m_dt * 20.0f;
	}

}

void Cursor::setTexture(std::string _fileName, ID3D11Device* _GD)
{

	string fullfilename =
#if DEBUG
		"../Debug/"
#else
		"../Release/"
#endif
		+ _fileName + ".dds";
	HRESULT hr = CreateDDSTextureFromFile(_GD, Helper::charToWChar(fullfilename.c_str()), nullptr, &m_pTextureRV);

	//this nasty thing is required to find out the size of this image!
	ID3D11Resource *pResource;
	D3D11_TEXTURE2D_DESC Desc;
	m_pTextureRV->GetResource(&pResource);
	((ID3D11Texture2D *)pResource)->GetDesc(&Desc);

	m_origin = 0.5f*Vector2(Desc.Width, Desc.Height);//around which rotation and scaing is done

}

void Cursor::destroyJoint(b2World* _world)
{
	_world->DestroyJoint(m_cursorJoint);
	m_joined = false;
	
}

void Cursor::setCursor(CursorTool tool, bool grab) {

	//Update the curor texture based on the tool
	if (tool == CursorTool::PLACE)
	{
		switch (*m_gameData->m_boxSize)
		{
		case BOX_1X1:
			setTexture("1x1_fade", m_GD);
			break;
		case BOX_2X2:
			setTexture("2x2_fade", m_GD);
			break;
		case BOX_4X4:
			setTexture("4x4_fade", m_GD);
			break;
		default:
			setTexture("2x2_fade", m_GD);
			break;
		}
		
	}

	if (tool == CursorTool::LINK)
	{
		setTexture("cross", m_GD);
	}

	if (tool == CursorTool::GRAB && !grab)
	{
		setTexture("grabopen", m_GD);
	}

	if (tool == CursorTool::GRAB && grab)
	{
		setTexture("grab", m_GD);
	}
}

//Set whether the cursor is locked in the z-axis
void Cursor::centerCursor(bool _center)
{
	m_centerForce = 0.0f;
	m_center = _center;
}