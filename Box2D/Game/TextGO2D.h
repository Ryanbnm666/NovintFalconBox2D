//DirectX Framework - 

#ifndef _TEXT_GO_2D_H_
#define _TEXT_GO_2D_H_
#include "GameObject2D.h"

class TextGO2D :public GameObject2D
{
public:
	TextGO2D(string _text);

	virtual void Tick(GameData* _GD);
	virtual void Draw(DrawData2D* _DD);

	void setText(string _text){ m_text = _text; };
protected:
	string m_text;
};

#endif