#pragma once

enum SpriteEnum
{
	MAP,

	PLAYER_ATTACK1_L_01,
	PLAYER_ATTACK1_L_02,
	PLAYER_ATTACK1_L_03,
	PLAYER_ATTACK1_L_04,

	PLAYER_ATTACK1_R_01,
	PLAYER_ATTACK1_R_02,
	PLAYER_ATTACK1_R_03,
	PLAYER_ATTACK1_R_04,

	PLAYER_ATTACK2_L_01,
	PLAYER_ATTACK2_L_02,
	PLAYER_ATTACK2_L_03,
	PLAYER_ATTACK2_L_04,

	PLAYER_ATTACK2_R_01,
	PLAYER_ATTACK2_R_02,
	PLAYER_ATTACK2_R_03,
	PLAYER_ATTACK2_R_04,

	PLAYER_ATTACK3_L_01,
	PLAYER_ATTACK3_L_02,
	PLAYER_ATTACK3_L_03,
	PLAYER_ATTACK3_L_04,
	PLAYER_ATTACK3_L_05,
	PLAYER_ATTACK3_L_06,

	PLAYER_ATTACK3_R_01,
	PLAYER_ATTACK3_R_02,
	PLAYER_ATTACK3_R_03,
	PLAYER_ATTACK3_R_04,
	PLAYER_ATTACK3_R_05,
	PLAYER_ATTACK3_R_06,

	PLAYER_HPGUAGE,

	PLAYER_MOVE_L_01,
	PLAYER_MOVE_L_02,
	PLAYER_MOVE_L_03,
	PLAYER_MOVE_L_04,
	PLAYER_MOVE_L_05,
	PLAYER_MOVE_L_06,
	PLAYER_MOVE_L_07,
	PLAYER_MOVE_L_08,
	PLAYER_MOVE_L_09,
	PLAYER_MOVE_L_10,
	PLAYER_MOVE_L_11,
	PLAYER_MOVE_L_12,

	PLAYER_MOVE_R_01,
	PLAYER_MOVE_R_02,
	PLAYER_MOVE_R_03,
	PLAYER_MOVE_R_04,
	PLAYER_MOVE_R_05,
	PLAYER_MOVE_R_06,
	PLAYER_MOVE_R_07,
	PLAYER_MOVE_R_08,
	PLAYER_MOVE_R_09,
	PLAYER_MOVE_R_10,
	PLAYER_MOVE_R_11,
	PLAYER_MOVE_R_12,

	PLAYER_SHADOW,

	PLAYER_STAND_L_01,
	PLAYER_STAND_L_02,
	PLAYER_STAND_L_03,
	
	PLAYER_STAND_R_01,
	PLAYER_STAND_R_02,
	PLAYER_STAND_R_03,

	EFFECT_SPACK_01,
	EFFECT_SPACK_02,
	EFFECT_SPACK_03,
	EFFECT_SPACK_04,

	TILE
};

struct ObjectType
{
	enum
	{
		PLAYER,
		EFFECT
	};
};

class BaseObject
{
protected:
	int objectType, userID;
	int posX, posY;
	int nowSprite, nowSpriteIndex, spriteMaxIndex, nowFrameDelay, frameDelayMax;
public:
	BaseObject();
	~BaseObject();

	virtual void Action() = 0;
	virtual void Draw(BYTE* dest, int destWidth, int destHeight, int destPitch) = 0;

	void SetAnimation(int startSprite, int spriteNum, int frameDelayMax);
	
	int GetYPos();
	int GetUserID();
	int GetObjectType();
};