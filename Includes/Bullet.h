//-----------------------------------------------------------------------------
// File: Bullet.cpp
//
// Desc: This file contains the class that represents the bullet object.
//
// Created by Vasilescu Vlad
//-----------------------------------------------------------------------------
#ifndef _BULLET_H_
#define _BULLET_H_

//-----------------------------------------------------------------------------
// ScoreSprite Specific Includes
//-----------------------------------------------------------------------------
#include "Sprite.h"

//-----------------------------------------------------------------------------
// Main Class Definitions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : Bullet (Class)
// Desc : Bullet class that stores which team shot the bullet and
// updates the sprite.
//-----------------------------------------------------------------------------
class Bullet: public Sprite
{
public:
	//-------------------------------------------------------------------------
	// Public Variables for This Class.
	//-------------------------------------------------------------------------
	ULONG team;

	//-------------------------------------------------------------------------
	// Constructor for This Class.
	//-------------------------------------------------------------------------
	Bullet(const char *szImageFile, COLORREF crTransparentColor) :
		Sprite(szImageFile, crTransparentColor) {};
};


#endif