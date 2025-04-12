#pragma once

#include "pch.h"

using DirectX::SimpleMath::Vector3;

class CatmullRomSpline
{
public:
	
	CatmullRomSpline() = default;

	void AddPoint(const Vector3& point);
	void PopPoint();
	void SetQuality(int newValue);
	void AddToQuality(int delta);
	const std::vector<DirectX::VertexPositionColor>& GetSplinePoints();
	const std::vector<Vector3>& GetControlPoints();

private:

	void RecalculateSpline();

private:
	std::vector<Vector3> m_controlPoints;
	std::vector<DirectX::VertexPositionColor> m_splinePoints;
	int m_pointsPerSegment = 4;
	static constexpr float tension = 1.f;
};
