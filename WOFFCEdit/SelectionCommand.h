#pragma once

#include "Command.h"
#include <vector>

enum class SelectionType
{
	Add,
	Remove,
	Set,
	Clear
};

class SelectionCommand : public Command
{
public:

	SelectionCommand(int change, SelectionType type);
	
	virtual void Execute(Game* game) override;
	virtual void Undo() override;
	virtual std::string GetName() const override;
	virtual std::string GetDebugData() const override;

private:

	int m_change = 0;
	SelectionType m_type;

	std::vector<int> m_backedUpSelectionArray;
};

