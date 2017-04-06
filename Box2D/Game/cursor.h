// (c) Ryan Johnson 2017

#ifndef _CURSOR_H_
#define _CURSOR_H_

#include "SimpleMath.h"
#include "ImageGO2D.h"
#include "Box2D.h"
#include "RigidBodyManager.h"
#include "Button.h"

using namespace  DirectX::SimpleMath;


class Cursor : public ImageGO2D
{
public:
	Cursor(ID3D11Device* _GD);
	~Cursor();

	virtual void Tick(GameData* _GD);

	void init(b2World* world, const Vector2& _position, GameData* _GD);

	b2Body* getBody() { return m_body; };
	//b2Fixture* getFixture() { return m_fixture; };

	void setTexture(std::string _fileName, ID3D11Device* _GD);

	void setJoint(b2DistanceJoint* joint) { m_cursorJoint = joint; m_joined = true; };
	b2DistanceJoint* getJoint() { return m_cursorJoint; };
	void destroyJoint(b2World* _world);

	bool getJoined() { return m_joined; };

	Vector2 getPos() { return m_pos;  };

	void setCursor(CursorTool tool, bool grab);

	void centerCursor( bool _center = true );

private:
	GameData* m_gameData;
	b2Body* m_body = nullptr;
	b2Fixture* m_fixture = nullptr;

	b2DistanceJoint* m_cursorJoint = nullptr;

	bool m_joined;

	ID3D11Device* m_GD;
	bool m_center;

	float m_centerForce;
};

#endif