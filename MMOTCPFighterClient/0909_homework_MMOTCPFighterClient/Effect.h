#pragma once

#include "BaseObject.h"

class Effect : public BaseObject
{
private:
	int realPosX, realPosY;
	bool endFrame;
public:
	Effect(int userID, int posX, int posY);
	~Effect();

	void Action();
	void Draw(BYTE* dest, int destWidth, int destHeight, int destPitch);

	bool IsEndFrame();
};