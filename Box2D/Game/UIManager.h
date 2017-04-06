// (c) Ryan Johnson 2017

#ifndef _UI_MANAGER_H_
#define _UI_MANAGER_H_

#include <vector>
#include "SimpleMath.h"
#include "Button.h"

using namespace  DirectX::SimpleMath;


class UIManager : public GameObject2D
{
public:
	UIManager(GameData* _GD);
	~UIManager();

	virtual void Tick(GameData* _GD);
	virtual void Draw(DrawData2D* _DD);

	void addButton(Vector2 _pos, std::string _name);

	bool pointCollides(Vector2 _pos, Button* &_button );

private:
	std::vector<Button*> m_buttons;
	GameData* m_GD;
};

#endif