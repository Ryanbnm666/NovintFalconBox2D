// (c) Ryan Johnson 2017

#include "Water.h"
#include "helper.h"
#include "DDSTextureLoader.h"
#include "GameData.h"
#include <sstream>
#include <string>
#include <iostream>
#include <set>
#include "ContactListener.h"

Water::Water(ID3D11Device* _GD) : ImageGO2D("water", _GD)
{
	
}

Water::~Water()
{
	m_body->GetWorld()->DestroyBody(m_body);
}

void Water::init(b2World* world, Vector2 _position, GameData* _GD, b2Vec2 _worldGravity)
{
	//Make the box body
	b2BodyDef bodyDef;
	bodyDef.type = b2_kinematicBody;
	bodyDef.position.Set(_position.x, _position.y);
	m_body = world->CreateBody(&bodyDef);

	//Set object type and reference back to the body
	ContactID* boxID = new ContactID;
	boxID->objectType = "water";
	boxID->object = this;
	m_body->SetUserData(boxID);


	b2PolygonShape boxShape;
	boxShape.SetAsBox(10.0f, 10.0f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &boxShape;
	fixtureDef.density = 0.1f;
	fixtureDef.friction = 0.5f;
	fixtureDef.restitution = 0.01f;
	fixtureDef.isSensor = true;
	m_fixture = m_body->CreateFixture(&fixtureDef);

	//Update texture depending on size and material
	setTexture("water", _GD->m_device);

	m_gameData = _GD;
	m_worldGravity = _worldGravity;
	m_enabled = false;
}

void Water::Tick(GameData* _GD)
{
	if (m_enabled)
	{
		//Update position
		b2Vec2 boxPos = m_body->GetPosition();
		m_pos.x = boxPos.x * 100;
		m_pos.y = boxPos.y * -100;

		applyBuoyantForces(_GD);
	}
}

void Water::setTexture(std::string _fileName, ID3D11Device* _GD)
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

void Water::Draw(DrawData2D* _DD)
{
	if (m_enabled)
	{
		ImageGO2D::Draw(_DD);
	}
	
}


void Water::applyBuoyantForces(GameData* _GD)
{
	std::set<buoyPair>* buoyancyPairs = _GD->m_contactListener->getBuyoancyPairs();

	set<buoyPair>::iterator it = buoyancyPairs->begin();
	set<buoyPair>::iterator end = buoyancyPairs->end();

	//Loop through all objects to calculate buoyancy on
	while (it != end) {

		b2Fixture* fixtureA = it->first;	//The first fixture should always be the sensor (water)
		b2Fixture* fixtureB = it->second;

		b2Body* fixBBody = fixtureB->GetBody();

		vector<b2Vec2> intersectionPoints;	//Vector of points describing only the submerged section of the polygon

		float density = fixtureA->GetDensity();

		if (getIntersect(fixtureA, fixtureB, intersectionPoints)) {

			float area = 0;
			b2Vec2 centerPoint = getCenterPoint(intersectionPoints, area);


			//Calculate and apply buoyant force
			float displacedMass = fixtureA->GetDensity() * area;
			b2Vec2 buoyancyForce = displacedMass * -m_worldGravity;

			

			fixBBody->ApplyForce(buoyancyForce, centerPoint, true);


			//Calculate and apply drag inside the fluid
			b2Vec2 velocityVector = fixBBody->GetLinearVelocityFromWorldPoint(centerPoint);
			float velocity = velocityVector.Normalize();
			float drag = fixtureA->GetDensity() * velocity * velocity * _GD->m_dt;
			b2Vec2 dragForce = drag * -velocityVector;

			//Ignore high drag (fixes bug)
			if (drag <= 1.0f)
			{
				fixBBody->ApplyForce(dragForce, centerPoint, true);
			}

			//Calculate and apply angular drag
			float angularDrag = -fixBBody->GetAngularVelocity() * area * _GD->m_dt;

			fixBBody->ApplyTorque(angularDrag, true);
		}

		it++;
	}

}


//Test if a point is left (outside, false) or right (inside, true) of an edge
bool Water::testInside(b2Vec2 _edgePoint1, b2Vec2 _edgePoint2, b2Vec2 _testPoint) {
	//Dot product to test if point is left or right of edge
	float testX = (_edgePoint2.x - _edgePoint1.x)*(_testPoint.y - _edgePoint1.y);
	float testY = (_edgePoint2.y - _edgePoint1.y)*(_testPoint.x - _edgePoint1.x);
	
	return testX > testY;
}

//Implimentation of the Sutherland–Hodgman polygon clipping algorithm
bool Water::getIntersect(b2Fixture* _fixtureA, b2Fixture* _fixtureB, vector<b2Vec2>& _outPolygon)
{

		

	b2PolygonShape* polyA = (b2PolygonShape*)_fixtureA->GetShape();
	b2PolygonShape* polyB = (b2PolygonShape*)_fixtureB->GetShape();


	//Polygon to get clipped moved to outPolygon
	for (int i = 0; i < polyA->GetVertexCount(); i++)
	{
		_outPolygon.push_back(_fixtureA->GetBody()->GetWorldPoint(polyA->GetVertex(i)));
	}
		

	//fill clip polygon from fixtureB polygon
	vector<b2Vec2> clipPolygon;

	//Clipping window polygon stored
	for (int i = 0; i < polyB->GetVertexCount(); i++)
	{
		clipPolygon.push_back(_fixtureB->GetBody()->GetWorldPoint(polyB->GetVertex(i)));
	}
		

	b2Vec2 clipEdgeStart = clipPolygon[clipPolygon.size() - 1];

	//Loop through the edges of the clipping window polygon
	for (int j = 0; j < clipPolygon.size(); j++) {


		b2Vec2 clipEdgeEnd = clipPolygon[j];
		if (_outPolygon.empty())
		{
			return false;
		}
			
		vector<b2Vec2> inPolygon = _outPolygon;

		_outPolygon.clear();

		b2Vec2 startPoint = inPolygon[inPolygon.size() - 1]; //last on the input list

		//Loop clockwise through the points of the polygon
		for (int i = 0; i < inPolygon.size(); i++) {

			b2Vec2 endPoint = inPolygon[i];

			//Find the points to save
			if (!testInside(clipEdgeStart, clipEdgeEnd, startPoint) && !testInside(clipEdgeStart, clipEdgeEnd, endPoint)) {
				//Case A - both points are outside of the edge, save nothing
			}
			else if (!testInside(clipEdgeStart, clipEdgeEnd, startPoint) && testInside(clipEdgeStart, clipEdgeEnd, endPoint)) {
				//Case B - line starts outside, ends inside, save intersection and end
				_outPolygon.push_back(getIntersectPoints(clipEdgeStart, clipEdgeEnd, startPoint, endPoint));
				_outPolygon.push_back(endPoint);
			}
			else if (testInside(clipEdgeStart, clipEdgeEnd, startPoint) && testInside(clipEdgeStart, clipEdgeEnd, endPoint)) {
				//Case C - both points are inside of the edge, save just the end
				_outPolygon.push_back(endPoint);
			}
			else if (testInside(clipEdgeStart, clipEdgeEnd, startPoint) && !testInside(clipEdgeStart, clipEdgeEnd, endPoint)) {
				//Case D - line starts inside, ends outside, save intersetion only
				_outPolygon.push_back(getIntersectPoints(clipEdgeStart, clipEdgeEnd, startPoint, endPoint));
			}




			//The end point becomes the start for the next loop
			startPoint = endPoint;
		}

		//The end point for  this edge becomes the start for the next loop
		clipEdgeStart = clipEdgeEnd;
	}

	return !_outPolygon.empty();
}


b2Vec2 Water::getIntersectPoints(b2Vec2 clipEdgeStart, b2Vec2 clipEdgeEnd, b2Vec2 s, b2Vec2 e) {
	b2Vec2 dc(clipEdgeStart.x - clipEdgeEnd.x, clipEdgeStart.y - clipEdgeEnd.y);
	b2Vec2 dp(s.x - e.x, s.y - e.y);
	float n1 = clipEdgeStart.x * clipEdgeEnd.y - clipEdgeStart.y * clipEdgeEnd.x;
	float n2 = s.x * e.y - s.y * e.x;
	float n3 = 1.0 / (dc.x * dp.y - dc.y * dp.x);
	return b2Vec2((n1*dp.x - n2*dc.x) * n3, (n1*dp.y - n2*dc.y) * n3);
}


//Based on ComputeCentroid function in b2PolygonShape.cpp
//Calculates the center point and area for a polygon
b2Vec2 Water::getCenterPoint(std::vector<b2Vec2> _vertices, float& _area)
{
	
	b2Vec2 center = b2Vec2(0.0f, 0.0f);
	_area = 0.0f;
	b2Vec2 refPoint(0.0f, 0.0f);


	int count = _vertices.size();
	b2Assert(count >= 3);
	for (int i = 0; i < count; ++i)
	{
		// Triangle vertices.
		b2Vec2 point1 = refPoint;
		b2Vec2 point2 = _vertices[i];
		b2Vec2 point3 = i + 1 < count ? _vertices[i + 1] : _vertices[0];

		b2Vec2 edge1 = point2 - point1;
		b2Vec2 edge2 = point3 - point1;

		float triangleArea = b2Cross(edge1, edge2) * 0.5f;
		
		//Add triangle area to total area
		_area += triangleArea;

		// Area weighted centroid
		center += triangleArea * (1.0f / 3.0f) * (point1 + point2 + point3);
	}

	// Calculate center point (unless area is smaller than the engines smallest limit)
	if (_area > b2_epsilon)
	{
		center *= 1.0f / _area;
	}
	else
	{
		_area = 0;
	}
	return center;
}