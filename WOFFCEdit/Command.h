#pragma once

#include "pch.h"
#include <string>

class Game;

class Command
{
public:

	Command() = default;
	virtual ~Command() = default;

	virtual void Execute(Game* game) = 0;
	virtual void Undo() = 0;
	virtual std::string GetName() const = 0;
	virtual std::string GetDebugData() const = 0;

protected:

	Game* m_game;
};

