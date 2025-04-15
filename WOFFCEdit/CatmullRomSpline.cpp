#include "CatmullRomSpline.h"
#include "SimpleMath.h"

#include <numeric>
#include <cmath>

void CatmullRomSpline::AddPoint(const Vector3& point)
{
	m_controlPoints.push_back(point);
	RecalculateSpline();
}

void CatmullRomSpline::PopPoint()
{
	if (m_controlPoints.empty())
	{
		return;
	}

	m_controlPoints.pop_back();
	RecalculateSpline();
}

void CatmullRomSpline::SetQuality(int newValue)
{
	m_pointsPerSegment = newValue;
	RecalculateSpline();
}

void CatmullRomSpline::AddToQuality(int delta)
{
	m_pointsPerSegment += delta;
	RecalculateSpline();
}

const std::vector<DirectX::VertexPositionColor>& CatmullRomSpline::GetSplinePoints()
{
	return m_splinePoints;
}

const std::vector<Vector3>& CatmullRomSpline::GetControlPoints()
{
	return m_controlPoints;
}

Vector3 CatmullRomSpline::GetLocationAtTime(float t)
{
	if (t <= 0.f || m_splinePoints.size() < 2)
	{
		return (m_splinePoints.empty() ? Vector3::Zero : m_splinePoints[0].position);
	}

	if (t >= 1.f)
	{
		return m_splinePoints.back().position;
	}

	float targetDistance = t * m_length;
	int segmentIndex = 0;
	for (int i = 1; i < m_accumulatedLengths.size(); ++i) 
	{
		if (m_accumulatedLengths[i] >= targetDistance) {
			segmentIndex = i - 1;
			break;
		}
	}

	float segmentStartDistance = m_accumulatedLengths[segmentIndex];
	float segmentLength = m_segmentLengths[segmentIndex];

	if (segmentLength < std::numeric_limits<float>::epsilon()) 
	{
		return (m_splinePoints[segmentIndex].position);
	}

	float distanceIntoSegment = targetDistance - segmentStartDistance;
	float t_segment = distanceIntoSegment / segmentLength;

	return Vector3::Lerp(
		m_splinePoints[segmentIndex].position,
		m_splinePoints[segmentIndex + 1].position,
		t_segment
	);
}

Vector3 CatmullRomSpline::GetTangentAtTime(float t)
{
	if (m_splinePoints.size() < 2)
	{
		return Vector3::Forward;
	}
	
	t = std::max(0.f, std::min(1.f, t));

	float targetDistance = t * m_length;
	int segmentIndex = 0;
	for (int i = 1; i < m_accumulatedLengths.size(); ++i)
	{
		if (m_accumulatedLengths[i] >= targetDistance) 
		{
			segmentIndex = i - 1;
			break;
		}
		if (i == m_accumulatedLengths.size() - 1 && targetDistance >= m_accumulatedLengths[i]) {
			segmentIndex = i - 1;
			break;
		}
	}
	Vector3 tangent = (Vector3)m_splinePoints[segmentIndex + 1].position - (Vector3)m_splinePoints[segmentIndex].position;
	tangent.Normalize();

	if (tangent.LengthSquared() < std::numeric_limits<float>::epsilon()) 
	{
		if (segmentIndex > 0) 
		{
			tangent = (Vector3)m_splinePoints[segmentIndex].position - (Vector3)m_splinePoints[segmentIndex - 1].position;
			tangent.Normalize();
		}
		else if (segmentIndex + 2 < m_splinePoints.size()) 
		{
			tangent = (Vector3)m_splinePoints[segmentIndex + 2].position - (Vector3)m_splinePoints[segmentIndex + 1].position;
			tangent.Normalize();
		}
		else 
		{
			tangent = Vector3::Forward;
		}
		if (tangent.LengthSquared() < std::numeric_limits<float>::epsilon()) {
			tangent = Vector3::Forward;
		}
	}
	return tangent;
}

float CatmullRomSpline::GetLength() const
{
	return m_length;
}

void CatmullRomSpline::RecalculateSpline()
{
	m_splinePoints.clear();

	auto controlPointsAmount = m_controlPoints.size();
    for (int i = 0; i < controlPointsAmount - 1; i++)
    {
        Vector3 prev = i == 0 ? m_controlPoints[i] : m_controlPoints[i - 1];
        Vector3 currStart = m_controlPoints[i];
        Vector3 currEnd = m_controlPoints[i + 1];
        Vector3 next = i == controlPointsAmount - 2 ? m_controlPoints[i + 1] : m_controlPoints[i + 2];

        for (int step = 0; step <= m_pointsPerSegment; step++)
        {
            const float t = (float)step / m_pointsPerSegment;
            Vector3 interpolatedPoint = Vector3::CatmullRom(prev, currStart, currEnd, next, t);
			interpolatedPoint.y += 1;
            m_splinePoints.push_back(DirectX::VertexPositionColor(interpolatedPoint, Vector3(1.f, 1.f, 1.f)));
        }
    }

	ComputeLength();
}

void CatmullRomSpline::ComputeLength()
{
	m_length = 0.f;
	m_segmentLengths.clear();
	m_accumulatedLengths.clear();
	if (m_controlPoints.size() < 2) return;
	if (m_splinePoints.size() < 2) return;

	m_segmentLengths.reserve(m_splinePoints.size() - 1);
	m_accumulatedLengths.reserve(m_splinePoints.size());
	m_accumulatedLengths.push_back(0.f);

	for (int i = 0; i < m_splinePoints.size() - 1; ++i) 
	{
		float len = Vector3::Distance(m_splinePoints[i].position, m_splinePoints[i + 1].position);
		
		if (len < std::numeric_limits<float>::epsilon()) 
		{
			len = 0.f;
		}
		m_segmentLengths.push_back(len);
		m_length += len;
		m_accumulatedLengths.push_back(m_length);
	}
}
