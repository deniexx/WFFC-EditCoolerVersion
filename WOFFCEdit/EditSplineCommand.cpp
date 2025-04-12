#include "EditSplineCommand.h"
#include "Game.h"
#include "sstream"
#include "iomanip"

EditSplineCommand::EditSplineCommand(DirectX::SimpleMath::Vector3 newPoint)
{
	m_newPoint = newPoint;
}

void EditSplineCommand::Execute(Game* game)
{
	m_game = game;
	m_game->AddPointToSpline(m_newPoint);
}

void EditSplineCommand::Undo()
{
	m_game->PopLastSplinePoint();
}

std::string EditSplineCommand::GetName() const
{
	return "Edit Spline Command";
}

std::string EditSplineCommand::GetDebugData() const
{
	std::stringstream ss;
	ss << "Added new point at: " << std::setprecision(2);
	ss << "X: " << m_newPoint.x << ", Y: " << m_newPoint.y << ", Z: " << m_newPoint.z;
	return ss.str();
}
