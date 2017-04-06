// (c) Ryan Johnson 2017

#ifndef _WATER_H_
#define _WATER_H_

#include <Box2D/Box2D.h>
#include "SimpleMath.h"
#include "ImageGO2D.h"
#include <vector>

using namespace  DirectX::SimpleMath;


class Water : public ImageGO2D
{
public:
	Water(ID3D11Device* _GD);
	~Water();

	virtual void Tick(GameData* _GD);

	void init(b2World* world, Vector2 _position, GameData* _GD, b2Vec2 _worldGravity);
	void Draw(DrawData2D* _DD);

	b2Body* getBody() { return m_body; };
	b2Fixture* getFixture() { return m_fixture; };

	void setTexture(std::string _fileName, ID3D11Device* _GD);

	//Set/get enabled status of the water ( when disabled the water is not rendered and no buoyant forces calculated)
	void setEnabled(bool _enabled = true) { m_enabled = _enabled; }
	bool getEnabled(){ return m_enabled; }

	//Applies buoyant forces to any bodies that intersect the water
	void applyBuoyantForces(GameData* _GD);

	b2Vec2 getIntersectPoints(b2Vec2 cp1, b2Vec2 cp2, b2Vec2 s, b2Vec2 e);
	b2Vec2 getCenterPoint(vector<b2Vec2> vs, float& area);

	bool testInside(b2Vec2 cp1, b2Vec2 cp2, b2Vec2 p);
	bool getIntersect(b2Fixture* fA, b2Fixture* fB, std::vector<b2Vec2>& outputVertices);
	
private:
	GameData* m_gameData;
	b2Body* m_body = nullptr;
	b2Fixture* m_fixture = nullptr;
	b2Vec2 m_worldGravity;
	bool m_enabled;

};

#endif