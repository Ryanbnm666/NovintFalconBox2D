// (c) Ryan Johnson 2017

#include "UIManager.h"
#include "GameData.h"
#include <iostream>


UIManager::UIManager(GameData* _GD)
{
	m_GD = _GD;
	m_buttons.reserve(2000);

}

UIManager::~UIManager()
{
	//Draw all boxes
	for (auto b : m_buttons)
	{
		delete b;
	}
}


void UIManager::Tick(GameData* _GD)
{

	//Tick all boxes
	for (auto b : m_buttons)
	{
		b->Tick(_GD);
	}

}

void UIManager::Draw(DrawData2D* _DD)
{
	//Draw all boxes
	for (auto b : m_buttons)
	{
		b->Draw(_DD);
	}

}

void UIManager::addButton(Vector2 _pos, std::string _name)
{
	Button* newButton = new Button(m_GD->m_device);
	newButton->init(m_GD, _pos, Vector2(120.0f, 120.0f), _name);
	m_buttons.push_back(newButton);
}

bool UIManager::pointCollides(Vector2 _pos, Button* &_button)
{
	for (auto b : m_buttons)
	{
		if (b->pointCollides(_pos))
		{
			_button = b;
			return true;
		}
	}
	return false;
}