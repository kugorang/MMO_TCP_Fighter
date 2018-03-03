#include "stdafx.h"
#include "0909_homework_MMOTCPFighterClient.h"
#include "BaseObject.h"

BaseObject::BaseObject() {}

BaseObject::~BaseObject() {}

void BaseObject::SetAnimation(int startSprite, int spriteNum, int frameDelayMax)
{
	nowSprite = startSprite;
	nowSpriteIndex = 0;
	nowFrameDelay = 0;
	this->spriteMaxIndex = spriteNum;
	this->frameDelayMax = frameDelayMax;
}

int BaseObject::GetYPos()
{
	return posY;
}

int BaseObject::GetUserID()
{
	return userID;
}

int BaseObject::GetObjectType()
{
	return objectType;
}