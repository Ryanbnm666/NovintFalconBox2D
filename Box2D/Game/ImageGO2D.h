//DirectX Framework - Simon Scarle

#ifndef _IMAGE_GO_2D_H_
#define _IMAGE_GO_2D_H_
#include "GameObject2D.h"

class ImageGO2D :public GameObject2D
{
public:
	ImageGO2D(string _fileName, ID3D11Device* _GD);
	virtual ~ImageGO2D();

	virtual void Tick(GameData* _GD);
	virtual void Draw(DrawData2D* _DD);
protected:

	ID3D11ShaderResourceView* m_pTextureRV;
	Vector2 m_vel;
};

#endif