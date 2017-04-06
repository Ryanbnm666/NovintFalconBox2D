// (c) Ryan Johnson 2017

#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "SimpleMath.h"
#include "ImageGO2D.h"

using namespace  DirectX::SimpleMath;

enum HapticButtonType
{
	RUMBLING, GUIDANCE, DEFORMABLE
};

class Button : public ImageGO2D
{
public:
	Button(ID3D11Device* _GD);
	~Button();

	virtual void Tick(GameData* _GD);

	void init(GameData* _GD, Vector2 _pos, Vector2 _size, std::string _name, HapticButtonType _type = HapticButtonType::DEFORMABLE);

	void setTexture(std::string _fileName, ID3D11Device* _GD);
	Vector2 m_size;

	Vector2 getPos() { return m_pos; }

	void setPressed(bool _pressed) { m_pressed = _pressed; }

	void setHaptics(double hapPos[3]);

	bool pointCollides(Vector2 _pos);

private:
	GameData* m_GD;
	ID3D11Device* m_Device;

	bool m_pressed;
	bool m_pressedPrev;

	std::string m_name;
	HapticButtonType m_type;

};

#endif