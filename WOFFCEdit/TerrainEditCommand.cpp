#include "TerrainEditCommand.h"
#include "Game.h"

TerrainEditCommand::TerrainEditCommand(const std::vector<BYTE>& preEditHeightMap, const std::vector<BYTE>& afterEditHeightMap)
{
	m_preEditHeightMap = preEditHeightMap;
	m_afterEditHeightMap = afterEditHeightMap;
}

void TerrainEditCommand::Execute(Game* game)
{
	m_game = game;
	m_game->SetTerrainHeightMap(m_afterEditHeightMap);
}

void TerrainEditCommand::Undo()
{
	m_game->SetTerrainHeightMap(m_preEditHeightMap);
}

std::string TerrainEditCommand::GetName() const
{
	return "Terrain Edit Command";
}

std::string TerrainEditCommand::GetDebugData() const
{
	return "No debug data";
}


