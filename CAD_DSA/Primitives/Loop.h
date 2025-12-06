#pragma once
#include "Utility.h"
#include "Rect.h"
#include "LoopDirection.h"
#include <string>
#include <vector>
#include <set>

namespace Primitives
{
	template<typename T>
	class Loop
	{
	private:
		bool m_IsOuter = true;
		LoopDirection m_LoopDirection = LoopDirection::CCW;
		char m_Name;
		Rect<T> m_Rect;
		std::vector<Point2D<T>> m_OrgPoints;
		std::vector<Point2D<T>> m_MergedPoints;

	public:
		Loop(bool i_IsOuter, 
			LoopDirection i_LoopDirection, 
			const char& i_Name,
			std::vector<Point2D<T>>& i_Points) :
			m_IsOuter(i_IsOuter),
			m_LoopDirection(i_LoopDirection),
			m_Name(i_Name), 
			m_OrgPoints(std::move(i_Points))
		{
			MergeCollinearPoints();
			SetRect();
		}

	private:
		void MergeCollinearPoints()
		{
			auto size = m_OrgPoints.size();
			if (size < 4)//triangle
				return;

			m_MergedPoints.push_back(m_OrgPoints[0]);
			m_MergedPoints.push_back(m_OrgPoints[1]);
			
			for (auto i = 2; i < size; ++i)
			{
				bool collinear = Utility::ArePointsCollinear<T>(
					m_MergedPoints[m_MergedPoints.size()-2], 
					m_MergedPoints[m_MergedPoints.size() - 1], 
					m_OrgPoints[i]);
				if (collinear)
				{
					m_MergedPoints[m_MergedPoints.size() - 1].Change(m_OrgPoints[i].GetX(), m_OrgPoints[i].GetY());
				}
				else
				{
					m_MergedPoints.push_back(m_OrgPoints[i]);
				}
			}
		}

		void SetRect()
		{
			if (m_MergedPoints.empty())
			{
				return;
			}
			else
			{
				T minX = m_MergedPoints[0].GetX();
				T minY = m_MergedPoints[0].GetY();
				T maxX = m_MergedPoints[0].GetX();
				T maxY = m_MergedPoints[0].GetY();

				for (size_t i = 0; i < m_MergedPoints.size(); ++i)
				{
					if (minX > m_MergedPoints[i].GetX())
					{
						minX = m_MergedPoints[i].GetX();
					}
					if (maxX < m_MergedPoints[i].GetX())
					{
						maxX = m_MergedPoints[i].GetX();
					}
					if (minY > m_MergedPoints[i].GetY())
					{
						minY = m_MergedPoints[i].GetY();
					}
					if (maxY < m_MergedPoints[i].GetY())
					{
						maxY = m_MergedPoints[i].GetY();
					}
				}

				m_Rect.Change(minX, minY, maxX, maxY);
			}
		}

	public:
		bool IsPointOnOrInsidePolygon(const Point2D<T>& i_point, bool& isInside)const
		{
			return IsPointOnOrInsidePolygon(m_MergedPoints, i_point, isInside);
		}

		bool IsPointOnOrInsidePolygon(std::vector<Point2D<T>> i_vtPoints, const Point2D<T>& i_point, bool& isInside)const
		{
			if (IsPointOnPolygon(i_vtPoints, i_point))
			{
				return true;
			}
			else
			{
				isInside = IsPointInsidePolygon(i_vtPoints, i_point);
				return isInside;
			}
		}

		bool IsPointOnPolygon(std::vector<Point2D<T>> i_vtPoints, const Point2D<T>& i_point)const
		{
			auto size = i_vtPoints.size();
			for (auto i = 0; i < size; ++i)
			{
				auto j = (i + 1) % size;
				if (Utility::IsPointPresentOnLineSegment<T>(i_vtPoints[i], i_vtPoints[j], i_point))
				{
					return true;
				}
			}
			return false;
		}

		bool IsPointInsidePolygon(const Point2D<T>& i_point)const
		{
			return IsPointInsidePolygon(m_MergedPoints, i_point);
		}

		bool IsPointInsidePolygon(std::vector<Point2D<T>> i_vtPoints, const Point2D<T>& i_point)const
		{
			double sweepAngle = Utility::GetSweepAngle<T>(i_vtPoints, i_point);
			if (sweepAngle<0.1 && sweepAngle>-0.1)return false;
			return true;
		}

		int Triangulate()
		{
			std::vector<Point2D<T>> currPoints = m_MergedPoints;
			int triangleCount = 0;
			while (currPoints.size() > 2)
			{
				int size = (int)currPoints.size();
				for (int i = 0; i < size; ++i)
				{
					int prevIndex = i - 1;
					if (prevIndex < 0)
						prevIndex = size - 1;
					int currentIndex = i;
					int nextIndex = (i + 1) % size;

					std::set<int> runnigIndexes = { prevIndex ,currentIndex, nextIndex };
					auto end = runnigIndexes.end();

					OrientationType type = Utility::Orientation<T>(currPoints[prevIndex], currPoints[currentIndex], currPoints[nextIndex]);
					if (type != OrientationType::CONVEX)
					{
						continue;
					}

					std::vector<Point2D<T>> vtTrianglePoints = { currPoints[prevIndex], currPoints[currentIndex], currPoints[nextIndex] };
					bool istriangleValid = true;
					for (int j = 0; j < size; ++j)
					{
						int lineP1Index = j;
						int lineP2Index = (j + 1) % size;

						//optimization
						if (runnigIndexes.find(lineP1Index) != end && runnigIndexes.find(lineP2Index) != end)
							continue;

						if (Utility::DoIntersectInBetween<T>(currPoints[prevIndex], currPoints[nextIndex],
							currPoints[lineP1Index], currPoints[lineP2Index]))
						{
							istriangleValid = false;
							break;
						}

						if (j == prevIndex || j == currentIndex || j == nextIndex)
						{
							continue;
						}

						if (IsPointInsidePolygon(vtTrianglePoints, currPoints[j]))
						{
							istriangleValid = false;
							break;
						}
					}

					if (!istriangleValid)
					{
						continue;
					}
					else
					{
						++triangleCount;
						std::vector<Point2D<T>> tempPoints = currPoints;
						currPoints.clear();
						for (int k = 0; k < size; ++k)
						{
							if (k == currentIndex)
								continue;
							currPoints.push_back(tempPoints[k]);
						}
						size = int(currPoints.size());
					}
				}
			}

			return triangleCount;
		}

		T GetWidth()const
		{
			return m_Rect.getXMax() - m_Rect.getXMin();
		}

		T GetHeight()const
		{
			return m_Rect.getYMax() - m_Rect.getYMin();
		}

		char GetName()const
		{
			return m_Name;
		}

		void GetOriginalPoints(std::vector<Point2D<T>>& i_OrgPoints)const
		{
			i_OrgPoints.insert(i_OrgPoints.end(), m_OrgPoints.begin(), m_OrgPoints.end());
			
		}

		bool IsOuter()const
		{
			return m_IsOuter;
		}
	};
}