#pragma once

#include "BaseObject.h"

#define ACTION_MOVE_LL 0
#define ACTION_MOVE_LU 1
#define ACTION_MOVE_UU 2
#define ACTION_MOVE_RU 3
#define ACTION_MOVE_RR 4
#define ACTION_MOVE_RD 5
#define ACTION_MOVE_DD 6
#define ACTION_MOVE_LD 7
#define ACTION_STAND 8
#define ACTION_ATTACK_01 9
#define ACTION_ATTACK_02 10
#define ACTION_ATTACK_03 11

#define ANIMATION_STAND_DELAY 5
#define ANIMATION_MOVE_DELAY 4
#define ANIMATION_ATTACK1_DELAY 3
#define ANIMATION_ATTACK2_DELAY 4
#define ANIMATION_ATTACK3_DELAY 4

#define CHARACTER_LR_MOVE_RANGE 3
#define CHARACTER_UD_MOVE_RANGE 2

struct PlayerStatus
{
	enum
	{
		STAND,
		MOVE,
		ATTACK
	};
};

class Player : public BaseObject
{
private:

	int hp, playerStatus, direction, animDirection, actionStatus, attackStatus;
	bool attackFlag, effectFlag;
public:
	Player(int userID, int direction, int posX, int posY, int hp);
	~Player();

	void Action();
	void MoveCharacter(int action);
	bool IsMoveRangeOver();
	void Draw(BYTE* dest, int destWidth, int destHeight, int destPitch);

	void SetActionStatus(int actionStatus);
	void SetHP(int hp);
	void SetEffectFlag(bool effectFlag);
	void SetPosition(WORD x, WORD y);
	void SetDirection(int direction);

	int GetXPos();
	int GetAnimDirection();
	int GetDirection();
};