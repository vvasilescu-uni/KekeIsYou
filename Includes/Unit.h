//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//
// Desc: This file stores the player object class. This class performs tasks
//	   such as player movement, some minor physics as well as rendering.
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

#ifndef _CPLAYER_H_
#define _CPLAYER_H_

//-----------------------------------------------------------------------------
// CPlayer Specific Includes
//-----------------------------------------------------------------------------
#include "Main.h"
#include "Sprite.h"

#include <string>

//-----------------------------------------------------------------------------
// Main Class Definitions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CPlayer (Class)
// Desc : Player class handles all player manipulation, update and management.
//-----------------------------------------------------------------------------
class Unit
{
public:
	//-------------------------------------------------------------------------
	// Enumerators
	//-------------------------------------------------------------------------
	enum DIRECTION 
	{ 
		DIR_STOP		= 0,
		DIR_FORWARD		= 1,
		DIR_BACKWARD	= 2,
		DIR_LEFT		= 4,
		DIR_RIGHT		= 8
	};

	enum STATE
	{
		MOVE = 1,
		DEATH = 2,
		STOP = 3,
		WIN = 4,
		PUSH = 5,
		ISDEAD = 6,
		NOCOLIDE = 7
	};
	//-------------------------------------------------------------------------
	// Public Variables for This Class.
	//-------------------------------------------------------------------------
	STATE currentState;
	STATE specialState;

	DIRECTION directionState;
	Vec2 newPosition;
	std::string name;

	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
			 Unit(const BackBuffer *pBackBuffer, const char* texturePath, Vec2 position);
	virtual ~Unit();

	//-------------------------------------------------------------------------
	// Public Functions for This Class.
	//-------------------------------------------------------------------------
	void					Draw();
	void					Update();
	void					Move(ULONG ulDirection);
	Vec2&					Position();
	Vec2					getSize();
	int&					frameCounter();
	bool					WillColide(Unit& otherUnit, Unit::DIRECTION dir);
	bool					IsColided(Unit& otherUnit);


private:
	//-------------------------------------------------------------------------
	// Private Variables for This Class.
	//-------------------------------------------------------------------------

	Sprite*					_sprite;
	const BackBuffer*		_BF;
};

#endif // _CPLAYER_H_