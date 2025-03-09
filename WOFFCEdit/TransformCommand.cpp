#include "TransformCommand.h"
#include <sstream>
#include <iomanip>
//#include "ToolMain.h"

TransformCommand::TransformCommand(Vector3 pos, Vector3 rot, Vector3 sca,
	Vector3 oldPos, Vector3 oldRot, Vector3 oldSca, std::function<void(Vector3, Vector3, Vector3)> callback)
{
	m_newPos = pos;
	m_newRot = rot;
	m_newSca = sca;
	m_oldPos = oldPos;
	m_oldRot = oldRot;
	m_oldSca = oldSca;

	m_callback = callback;
}

void TransformCommand::Execute(Game* game)
{
	m_callback(m_newPos, m_newRot, m_newSca);
}

void TransformCommand::Undo()
{
	m_callback(m_oldPos, m_oldRot, m_oldSca);
}

std::string TransformCommand::GetName() const
{
	return "Transform Command";
}

std::string TransformCommand::GetDebugData() const
{
	std::stringstream ss;
	ss << std::fixed << std::setprecision(2) << "From: ";
	ss << "x: " << m_oldPos.x << "; y: " << m_oldPos.y << "; z: " << m_oldPos.z << '\n';
	ss << "To: " << "x: " << m_newPos.x << "; y: " << m_newPos.y << "; z: " << m_newPos.z << '\n';

	return ss.str();
}
