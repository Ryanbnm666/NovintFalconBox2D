//DirectX Framework - Simon Scarle

#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

//=================================================================
//Base Game Object Class
//=================================================================
#include "CommonStates.h"
#include "SimpleMath.h"
#include "Keyboard.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

// Forward declarations
class Camera;
struct ID3D11DeviceContext;
struct GameData;
struct DrawData;

class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

	virtual void Tick(GameData* _GD);
	virtual void Draw(DrawData* _DD) = 0;

	//getters
	Vector3		GetPos(){		return m_pos;		}
	Vector3		GetScale(){		return m_scale;		}

	float		GetPitch(){		return m_pitch;		}
	float		GetYaw(){		return m_yaw;		}
	float		GetRoll(){		return m_roll;		}

	//setters
	void		SetPos(		Vector3 _pos	){ m_pos	= _pos;		}

	void		SetScale(	float _scale	){ m_scale	= _scale * Vector3::One; }
	void		SetScale(float _x, float _y, float _z){ m_scale = Vector3(_x, _y, _z); }
	void		SetScale(Vector3 _scale){ m_scale = _scale; }

	void		SetPitch(	float _pitch	){ m_pitch	= _pitch;	}
	void		SetYaw(		float _yaw		){ m_yaw	= _yaw;		}
	void		SetRoll(	float _roll		){ m_roll	= _roll;	}
	void		SetPitchYawRoll(float _pitch, float _yaw, float _roll){ m_pitch = _pitch; m_yaw = _yaw; m_roll = _roll; }


protected:

	//World transform/matrix of this GO and it components
	Matrix m_worldMat;
	Matrix m_rotMat;
	Matrix m_fudge;
	Vector3 m_pos;
	float m_pitch, m_yaw, m_roll;
	Vector3 m_scale;

};

#endif