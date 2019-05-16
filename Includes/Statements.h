#pragma once

#include "Main.h"
#include "Sprite.h"
#include "Unit.h"

#include <list>

class Statements
{
public:
	std::list<Unit*> prefixes;
	std::list<Unit*> sufixes;
	std::list<Unit*> connectors;

	Unit::STATE GetState(std::string unitName);
	void Update();
	void Draw();
};