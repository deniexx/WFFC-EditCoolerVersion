#include "Camera.h"
#include <algorithm>

Camera::Camera() 
	: m_movementInput(Vector3(0.f)), m_pitchInput(0.f), m_yawInput(0.f),  
	  m_cameraMoveSpeed(.3f), m_cameraRotateRate(3.f)
{
	m_cameraPos = Vector3(0.0f, 3.7f, -3.5f);

	m_cameraLookAt.x = 0.0f;
	m_cameraLookAt.y = 0.0f;
	m_cameraLookAt.z = 0.0f;

	m_cameraLookDirection.x = 0.0f;
	m_cameraLookDirection.y = 0.0f;
	m_cameraLookDirection.z = 0.0f;

	m_cameraRight.x = 0.0f;
	m_cameraRight.y = 0.0f;
	m_cameraRight.z = 0.0f;

	m_cameraOrientation.x = 0.0f;
	m_cameraOrientation.y = 0.0f;
	m_cameraOrientation.z = 0.0f;

	m_cameraMouseSensitivity = 0.5f;
}

void Camera::AddMovementInput(Vector3 input)
{
	m_movementInput += input;
}

void Camera::AddMouseInput(Vector3 input)
{
	m_mouseInput = input;
}

void Camera::AddPitchInput(float input)
{
	m_pitchInput += input;
}

void Camera::AddYawInput(float input)
{
	m_yawInput += input;
}

void Camera::Update()
{
	Vector3 planarMotionVector = m_cameraLookDirection;
	planarMotionVector.y = 0.0;

	if (m_mouseInput.Length() > 0.1f)
	{
		m_cameraOrientation.y -= m_mouseInput.x * m_cameraMouseSensitivity;
		m_cameraOrientation.x -= m_mouseInput.y * m_cameraMouseSensitivity;
	}
	if (m_pitchInput > 0.01f)
	{
		m_cameraOrientation.y -= m_cameraRotateRate;
	}
	if (m_pitchInput < -0.01f)
	{
		m_cameraOrientation.y += m_cameraRotateRate;
	}
	if (m_yawInput > 0.01f)
	{
		m_cameraOrientation.x += m_cameraRotateRate;
		m_cameraOrientation.x = std::min(m_cameraOrientation.x, 89.f);
	}
	if (m_yawInput < -0.1f)
	{
		m_cameraOrientation.x -= m_cameraRotateRate;
		m_cameraOrientation.x = std::max(m_cameraOrientation.x, -89.f);
	}

	float pitch = (m_cameraOrientation.y) * 3.1415 / 180;
	float yaw = (m_cameraOrientation.x) * 3.1415 / 180;
	//create look direction from Euler angles in m_camOrientation
	m_cameraLookDirection.x = sinf(pitch) * cosf(yaw);
	m_cameraLookDirection.y = sinf(yaw);
	m_cameraLookDirection.z = cosf(yaw) * cosf(pitch);
	m_cameraLookDirection.Normalize();

	//create right vector from look Direction
	m_cameraLookDirection.Cross(Vector3::UnitY, m_cameraRight);

	//process input and update stuff
	if (m_movementInput.x > 0.01f)
	{
		m_cameraPos += m_cameraLookDirection * m_cameraMoveSpeed;
	}
	if (m_movementInput.x < -0.01f)
	{
		m_cameraPos -= m_cameraLookDirection * m_cameraMoveSpeed;
	}
	if (m_movementInput.z > 0.01f)
	{
		m_cameraPos += m_cameraRight * m_cameraMoveSpeed;
	}
	if (m_movementInput.z < -0.01f)
	{
		m_cameraPos -= m_cameraRight * m_cameraMoveSpeed;
	}
	if (m_movementInput.y > 0.01f)
	{
		m_cameraPos.y += m_cameraMoveSpeed;
	}
	if (m_movementInput.y < -0.01f)
	{
		m_cameraPos.y -= m_cameraMoveSpeed;
	}

	//update lookat point
	m_cameraLookAt = m_cameraPos + m_cameraLookDirection;

	m_viewMatrix = Matrix::CreateLookAt(m_cameraPos, m_cameraLookAt, Vector3::UnitY);

	// Reset input
	m_pitchInput = 0.f;
	m_yawInput = 0.f;
	m_movementInput = Vector3::Zero;
	m_mouseInput = Vector3::Zero;
}

const Vector3& Camera::GetCameraPosition() const
{
	return m_cameraPos;
}

const Matrix& Camera::GetViewMatrix() const
{
	return m_viewMatrix;
}

