// (c) Ryan Johnson 2017

#include "button.h"

#include "helper.h"
#include "DDSTextureLoader.h"
#include "GameData.h"
#include <iostream>
#include "ContactListener.h"

Button::Button(ID3D11Device* _device) : ImageGO2D("button", _device)
{
	m_Device = _device;
	m_pressed = false;
	m_pressedPrev = false;
}

Button::~Button()
{

}

void Button::init(GameData* _GD, Vector2 _pos, Vector2 _size, std::string _name, HapticButtonType _type)
{
	m_name = _name;
	//Set material for the button
	setTexture(m_name, _GD->m_device);
	m_GD = _GD;

	m_size = _size;
	m_pos = _pos;
	m_type = _type;
}

void Button::Tick(GameData* _GD)
{
	//Trigger press function only on the frame it's first pressed
	if (m_pressed != m_pressedPrev)
	{
		if (m_pressed)
		{
			setTexture(m_name + "_pressed", m_Device);

			if (m_name == "btnGrab")
			{
				_GD->m_bodyManager->setTool(CursorTool::GRAB, _GD);
			}
			else if (m_name == "btnLink")
			{
				_GD->m_bodyManager->setTool(CursorTool::LINK, _GD);
			}
			else if (m_name == "btnPlace")
			{
				_GD->m_bodyManager->setTool(CursorTool::PLACE, _GD);
			}
			else if (m_name == "btnSmall")
			{
				_GD->m_bodyManager->setSize(BoxSize::BOX_1X1, _GD);
			}
			else if (m_name == "btnMedium")
			{
				_GD->m_bodyManager->setSize(BoxSize::BOX_2X2, _GD);
			}
			else if (m_name == "btnLarge")
			{
				_GD->m_bodyManager->setSize(BoxSize::BOX_4X4, _GD);
			}
			else if (m_name == "btnWater")
			{
				_GD->m_water->setEnabled(!_GD->m_water->getEnabled());
			}

		}
		else
		{
			setTexture(m_name, m_Device);
		}
	}
	

	m_pressedPrev = m_pressed;
}

void Button::setHaptics(double hapPos[3])
{

	switch (m_type)
	{
	case RUMBLING:
		m_GD->m_haptics->setForce(sin(GetTickCount() / 5.0f) * 2.0f, 2);
		break;
	case GUIDANCE:
		m_GD->m_haptics->setForce((hapPos[2] + 0.2) * -40.0f, 2);
		break;
	case DEFORMABLE:
	default:
		//Test if cursor pressed in to button
		if (hapPos[2] <= 0.0f)
		{
			//Increase spring pressure in range (actuation point)
			if (hapPos[2] < -0.1f && hapPos[2] > -0.12f)
			{
				m_GD->m_haptics->setForce((hapPos[2]) * -120.0f, 2);
			}
			else
			{
				m_GD->m_haptics->setForce((hapPos[2]) * -60.0f, 2);
			}

			//Push the cursor to the center of the button
			float centerForce = (hapPos[2] < -0.1f) ? -0.1f : hapPos[2];
			centerForce = centerForce;

			m_GD->m_haptics->setForce((hapPos[0] - m_pos.x) * centerForce, 0);
			m_GD->m_haptics->setForce((hapPos[1] - m_pos.y) * centerForce * -1, 1);

		}
		else
		{
			//If not pressing, apply normal restoration force
			m_GD->m_haptics->setForce((hapPos[2]) * -100.0f, 2);
		}
		break;
	}

	setPressed(hapPos[2] <= -0.12f);

}

//Check if a point collides witht this button
bool Button::pointCollides(Vector2 _pos)
{
	if (_pos.x >= (m_pos.x - (m_size.x / 2)))
	{
		if (_pos.x <= (m_pos.x + (m_size.x / 2)))
		{
			if (_pos.y >= (m_pos.y - (m_size.y / 2)))
			{
				if (_pos.y <= (m_pos.y + (m_size.y / 2)))
				{
					return true;
				}
			}
		}
	}

	return false;
}


void Button::setTexture(std::string _fileName, ID3D11Device* _GD)
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