#include "stdafx.h"
#include "Map.h"
#include "Player.h"
#include "Effect.h"
#include "0909_homework_MMOTCPFighterClient.h"
#include "WSAAsyncSelect.h"

Player::Player(int userID, int direction, int posX, int posY, int hp) 
{
	this->userID = userID;
	this->direction = direction;
	this->posX = posX;
	this->posY = posY;
	this->hp = hp;

	actionStatus = ACTION_STAND;
	objectType = ObjectType::PLAYER;
	playerStatus = PlayerStatus::STAND;
	attackFlag = false;
	effectFlag = false;
	SetDirection(direction);
	
	if (animDirection == ACTION_MOVE_LL)
	{
		SetAnimation(PLAYER_STAND_L_01, 3, 8);
	}
	else
	{
		SetAnimation(PLAYER_STAND_R_01, 3, 8);
	}	
}

Player::~Player() {}

void Player::Action() 
{
	bool sendFlag = false;

	if (attackFlag && this == myPlayer)
	{
		return;
	}
	
	int scrollX = tileMap->GetScrollX();
	int scrollY = tileMap->GetScrollY();

	switch (actionStatus)
	{
	case ACTION_MOVE_LU:
		if (playerStatus != PlayerStatus::MOVE)
		{
			playerStatus = PlayerStatus::MOVE;
			SetAnimation(PLAYER_MOVE_L_01, 12, 4);

			sendFlag = true;
		}

		if (direction != ACTION_MOVE_LU)
		{
			if (animDirection != ACTION_MOVE_LL)
			{
				animDirection = ACTION_MOVE_LL;
				SetAnimation(PLAYER_MOVE_L_01, 12, 4);
			}

			direction = ACTION_MOVE_LU;
			sendFlag = true;
		}

		posX -= CHARACTER_LR_MOVE_RANGE;
		posY -= CHARACTER_UD_MOVE_RANGE;

		if (this == myPlayer)
		{
			tileMap->SetScrollX(scrollX - CHARACTER_LR_MOVE_RANGE);
			tileMap->SetScrollY(scrollY - CHARACTER_UD_MOVE_RANGE);
		}
		

		if (IsMoveRangeOver())
		{
			posX += CHARACTER_LR_MOVE_RANGE;
			posY += CHARACTER_UD_MOVE_RANGE;

			if (this == myPlayer)
			{
				tileMap->SetScrollX(scrollX);
				tileMap->SetScrollY(scrollY);
			}			
		}
		
		if (this == myPlayer && sendFlag)
		{
			SendMoveStart();
		}

		break;
	case ACTION_MOVE_LL:
		if (playerStatus != PlayerStatus::MOVE)
		{
			playerStatus = PlayerStatus::MOVE;
			SetAnimation(PLAYER_MOVE_L_01, 12, 4);

			sendFlag = true;
		}

		if (direction != ACTION_MOVE_LL)
		{
			if (animDirection != ACTION_MOVE_LL)
			{
				animDirection = ACTION_MOVE_LL;
				SetAnimation(PLAYER_MOVE_L_01, 12, 4);
			}

			direction = ACTION_MOVE_LL;
			sendFlag = true;
		}

		posX -= CHARACTER_LR_MOVE_RANGE;

		if (this == myPlayer)
		{
			tileMap->SetScrollX(scrollX - CHARACTER_LR_MOVE_RANGE);
		}		

		if (IsMoveRangeOver())
		{
			posX += CHARACTER_LR_MOVE_RANGE;

			if (this == myPlayer)
			{
				tileMap->SetScrollX(scrollX);
			}			
		}

		if (this == myPlayer && sendFlag)
		{
			SendMoveStart();
		}

		break;
	case ACTION_MOVE_LD:
		if (playerStatus != PlayerStatus::MOVE)
		{
			playerStatus = PlayerStatus::MOVE;
			SetAnimation(PLAYER_MOVE_L_01, 12, 4);

			sendFlag = true;
		}

		if (direction != ACTION_MOVE_LD)
		{
			if (animDirection != ACTION_MOVE_LL)
			{
				animDirection = ACTION_MOVE_LL;
				SetAnimation(PLAYER_MOVE_L_01, 12, 4);
			}

			direction = ACTION_MOVE_LD;			
			sendFlag = true;
		}

		posX -= CHARACTER_LR_MOVE_RANGE;
		posY += CHARACTER_UD_MOVE_RANGE;

		if (this == myPlayer)
		{
			tileMap->SetScrollX(scrollX - CHARACTER_LR_MOVE_RANGE);
			tileMap->SetScrollY(scrollY + CHARACTER_UD_MOVE_RANGE);
		}

		if (IsMoveRangeOver())
		{
			posX += CHARACTER_LR_MOVE_RANGE;
			posY -= CHARACTER_UD_MOVE_RANGE;

			if (this == myPlayer)
			{
				tileMap->SetScrollX(scrollX);
				tileMap->SetScrollY(scrollY);
			}
		}

		if (this == myPlayer && sendFlag)
		{
			SendMoveStart();
		}

		break;
	case ACTION_MOVE_UU:
		if (playerStatus != PlayerStatus::MOVE)
		{
			playerStatus = PlayerStatus::MOVE;

			if (animDirection == ACTION_MOVE_LL)
			{
				SetAnimation(PLAYER_MOVE_L_01, 12, 4);				
			}
			else
			{
				SetAnimation(PLAYER_MOVE_R_01, 12, 4);
			}

			sendFlag = true;
		}

		if (direction != ACTION_MOVE_UU)
		{
			direction = ACTION_MOVE_UU;
			sendFlag = true;
		}

		posY -= CHARACTER_UD_MOVE_RANGE;

		if (this == myPlayer)
		{
			tileMap->SetScrollY(scrollY - CHARACTER_UD_MOVE_RANGE);
		}		

		if (IsMoveRangeOver())
		{
			posY += CHARACTER_UD_MOVE_RANGE;

			if (this == myPlayer)
			{
				tileMap->SetScrollY(scrollY);
			}			
		}

		if (this == myPlayer && sendFlag)
		{
			SendMoveStart();
		}

		break;
	case ACTION_MOVE_RU:
		if (playerStatus != PlayerStatus::MOVE)
		{
			playerStatus = PlayerStatus::MOVE;
			SetAnimation(PLAYER_MOVE_R_01, 12, 4);

			sendFlag = true;
		}

		if (direction != ACTION_MOVE_RU)
		{
			if (animDirection != ACTION_MOVE_RR)
			{
				animDirection = ACTION_MOVE_RR;
				SetAnimation(PLAYER_MOVE_R_01, 12, 4);
			}

			direction = ACTION_MOVE_RU;
			sendFlag = true;
		}

		posX += CHARACTER_LR_MOVE_RANGE;
		posY -= CHARACTER_UD_MOVE_RANGE;

		if (this == myPlayer)
		{
			tileMap->SetScrollX(scrollX + CHARACTER_LR_MOVE_RANGE);
			tileMap->SetScrollY(scrollY - CHARACTER_UD_MOVE_RANGE);
		}

		if (IsMoveRangeOver())
		{
			posX -= CHARACTER_LR_MOVE_RANGE;
			posY += CHARACTER_UD_MOVE_RANGE;

			if (this == myPlayer)
			{
				tileMap->SetScrollX(scrollX);
				tileMap->SetScrollY(scrollY);
			}
		}

		if (this == myPlayer && sendFlag)
		{
			SendMoveStart();
		}

		break;
	case ACTION_MOVE_RR:
		if (playerStatus != PlayerStatus::MOVE)
		{
			playerStatus = PlayerStatus::MOVE;
			SetAnimation(PLAYER_MOVE_R_01, 12, 4);

			sendFlag = true;
		}

		if (direction != ACTION_MOVE_RR)
		{
			if (animDirection != ACTION_MOVE_RR)
			{
				animDirection = ACTION_MOVE_RR;
				SetAnimation(PLAYER_MOVE_R_01, 12, 4);
			}

			direction = ACTION_MOVE_RR;
			sendFlag = true;
		}

		posX += CHARACTER_LR_MOVE_RANGE;

		if (this == myPlayer)
		{
			tileMap->SetScrollX(scrollX + CHARACTER_LR_MOVE_RANGE);
		}		

		if (IsMoveRangeOver())
		{
			posX -= CHARACTER_LR_MOVE_RANGE;

			if (this == myPlayer)
			{
				tileMap->SetScrollX(scrollX);
			}			
		}

		if (this == myPlayer && sendFlag)
		{
			SendMoveStart();
		}

		break;
	case ACTION_MOVE_RD:
		if (playerStatus != PlayerStatus::MOVE)
		{
			playerStatus = PlayerStatus::MOVE;
			SetAnimation(PLAYER_MOVE_R_01, 12, 4);

			sendFlag = true;
		}

		if (direction != ACTION_MOVE_RD)
		{
			if (animDirection != ACTION_MOVE_RR)
			{
				animDirection = ACTION_MOVE_RR;
				SetAnimation(PLAYER_MOVE_R_01, 12, 4);
			}

			direction = ACTION_MOVE_RD;
			sendFlag = true;
		}

		posX += CHARACTER_LR_MOVE_RANGE;
		posY += CHARACTER_UD_MOVE_RANGE;

		if (this == myPlayer)
		{
			tileMap->SetScrollX(scrollX + CHARACTER_LR_MOVE_RANGE);
			tileMap->SetScrollY(scrollY + CHARACTER_UD_MOVE_RANGE);
		}		

		if (IsMoveRangeOver())
		{
			posX -= CHARACTER_LR_MOVE_RANGE;
			posY -= CHARACTER_UD_MOVE_RANGE;

			if (this == myPlayer)
			{
				tileMap->SetScrollX(scrollX);
				tileMap->SetScrollY(scrollY);
			}			
		}

		if (this == myPlayer && sendFlag)
		{
			SendMoveStart();
		}

		break;
	case ACTION_MOVE_DD:
		if (playerStatus != PlayerStatus::MOVE)
		{
			playerStatus = PlayerStatus::MOVE;

			if (animDirection == ACTION_MOVE_LL)
			{
				SetAnimation(PLAYER_MOVE_L_01, 12, 4);
			}
			else
			{
				SetAnimation(PLAYER_MOVE_R_01, 12, 4);
			}

			sendFlag = true;
		}

		if (direction != ACTION_MOVE_DD)
		{
			direction = ACTION_MOVE_DD;
			sendFlag = true;
		}

		posY += CHARACTER_UD_MOVE_RANGE;

		if (this == myPlayer)
		{
			tileMap->SetScrollY(scrollY + CHARACTER_UD_MOVE_RANGE);
		}		

		if (IsMoveRangeOver())
		{
			posY -= CHARACTER_UD_MOVE_RANGE;

			if (this == myPlayer)
			{
				tileMap->SetScrollY(scrollY);
			}			
		}

		if (this == myPlayer && sendFlag)
		{
			SendMoveStart();
		}

		break;
	case ACTION_ATTACK_01:
		if (playerStatus != PlayerStatus::ATTACK)
		{
			playerStatus = PlayerStatus::ATTACK;

			if (animDirection == ACTION_MOVE_LL)
			{
				SetAnimation(PLAYER_ATTACK1_L_01, 4, 3);
			}
			else
			{
				SetAnimation(PLAYER_ATTACK1_R_01, 4, 3);
			}

			attackFlag = true;		
			sendFlag = true;
		}

		if (attackStatus != 1)
		{
			attackStatus = 1;

			if (animDirection == ACTION_MOVE_LL)
			{
				SetAnimation(PLAYER_ATTACK1_L_01, 4, 3);
			}
			else
			{
				SetAnimation(PLAYER_ATTACK1_R_01, 4, 3);
			}

			attackFlag = true;
			sendFlag = true;
		}

		if (sendFlag && this == myPlayer)
		{
			SendAttack1();
		}

		break;
	case ACTION_ATTACK_02:
		if (playerStatus != PlayerStatus::ATTACK)
		{
			playerStatus = PlayerStatus::ATTACK;

			if (animDirection == ACTION_MOVE_LL)
			{
				SetAnimation(PLAYER_ATTACK2_L_01, 4, 4);
			}
			else
			{
				SetAnimation(PLAYER_ATTACK2_R_01, 4, 4);
			}

			attackFlag = true;
			sendFlag = true;
		}

		if (attackStatus != 2)
		{
			attackStatus = 2;

			if (animDirection == ACTION_MOVE_LL)
			{
				SetAnimation(PLAYER_ATTACK2_L_01, 4, 4);
			}
			else
			{
				SetAnimation(PLAYER_ATTACK2_R_01, 4, 4);
			}

			attackFlag = true;
			sendFlag = true;
		}

		if (sendFlag && this == myPlayer)
		{
			SendAttack2();
		}

		break;
	case ACTION_ATTACK_03:
		if (playerStatus != PlayerStatus::ATTACK)
		{
			playerStatus = PlayerStatus::ATTACK;

			if (animDirection == ACTION_MOVE_LL)
			{
				SetAnimation(PLAYER_ATTACK3_L_01, 6, 4);
			}
			else
			{
				SetAnimation(PLAYER_ATTACK3_R_01, 6, 4);
			}
			
			attackFlag = true;
			sendFlag = true;
		}

		if (attackStatus != 3)
		{
			attackStatus = 3;
			
			if (animDirection == ACTION_MOVE_LL)
			{
				SetAnimation(PLAYER_ATTACK3_L_01, 6, 4);
			}
			else
			{
				SetAnimation(PLAYER_ATTACK3_R_01, 6, 4);
			}

			attackFlag = true;
			sendFlag = true;
		}

		if (sendFlag && this == myPlayer)
		{
			SendAttack3();
		}
		
		break;
	case ACTION_STAND:
		if (playerStatus != PlayerStatus::STAND)
		{
			if ((playerStatus == PlayerStatus::MOVE || playerStatus == PlayerStatus::ATTACK)
				&& this == myPlayer)
			{
				SendMoveStop();
			}

			playerStatus = PlayerStatus::STAND;

			if (animDirection == ACTION_MOVE_LL)
			{
				SetAnimation(PLAYER_STAND_L_01, 3, 8);
			}
			else
			{
				SetAnimation(PLAYER_STAND_R_01, 3, 8);
			}			
		}
		break;
	}
}

void Player::MoveCharacter(int action)
{
	actionStatus = action;
}

bool Player::IsMoveRangeOver()
{
	if (posX < MAP_RANGE_LEFT || posX > MAP_RANGE_RIGHT
		|| posY < MAP_RANGE_TOP || posY > MAP_RANGE_BOTTOM)
	{
		return true;
	}
	else
	{
		return false;
	}	
}

void Player::Draw(BYTE* dest, int destWidth, int destHeight, int destPitch)
{
	int scrollX = tileMap->GetScrollX();
	int scrollY = tileMap->GetScrollY();

	spriteDib->DrawSpriteTranslucence(PLAYER_SHADOW, posX - scrollX, posY - scrollY, 
		dest, destWidth, destHeight, destPitch);
	
	if (this != myPlayer)
	{
		spriteDib->DrawSpriteRed(nowSprite + nowSpriteIndex, posX - scrollX, posY - scrollY,
			dest, destWidth, destHeight, destPitch);
	}
	else if (myPlayer != nullptr && this == myPlayer)
	{
		spriteDib->DrawSprite(nowSprite + nowSpriteIndex, posX - scrollX, posY - scrollY,
			dest, destWidth, destHeight, destPitch);
	}

	spriteDib->DrawSprite(PLAYER_HPGUAGE, posX - scrollX - 35, posY - scrollY + 9, 
		dest, destWidth, destHeight, destPitch, hp);

	switch (playerStatus)
	{
	case PlayerStatus::STAND:
	case PlayerStatus::MOVE:
		if (nowFrameDelay == frameDelayMax)
		{
			nowFrameDelay = 0;

			if (nowSpriteIndex == spriteMaxIndex - 1)
			{
				nowSpriteIndex = 0;
			}
			else
			{
				nowSpriteIndex++;
			}
		}
		break;
	case PlayerStatus::ATTACK:
		if (nowFrameDelay == frameDelayMax)
		{
			nowFrameDelay = 0;

			if (effectFlag)
			{
				if (attackStatus == 1 && nowSpriteIndex == 0)
				{
					if (animDirection == ACTION_MOVE_LL)
					{
						objectList.push_back(new Effect(userID, posX - 63, posY - 68));
					}
					else
					{
						objectList.push_back(new Effect(userID, posX + 63, posY - 68));
					}

					effectFlag = false;
				}
				else if (attackStatus == 2 && nowSpriteIndex == 1)
				{
					if (animDirection == ACTION_MOVE_LL)
					{
						objectList.push_back(new Effect(userID, posX - 60, posY - 64));
					}
					else
					{
						objectList.push_back(new Effect(userID, posX + 60, posY - 64));
					}

					effectFlag = false;
				}
				else if (attackStatus == 3 && nowSpriteIndex == 3)
				{
					if (animDirection == ACTION_MOVE_LL)
					{
						objectList.push_back(new Effect(userID, posX - 63, posY - 60));
					}
					else
					{
						objectList.push_back(new Effect(userID, posX + 63, posY - 60));
					}

					effectFlag = false;
				}
			}			

			if (nowSpriteIndex == spriteMaxIndex - 1)
			{
				nowSpriteIndex = 0;

				attackFlag = false;

				actionStatus = ACTION_STAND;
				attackStatus = 0;
			}
			else
			{
				nowSpriteIndex++;
			}
		}
		break;
	}
	
	nowFrameDelay++;
}

void Player::SetActionStatus(int actionStatus)
{
	this->actionStatus = actionStatus;
}

void Player::SetHP(int hp)
{
	this->hp = hp;
}

void Player::SetEffectFlag(bool effectFlag)
{
	this->effectFlag = effectFlag;
}

void Player::SetPosition(WORD x, WORD y)
{
	this->posX = x;
	this->posY = y;
}

void Player::SetDirection(int direction)
{
	this->direction = direction;

	switch (direction)
	{
	case ACTION_MOVE_LU:
	case ACTION_MOVE_LL:
	case ACTION_MOVE_LD:
		this->animDirection = ACTION_MOVE_LL;

		if (actionStatus == ACTION_STAND)
		{
			SetAnimation(PLAYER_STAND_L_01, 3, 8);
		}
		else
		{
			SetAnimation(PLAYER_MOVE_L_01, 12, 4);
		}
		break;
	case ACTION_MOVE_RU:
	case ACTION_MOVE_RR:
	case ACTION_MOVE_RD:
		this->animDirection = ACTION_MOVE_RR;

		if (actionStatus == ACTION_STAND)
		{
			SetAnimation(PLAYER_STAND_R_01, 3, 8);
		}
		else
		{
			SetAnimation(PLAYER_MOVE_R_01, 12, 4);
		}
		break;
	case ACTION_MOVE_UU:
	case ACTION_MOVE_DD:
		if (this->direction == ACTION_MOVE_LL)
		{
			if (actionStatus == ACTION_STAND)
			{
				SetAnimation(PLAYER_STAND_L_01, 3, 8);
			}
			else
			{
				SetAnimation(PLAYER_MOVE_L_01, 12, 4);
			}
		}
		else if (this->direction == ACTION_MOVE_RR)
		{
			if (actionStatus == ACTION_STAND)
			{
				SetAnimation(PLAYER_STAND_R_01, 3, 8);
			}
			else
			{
				SetAnimation(PLAYER_MOVE_R_01, 12, 4);
			}
		}
		break;
	}
}

int Player::GetXPos()
{
	return posX;
}

int Player::GetAnimDirection()
{
	return animDirection;
}

int Player::GetDirection()
{
	return direction;
}