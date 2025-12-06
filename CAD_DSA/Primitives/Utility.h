#pragma once
#include "Point2D.h"
#include "Constants.h"
#include "OrientationType.h"
#include <math.h>


namespace Primitives
{

	class Utility
	{
	public:
		Utility() = delete;
		Utility(const Utility&) = delete;
		Utility& operator=(const Utility&) = delete;

		template <typename T>
		static double CrossProduct(const Point2D<T>& SPoint, const Point2D<T>& EPoint, const Point2D<T>& i_point)
		{
			double i_point_SPoint_X = i_point.GetX() - SPoint.GetX();
			double i_point_SPoint_Y = i_point.GetY() - SPoint.GetY();

			double EPoint_SPoint_X = EPoint.GetX() - SPoint.GetX();
			double EPoint_SPoint_Y = EPoint.GetY() - SPoint.GetY();

			return i_point_SPoint_Y * EPoint_SPoint_X - i_point_SPoint_X * EPoint_SPoint_Y;
		}

		template <typename T>
		static double DotProduct(const Point2D<T>& SPoint, const Point2D<T>& EPoint, const Point2D<T>& i_point)
		{
			double i_point_SPoint_X = i_point.GetX() - SPoint.GetX();
			double i_point_SPoint_Y = i_point.GetY() - SPoint.GetY();

			double EPoint_SPoint_X = EPoint.GetX() - SPoint.GetX();
			double EPoint_SPoint_Y = EPoint.GetY() - SPoint.GetY();

			return  i_point_SPoint_X * EPoint_SPoint_X + i_point_SPoint_Y * EPoint_SPoint_Y;
		}

		template <typename T>
		static bool ArePointsCollinear(const Point2D<T>& SPoint, const Point2D<T>& EPoint, const Point2D<T>& i_point)
		{
			double crossProduct = CrossProduct(SPoint, EPoint, i_point);
			return std::fabs(crossProduct) <= TOLERANCE;
		}

		template <typename T>
		static bool IsPointPresentOnLineSegment(const Point2D<T>& SPoint, const Point2D<T>& EPoint, const Point2D<T>& i_point)
		{
			double i_point_SPoint_X = i_point.GetX() - SPoint.GetX();
			double i_point_SPoint_Y = i_point.GetY() - SPoint.GetY();

			double EPoint_SPoint_X = EPoint.GetX() - SPoint.GetX();
			double EPoint_SPoint_Y = EPoint.GetY() - SPoint.GetY();

			double crossProduct = i_point_SPoint_Y * EPoint_SPoint_X - i_point_SPoint_X * EPoint_SPoint_Y;

			if (std::fabs(crossProduct) > TOLERANCE)
			{
				return false;
			}
			else
			{
				double dotProduct = i_point_SPoint_X * EPoint_SPoint_X + i_point_SPoint_Y * EPoint_SPoint_Y;
				if (dotProduct < 0)
				{
					return false;
				}
				double squaredLength = EPoint_SPoint_X * EPoint_SPoint_X + EPoint_SPoint_Y * EPoint_SPoint_Y;
				if (dotProduct > squaredLength)
				{
					return false;
				}
				return true;
			}
		}

		template <typename T>
		static OrientationType Orientation(const Point2D<T>& SPoint, const Point2D<T>& EPoint, const Point2D<T>& i_point)
		{
			double crossProduct = CrossProduct(SPoint, EPoint, i_point);
			if (std::fabs(crossProduct) <= TOLERANCE)
			{
				return OrientationType::COLLINEAR;
			}
			else
			{
				return (crossProduct > 0.0) ? OrientationType::CONVEX : OrientationType::CONCAVE;
			}
		}

		template <typename T>
		static double GetSweepAngle(std::vector<Point2D<T>> i_vtPoints, const Point2D<T>& i_point)
		{
			double sweepAngle = 0.0;
			auto size = i_vtPoints.size();
			for (auto i = 0; i < size; ++i)
			{
				auto j = (i + 1) % size;
				sweepAngle += Utility::GetSweepAngle<T>(i_vtPoints[i], i_vtPoints[j], i_point);
			}

			return sweepAngle;
		}

		template <typename T>
		static double GetSweepAngle(const Point2D<T>& SPoint, const Point2D<T>& EPoint, const Point2D<T>& i_point)
		{
			if (i_point == SPoint || i_point == EPoint)	return 0.0;
			double angle = atan2(EPoint.GetY() - i_point.GetY(), EPoint.GetX() - i_point.GetX()) -
				atan2(SPoint.GetY() - i_point.GetY(), SPoint.GetX() - i_point.GetX());

			if (angle > PI)angle -= TWO_PI;
			if (angle < -PI)angle += TWO_PI;
			return angle;
		}

		template <typename T>
		static bool DoIntersectInBetween(const Point2D<T>& SPoint1, const Point2D<T>& EPoint1, 
			const Point2D<T>& SPoint2, const Point2D<T>& EPoint2) 
		{
			//exlude intersection at end point
			if (SPoint1 == SPoint2)
				return false;
			if (SPoint1 == EPoint2)
				return false;

			if (EPoint1 == SPoint2)
				return false;
			if (EPoint1 == EPoint2)
				return false;

			OrientationType o1 = Orientation(SPoint1, EPoint1, SPoint2);
			OrientationType o2 = Orientation(SPoint1, EPoint1, EPoint2);
			OrientationType o3 = Orientation(SPoint2, EPoint2, SPoint1);
			OrientationType o4 = Orientation(SPoint2, EPoint2, EPoint1);

			if (o1 != o2 && o3 != o4)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

	};
}