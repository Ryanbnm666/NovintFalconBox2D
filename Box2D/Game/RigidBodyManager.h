// (c) Ryan Johnson 2017

#ifndef _RIGID_BODY_MANAGER_H_
#define _RIGID_BODY_MANAGER_H_

#include <vector>
#include "SimpleMath.h"
#include "Box.h"

using namespace  DirectX::SimpleMath;

enum CursorTool
{
	PLACE, GRAB, LINK
};

class RigidBodyManager : public GameObject2D
{
public:
	RigidBodyManager(b2World* _world, GameData* _GD);
	~RigidBodyManager();

	virtual void Tick(GameData* _GD);
	virtual void Draw(DrawData2D* _DD);

	void updateInput(GameData* _GD);
	void updateDebug(GameData* _GD);

	void addDebugText(ostringstream* _stringStream);

	void placeBox(GameData* _GD, Vector2 position);
	void linkBoxes(GameData* _GD, Vector2 position);
	void grabBox(GameData* _GD, Vector2 position);
	bool unGrabBox(GameData* _GD);

	void setTool(CursorTool _tool, GameData* _GD);
	void incTool(GameData* _GD);
	void setSize(BoxSize _size, GameData* _GD);

	Box* findBoxAtPosition(Vector2 position);
	b2DistanceJoint* createJoint(b2Body* bodyA, b2Body* bodyB, float32 length, float32 frequency);
private:
	std::vector< Box* > m_boxes;
	b2World* m_world; //Box2D Physics world

	BoxSize m_buildSize;
	int m_buildAngle;

	CursorTool m_cursorTool;


	bool m_jointStarted;
	b2Body* m_jointBodyA;
};

#endif