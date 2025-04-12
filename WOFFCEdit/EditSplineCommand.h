#pragma once

#include "Command.h"

class EditSplineCommand : public Command
{
public:

	EditSplineCommand(DirectX::SimpleMath::Vector3 newPoint);

	virtual void Execute(Game* game) override;
	virtual void Undo() override;
	virtual std::string GetName() const override;
	virtual std::string GetDebugData() const override;

private:

	DirectX::SimpleMath::Vector3 m_newPoint;
};

