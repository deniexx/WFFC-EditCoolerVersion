#pragma once

#include "Command.h"

using namespace DirectX::SimpleMath;

class TransformCommand : public Command
{
public:
	
	TransformCommand(Vector3 pos, Vector3 rot, Vector3 sca, Vector3 oldPos, Vector3 oldRot, Vector3 oldSca, std::function<void(Vector3, Vector3, Vector3)> callback);

	virtual void Execute(Game* game) override;
	virtual void Undo() override;
	virtual std::string GetName() const override;
	virtual std::string GetDebugData() const override;

private:

	std::function<void(Vector3, Vector3, Vector3)> m_callback;

	Vector3 m_oldPos;
	Vector3 m_oldRot;
	Vector3 m_oldSca;

	Vector3 m_newPos;
	Vector3 m_newRot;
	Vector3 m_newSca;
};
