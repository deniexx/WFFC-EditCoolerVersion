#pragma once

#include "pch.h"

typedef DirectX::SimpleMath::Vector3 Vector3;
typedef DirectX::SimpleMath::Matrix Matrix;

class Camera
{
public:

	Camera();

	void AddMovementInput(Vector3 input);
	void AddMouseInput(Vector3 input);
	void AddPitchInput(float input);
	void AddYawInput(float input);

	void Update();

	const Vector3& GetCameraPosition() const;
	const Vector3& GetCameraRotation() const;
	const Matrix& GetViewMatrix() const;

private:

	int sign(float number) const;

private:

	Matrix m_viewMatrix;

	Vector3 m_movementInput;
	float m_pitchInput;
	float m_yawInput;
	float m_cameraMouseSensitivity;

	Vector3 m_mouseInput;
	Vector3 m_cameraRight;
	Vector3 m_cameraLookDirection;
	Vector3 m_cameraLookAt;
	Vector3 m_cameraPos;
	Vector3 m_cameraOrientation;
	float m_cameraMoveSpeed;
	float m_cameraRotateRate;
};

