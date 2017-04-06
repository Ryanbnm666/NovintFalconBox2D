// (c) Ryan Johnson 2017

#include "Box.h"
#include "helper.h"
#include "DDSTextureLoader.h"
#include "GameData.h"
#include <sstream>
#include <string>
#include <iostream>
#include "ContactListener.h"

Box::Box(ID3D11Device* _GD) : ImageGO2D("1x1", _GD)
{

}

Box::~Box()
{
	m_body->GetWorld()->DestroyBody(m_body);
}

void Box::init(b2World* world, const Vector2& _position, const Vector2& dimensions, BoxSize _size, int _angle, GameData* _GD)
{
	//Make the box body
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set( _position.x, _position.y );
	m_body = world->CreateBody(&bodyDef);

	//Set object type and reference back to the body
	ContactID* boxID = new ContactID;
	boxID->objectType = "box";
	boxID->object = this;
	m_body->SetUserData(boxID);


	float unit = (((15.5f) / 100.0f) / 2.0f); //Block size is 16, physics world is /100 scale, units are 1/2 units so /2
	ostringstream fileName;
	b2PolygonShape boxShape;

	//Find block size
	switch (_size)
	{
	case BOX_1X1:
		boxShape.SetAsBox(unit, unit);
		fileName << "1x1";
		break;

	case BOX_2X2:
		boxShape.SetAsBox(2.0f * unit, 2.0f * unit);
		fileName << "2x2";
		break;

	case BOX_4X4:
	default:
		boxShape.SetAsBox(4.0f * unit, 4.0f * unit);
		fileName << "4x4";
		break;
	}



	//Update texture depending on size and material
	setTexture(fileName.str(), _GD->m_device);
	
	//Set angle
	setAngle(_angle);
	

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &boxShape;
	fixtureDef.density = 0.043f;
	fixtureDef.friction = 0.5f;
	fixtureDef.restitution = 0.01f;
	m_fixture = m_body->CreateFixture(&fixtureDef);

	m_gameData = _GD;
	
	//Set Restitution
	m_boxSize = _size;

	m_alive = true;
	
}

void Box::Tick(GameData* _GD)
{
	b2Vec2 boxPos = m_body->GetPosition();

	m_pos.x = boxPos.x * 100;
	m_pos.y = boxPos.y * -100;

	m_rotation = m_body->GetAngle() * -1;

}

void Box::setTexture(std::string _fileName, ID3D11Device* _GD)
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

//Set angle with float radians
void Box::setAngle(float _angle)
{
	m_body->SetTransform(m_body->GetWorldCenter(), _angle);
	m_rotation = m_body->GetAngle() * -1;
}

//Set angle with int degrees
void Box::setAngle(int _angle)
{
	float radAngle = (XM_PI / 180) * _angle;
	m_body->SetTransform(m_body->GetWorldCenter(), radAngle);
	m_rotation = m_body->GetAngle() * -1;
}