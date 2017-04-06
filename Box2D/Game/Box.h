// (c) Ryan Johnson 2017
// Simple rigid body box


#ifndef _BOX_H_
#define _BOX_H_

#include <Box2D/Box2D.h>
#include "SimpleMath.h"
#include "ImageGO2D.h"

using namespace  DirectX::SimpleMath;

enum BoxSize
{
	BOX_1X1, BOX_2X2, BOX_4X4
};


class Box : public ImageGO2D
{
public:
	Box( ID3D11Device* _GD );
	~Box();

	virtual void Tick(GameData* _GD);

	void init(b2World* world, const Vector2& _position, const Vector2& dimensions, BoxSize _size, int _angle, GameData* _GD);

	b2Body* getBody() { return m_body; };
	b2Fixture* getFixture() { return m_fixture; };

	void setTexture(std::string _fileName, ID3D11Device* _GD);
	void setAngle(float _angle);
	void setAngle(int _angle);

	bool getAlive(){ return m_alive;  };

	BoxSize m_boxSize;

private:
	GameData* m_gameData;
	b2Body* m_body = nullptr;
	b2Fixture* m_fixture = nullptr;

	bool m_alive;
};

#endif