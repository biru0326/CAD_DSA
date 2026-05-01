
#pragma once
#include <cmath>
#include <optional>
#include "Point2D.h"
#include "Constants.h"

namespace Primitives
{
    template<typename T>
    class Line2D
    {
    private:
        Point2D<T> m_Start;
        Point2D<T> m_End;

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
        Line2D(const Point2D<T>& i_start, const Point2D<T>& i_end)
            : m_Start(i_start), m_End(i_end)
        {
            if (Length(m_End - m_Start) < TOLERANCE)
                throw std::runtime_error("Degenerate line");
        }

        // =========================
        // Basic getters
        // =========================
        Point2D<T> GetStart() const { return m_Start; }
        Point2D<T> GetEnd() const { return m_End; }

        T Length() const
        {
            return Line2D::Length(m_End - m_Start);
        }

        Point2D<T> Direction() const
        {
            return Normalize(m_End - m_Start);
        }

        // =========================
        // Tangents (your API semantics)
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

        double ParameterOfPoint(const Point2D<T>& i_pt) const
        {
            Point2D<T> d = m_End - m_Start;
            Point2D<T> v = i_pt - m_Start;

            return Dot(v, d) / Dot(d, d);
        }

        // =========================
        // Projection
        // =========================
        Point2D<T> ProjectPoint(const Point2D<T>& i_pt) const
        {
            double t = ParameterOfPoint(i_pt);
            return PointAtParameter(t);
        }

        // =========================
        // Extend
        // =========================
        Line2D ExtendAtStart(T dist) const
        {
            Point2D<T> dir = Normalize(m_Start - m_End);
            return Line2D(
                Point2D<T>(
                    m_Start.GetX() + dir.GetX() * dist,
                    m_Start.GetY() + dir.GetY() * dist
                ),
                m_End
            );
        }

        Line2D ExtendAtEnd(T dist) const
        {
            Point2D<T> dir = Normalize(m_End - m_Start);
            return Line2D(
                m_Start,
                Point2D<T>(
                    m_End.GetX() + dir.GetX() * dist,
                    m_End.GetY() + dir.GetY() * dist
                )
            );
        }

        // =========================
        // Midpoint
        // =========================
        Point2D<T> MidPoint() const
        {
            return Point2D<T>(
                (m_Start.GetX() + m_End.GetX()) / 2,
                (m_Start.GetY() + m_End.GetY()) / 2
            );
        }

        // =========================
        // Angle between lines (radians)
        // =========================
        double AngleWith(const Line2D& other) const
        {
            Point2D<T> d1 = Direction();
            Point2D<T> d2 = other.Direction();

            double dot = Dot(d1, d2);
            dot = std::max(-1.0, std::min(1.0, dot));

            return std::acos(dot);
        }

        // =========================
        // Distance between parallel lines
        // =========================
        double ParallelDistance(const Line2D& other) const
        {
            Point2D<T> d = Direction();
            Point2D<T> diff = other.m_Start - m_Start;

            return std::fabs(Cross(diff, d));
        }

        // =========================
        // Rotation helpers
        // =========================
        static Point2D<T> RotatePoint(
            const Point2D<T>& pt,
            const Point2D<T>& origin,
            double angleRad)
        {
            double s = std::sin(angleRad);
            double c = std::cos(angleRad);

            double x = pt.GetX() - origin.GetX();
            double y = pt.GetY() - origin.GetY();

            return Point2D<T>(
                origin.GetX() + x * c - y * s,
                origin.GetY() + x * s + y * c
            );
        }

        Line2D RotateFromStartCCW(double angleRad) const
        {
            return Line2D(
                m_Start,
                RotatePoint(m_End, m_Start, angleRad)
            );
        }

        Line2D RotateFromStartCW(double angleRad) const
        {
            return Line2D(
                m_Start,
                RotatePoint(m_End, m_Start, -angleRad)
            );
        }

        Line2D RotateAbout(const Point2D<T>& origin, double angleRad) const
        {
            return Line2D(
                RotatePoint(m_Start, origin, angleRad),
                RotatePoint(m_End, origin, angleRad)
            );
        }

        // =========================
        // Intersection
        // =========================
        std::optional<Point2D<T>> Intersection(const Line2D& other) const
        {
            Point2D<T> r = m_End - m_Start;
            Point2D<T> s = other.m_End - other.m_Start;

            double denom = Cross(r, s);

            if (std::fabs(denom) < TOLERANCE)
                return std::nullopt; // Parallel

            Point2D<T> diff = other.m_Start - m_Start;

            double t = Cross(diff, s) / denom;
            double u = Cross(diff, r) / denom;

            if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
            {
                return PointAtParameter(t);
            }

            return std::nullopt;
        }

        bool IsIntersecting(const Line2D& other) const
        {
            return Intersection(other).has_value();
        }
    };
}
