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

	Vector3 GetLocationAtTime(float t);
	Vector3 GetTangentAtTime(float t);

	float GetLength() const;

private:

	void RecalculateSpline();
	void ComputeLength();

private:
	std::vector<Vector3> m_controlPoints;
	std::vector<DirectX::VertexPositionColor> m_splinePoints;
	std::vector<float> m_segmentLengths;
	std::vector<float> m_accumulatedLengths;
	int m_pointsPerSegment = 10;
	float m_length = 0.f;


	static constexpr float tension = 1.f;
};
