#include "SelectionCommand.h"
#include "Game.h"

SelectionCommand::SelectionCommand(int change, SelectionType type) 
	: m_change(change), m_type(type)
{
}

void SelectionCommand::Execute(Game* game)
{
	m_game = game;
	m_backedUpSelectionArray = game->GetPickedObjects();

	switch (m_type)
	{
	case SelectionType::Add:
		game->AddPickedObject(m_change);
		break;
	case SelectionType::Remove:
		game->RemovePickedObject(m_change);
		break;
	case SelectionType::Set:
		game->SetPickedObject(m_change);
		break;
	case SelectionType::Clear:
		game->SetPickedObjectsVector(std::vector<int>());
		break;
	}
}

void SelectionCommand::Undo()
{
	m_game->SetPickedObjectsVector(m_backedUpSelectionArray);
}

std::string SelectionCommand::GetName() const
{
	return "Selection Command";
}

std::string SelectionCommand::GetDebugData() const
{
	std::string debugData;

	switch (m_type)
	{
	case SelectionType::Add:
		debugData = "Add: ";
		break;
	case SelectionType::Remove:
		debugData = "Remove: ";
		break;
	case SelectionType::Set:
		debugData = "Set: ";
		break;
	case SelectionType::Clear:
		debugData = "Clear: ";
		break;
	}

	return debugData + std::to_string(m_change);
}
