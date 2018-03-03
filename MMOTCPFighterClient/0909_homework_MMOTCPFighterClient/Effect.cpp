#include "stdafx.h"
#include "Player.h"
#include "Effect.h"
#include "0909_homework_MMOTCPFighterClient.h"
#include "Map.h"

Effect::Effect(int userID, int posX, int posY) 
{
	this->userID = userID;
	realPosX = posX;
	realPosY = posY;
	this->posX = MAP_RANGE_RIGHT + 1;
	this->posY = MAP_RANGE_BOTTOM + 1;

	objectType = ObjectType::EFFECT;
	endFrame = false;

	SetAnimation(EFFECT_SPACK_01, 4, 3);
}

Effect::~Effect() {}

void Effect::Action() {}

void Effect::Draw(BYTE* dest, int destWidth, int destHeight, int destPitch)
{
	int scrollX = tileMap->GetScrollX();
	int scrollY = tileMap->GetScrollY();
	
	if (myPlayer != nullptr && userID == myPlayer->GetUserID())
	{
		spriteDib->DrawSprite(nowSprite + nowSpriteIndex, realPosX - scrollX, realPosY - scrollY,
			dest, destWidth, destHeight, destPitch);
	}
	else
	{
		spriteDib->DrawSpriteRed(nowSprite + nowSpriteIndex, realPosX - scrollX, realPosY - scrollY,
			dest, destWidth, destHeight, destPitch);
	}
		
	if (nowFrameDelay == frameDelayMax)
	{
		nowFrameDelay = 0;

		if (nowSpriteIndex == spriteMaxIndex - 1)
		{
			endFrame = true;
		}
		else
		{
			nowSpriteIndex++;
		}
	}

	nowFrameDelay++;
}

bool Effect::IsEndFrame()
{
	return endFrame;
}