//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//
// Desc: This file stores the player object class. This class performs tasks
//       such as player movement, some minor physics as well as rendering.
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CPlayer Specific Includes
//-----------------------------------------------------------------------------
#include "../Includes/Unit.h"

//-----------------------------------------------------------------------------
// Name : CPlayer () (Constructor)
// Desc : CPlayer Class Constructor
//-----------------------------------------------------------------------------
Unit::Unit(const BackBuffer *pBackBuffer, const char* texturePath, Vec2 position)
{
	_sprite	= new Sprite(texturePath, RGB(0xff, 0x00, 0xff));
	_sprite->setBackBuffer(pBackBuffer);
	_BF = pBackBuffer;

	// Animation frame crop rectangle
	RECT r;
	r.left		= 0;
	r.top		= 0;
	r.right		= 128;
	r.bottom	= 128;

	Position() = position;
	newPosition = position;
	currentState = Unit::STOP;
	directionState = (Unit::DIRECTION)0;
}

//-----------------------------------------------------------------------------
// Name : ~CPlayer () (Destructor)
// Desc : CPlayer Class Destructor
//-----------------------------------------------------------------------------
Unit::~Unit()
{
	delete _sprite;
}

void Unit::Draw()
{
	_sprite->draw();
}

void Unit::Update()
{
	if (Position().x < newPosition.x) {
		Position().x += 5;
	}
	else if (Position().x > newPosition.x) {
		Position().x -= 5;
	}

	if (Position().y < newPosition.y) {
		Position().y += 5;
	}
	else if (Position().y > newPosition.y) {
		Position().y -= 5;
	}

	if (abs(Position().x - newPosition.x) + abs(Position().y - newPosition.y) < 50) {
		directionState = Unit::DIR_STOP;
	}

	frameCounter()++;
}

void Unit::Move(ULONG ulDirection)
{
	if (Position() != newPosition) {
		return;
	}
	
	newPosition = Position();
	directionState = (Unit::DIRECTION)ulDirection;

	switch (ulDirection) {
	case DIRECTION::DIR_FORWARD:
		newPosition.y -= 100;
		break;
	case DIRECTION::DIR_BACKWARD:
		newPosition.y += 100;
		break;
	case DIRECTION::DIR_LEFT:
		newPosition.x -= 100;
		break;
	case DIRECTION::DIR_RIGHT:
		newPosition.x += 100;
		break;
	default:
		break;
	}
}

Vec2& Unit::Position()
{
	return _sprite->mPosition;
}

Vec2 Unit::getSize() 
{
	return Vec2(_sprite->width(), _sprite->height());
}

int& Unit::frameCounter()
{
	return _sprite->frameCounter;
}

bool Unit::WillColide(Unit& otherUnit, Unit::DIRECTION dir)
{
	Vec2 collisionPos;

	switch (dir)
	{
	case Unit::DIR_FORWARD:
		collisionPos = Vec2(Position().x, Position().y - 100);
		break;

	case Unit::DIR_BACKWARD:
		collisionPos = Vec2(Position().x, Position().y + 100);
		break;

	case Unit::DIR_LEFT:
		collisionPos = Vec2(Position().x - 100, Position().y);
		break;

	case Unit::DIR_RIGHT:
		collisionPos = Vec2(Position().x + 100, Position().y);
		break;
	}

	if (collisionPos.x > otherUnit.Position().x - 40 && collisionPos.x < otherUnit.Position().x + 40) {
		if (collisionPos.y > otherUnit.Position().y - 40 && collisionPos.y < otherUnit.Position().y + 40) {
			return true;
		}
	}

	return false;
}

bool Unit::IsColided(Unit& otherUnit)
{
	if (Position().x > otherUnit.Position().x - 7 && Position().x < otherUnit.Position().x + 7) {
		if (Position().y > otherUnit.Position().y - 7 && Position().y < otherUnit.Position().y + 7) {
			return true;
		}
	}

	return false;
}
