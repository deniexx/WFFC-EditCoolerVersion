#pragma once
#include "Command.h"

class TerrainEditCommand : public Command
{
public:

	TerrainEditCommand(const std::vector<BYTE>& preEditHeightMap, const std::vector<BYTE>& afterEditHeightMap);

	virtual void Execute(Game* game);
	virtual void Undo();
	virtual std::string GetName() const;
	virtual std::string GetDebugData() const;

private:

	std::vector<BYTE> m_preEditHeightMap;
	std::vector<BYTE> m_afterEditHeightMap;
};

