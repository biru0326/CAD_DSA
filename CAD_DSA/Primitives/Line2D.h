#pragma once
#include <cmath>
#include <optional>
#include <stdexcept>
#include <algorithm>

#include "Point2D.h"
#include "Constants.h"

namespace Primitives
{
    enum class LineRelation
    {
        NONE,
        PARALLEL,
        COLLINEAR,
        INTERSECTING,
        OVERLAPPING
    };

    template<typename T>
    class Line2D
    {
    private:
        Point2D<T> m_Start;
        Point2D<T> m_End;

        // =========================
        // Math helpers
        // =========================
        static T Dot(const Point2D<T>& a, const Point2D<T>& b)
        {
            return a.GetX() * b.GetX() + a.GetY() * b.GetY();
        }

        static T Cross(const Point2D<T>& a, const Point2D<T>& b)
        {
            return a.GetX() * b.GetY() - a.GetY() * b.GetX();
        }

        static T Length(const Point2D<T>& v)
        {
            return std::sqrt(v.GetX() * v.GetX() + v.GetY() * v.GetY());
        }

        static Point2D<T> Normalize(const Point2D<T>& v)
        {
            T len = Length(v);
            if (len < TOLERANCE)
                throw std::runtime_error("Zero length vector");

            return Point2D<T>(v.GetX() / len, v.GetY() / len);
        }

    public:
        // =========================
        // Constructor
        // =========================
        Line2D(const Point2D<T>& start, const Point2D<T>& end)
            : m_Start(start), m_End(end)
        {
            if (Length(m_End - m_Start) < TOLERANCE)
                throw std::runtime_error("Degenerate line");
        }

        // =========================
        // Basic
        // =========================
        Point2D<T> GetStart() const { return m_Start; }
        Point2D<T> GetEnd() const { return m_End; }

        T Length() const { return Length(m_End - m_Start); }

        Point2D<T> Direction() const
        {
            return Normalize(m_End - m_Start);
        }

        // =========================
        // Tangents (your semantics)
        // =========================
        Point2D<T> TangentAtStart() const
        {
            return Normalize(m_End - m_Start);
        }

        Point2D<T> TangentAtEnd() const
        {
            return Normalize(m_Start - m_End);
        }

        // =========================
        // Parameterization
        // =========================
        Point2D<T> PointAtParameter(double t) const
        {
            return Point2D<T>(
                m_Start.GetX() + (m_End.GetX() - m_Start.GetX()) * t,
                m_Start.GetY() + (m_End.GetY() - m_Start.GetY()) * t
            );
        }

        double ParameterOfPoint(const Point2D<T>& pt) const
        {
            // Start with:
            // P = start + t*d
            // where:
            // d = end - start
            // P - start = t*d
            // Since vectors cannot be divided, take dot product with d:
            // (P - start) · d = (t*d) · d
            // Using dot product properties:
            // (P - start) · d = t * (d · d)
            // Divide by scalar:
            // t = ((P - start) · d) / (d · d)
            
            Point2D<T> d = m_End - m_Start;
            return Dot(pt - m_Start, d) / Dot(d, d);
        }

        // =========================
        // Projection / Closest
        // =========================
        Point2D<T> ProjectPoint(const Point2D<T>& pt) const
        {
            return PointAtParameter(ParameterOfPoint(pt));
        }

        Point2D<T> ClosestPoint(const Point2D<T>& pt) const
        {
            double t = ParameterOfPoint(pt);
            t = std::max(0.0, std::min(1.0, t));
            return PointAtParameter(t);
        }

        double PerpendicularDistanceToPointOnInfiniteLine(const Point2D<T>& pt) const
        {
            return Length(pt - ProjectPoint(pt));
        }

        double DistanceToPointOnSegment(const Point2D<T>& pt) const
        {
            return Length(pt - ClosestPoint(pt));
        }

        // =========================
        // Point checks
        // =========================
        bool IsPointOnInfiniteLine(const Point2D<T>& pt) const
        {
            return std::fabs(Cross(m_End - m_Start, pt - m_Start)) < TOLERANCE;
        }

        bool IsPointOnLineSegment(const Point2D<T>& pt) const
        {
            if (!IsPointOnInfiniteLine(pt))
                return false;

            double t = ParameterOfPoint(pt);
            return (t >= -TOLERANCE && t <= 1 + TOLERANCE);
        }

        // =========================
        // Extend
        // =========================
        Line2D ExtendAtStart(T dist) const
        {
            Point2D<T> dir = Normalize(m_Start - m_End);
            return Line2D(m_Start + dir * dist, m_End);
        }

        Line2D ExtendAtEnd(T dist) const
        {
            Point2D<T> dir = Normalize(m_End - m_Start);
            return Line2D(m_Start, m_End + dir * dist);
        }

        // =========================
        // Midpoint
        // =========================
        Point2D<T> MidPoint() const
        {
            return (m_Start + m_End) * 0.5;
        }

        // =========================
        // Relations
        // =========================
        bool IsParallel(const Line2D& other) const
        {
            return std::fabs(Cross(Direction(), other.Direction())) < TOLERANCE;
        }

        bool IsPerpendicular(const Line2D& other) const
        {
            return std::fabs(Dot(Direction(), other.Direction())) < TOLERANCE;
        }

        double AngleWith(const Line2D& other) const
        {
            double d = Dot(Direction(), other.Direction());
            d = std::max(-1.0, std::min(1.0, d));
            return std::acos(d);
        }

        double AngleWithFull(const Line2D& other) const
        {
            // Compute full CCW angle from this line to other line.
            // d1 = normalized direction of current line
            // d2 = normalized direction of other line
            // Dot product gives:
            // dot = d1 · d2 = cos(theta)
            // Cross product in 2D gives:
            // cross = d1.x*d2.y - d1.y*d2.x = sin(theta)
            // So together:
            // (dot, cross) = (cos(theta), sin(theta))
            // which represents a unique point on the unit circle.
            // Intuition:
            // - dot   -> horizontal coordinate on unit circle
            // - cross -> vertical coordinate on unit circle
            // Using only dot product is insufficient because:
            // cos(+theta) == cos(-theta)
            // so dot alone cannot determine CW/CCW orientation
            // or full 360-degree angle.
            // Cross product preserves orientation:
            // - cross > 0 -> CCW
            // - cross < 0 -> CW
            // atan2(y, x) reconstructs the full angle from the
            // unit-circle coordinates:
            // angle = atan2(sin(theta), cos(theta))
            // Here:
            // angle = atan2(cross, dot)
            // atan2 returns angle in range:
            // [-PI, +PI]
            // Convert negative angles into full CCW range:
            // [0, 2*PI]
            // Final result:
            // - always full CCW angle
            // - range: [0, 2*PI]
            
            Point2D<T> d1 = Direction();
            Point2D<T> d2 = other.Direction();
        
            double dot = Dot(d1, d2);
            double cross = d1.GetX()*d2.GetY() - d1.GetY()*d2.GetX();
        
            double angle = std::atan2(cross, dot);
        
            if (angle < 0)
                angle += 2 * M_PI;
        
            return angle;
        }

        double DistanceBetweenParallelLines(const Line2D& other) const
        {
            // Compute perpendicular distance between two parallel lines.
            //
            // Let:
            //
            // diff = vector between a point on this line and
            //        a point on the other line.
            //
            // Direction() returns normalized line direction.
            //
            // Geometric meaning of 2D cross product:
            //
            // |a x b| = |a||b|sin(theta)
            //
            // Since Direction() is normalized:
            //
            // |Direction()| = 1
            //
            // Therefore:
            //
            // |Cross(diff, Direction())|
            //
            // becomes:
            //
            // |diff|sin(theta)
            //
            // which is exactly the perpendicular distance component
            // between the two parallel lines.
            //
            // Intuition:
            // - dot product extracts parallel component
            // - cross product extracts perpendicular component
            //
            // fabs() is used because distance is always non-negative.
            
            Point2D<T> diff = other.m_Start - m_Start;
            return std::fabs(Cross(diff, Direction()));
        }

        // =========================
        // Normal / Offset
        // =========================
        Point2D<T> NormalCCW() const
        {
            Point2D<T> d = Direction();
            return Point2D<T>(-d.GetY(), d.GetX());
        }

        Line2D Offset(T dist) const
        {
            Point2D<T> n = NormalCCW();
            return Line2D(m_Start + n * dist, m_End + n * dist);
        }

        // =========================
        // Intersection (robust)
        // =========================
        LineRelation Intersect(
            const Line2D& other,
            std::optional<Point2D<T>>& outPoint) const
        {
            Point2D<T> r = m_End - m_Start;
            Point2D<T> s = other.m_End - other.m_Start;

            double denom = Cross(r, s);
            Point2D<T> diff = other.m_Start - m_Start;

            if (std::fabs(denom) < TOLERANCE)
            {
                if (std::fabs(Cross(diff, r)) < TOLERANCE)
                {
                    // collinear → check overlap
                    double t0 = Dot(diff, r) / Dot(r, r);
                    double t1 = t0 + Dot(s, r) / Dot(r, r);

                    if (std::max(std::min(t0, t1), 0.0) <=
                        std::min(std::max(t0, t1), 1.0))
                    {
                        return LineRelation::OVERLAPPING;
                    }

                    return LineRelation::COLLINEAR;
                }

                return LineRelation::PARALLEL;
            }

            double t = Cross(diff, s) / denom;
            double u = Cross(diff, r) / denom;

            if (t >= -TOLERANCE && t <= 1 + TOLERANCE &&
                u >= -TOLERANCE && u <= 1 + TOLERANCE)
            {
                outPoint = PointAtParameter(t);
                return LineRelation::INTERSECTING;
            }

            return LineRelation::NONE;
        }

        bool IsIntersecting(const Line2D& other) const
        {
            std::optional<Point2D<T>> pt;
            return Intersect(other, pt) == LineRelation::INTERSECTING;
        }

        // =========================
        // Bounding box
        // =========================
        Rect<T> BoundingBox() const
        {
            return Rect<T>(
                std::min(m_Start.GetX(), m_End.GetX()),
                std::min(m_Start.GetY(), m_End.GetY()),
                std::max(m_Start.GetX(), m_End.GetX()),
                std::max(m_Start.GetY(), m_End.GetY())
            );
        }
    };
}
