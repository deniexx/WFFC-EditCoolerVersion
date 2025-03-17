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
	ss << std::fixed << std::setprecision(2) << "Translation: \n";
	ss << "From: ";
	ss << "x: " << m_oldPos.x << "; y: " << m_oldPos.y << "; z: " << m_oldPos.z << '\n';
	ss << "To: " << "x: " << m_newPos.x << "; y: " << m_newPos.y << "; z: " << m_newPos.z << '\n';
	ss << "Rotation: \n";
	ss << "From: ";
	ss << "x: " << m_oldRot.x << "; y: " << m_oldRot.y << "; z: " << m_oldRot.z << '\n';
	ss << "To: " << "x: " << m_newRot.x << "; y: " << m_newRot.y << "; z: " << m_newRot.z << '\n';
	ss << "Scale: \n";
	ss << "From: ";
	ss << "x: " << m_oldSca.x << "; y: " << m_oldSca.y << "; z: " << m_oldSca.z << '\n';
	ss << "To: " << "x: " << m_newSca.x << "; y: " << m_newSca.y << "; z: " << m_newSca.z << '\n';

	return ss.str();
}
