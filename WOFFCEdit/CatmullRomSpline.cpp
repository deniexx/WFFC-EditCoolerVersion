#include "CatmullRomSpline.h"
#include "SimpleMath.h"

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
}
