#pragma once
#include "Constants.h"

namespace Primitives
{
    template<typename T>
    class Point2D
    {
    private:
        T m_X;
        T m_Y;

    public:
        Point2D() : m_X(0), m_Y(0) {}
        Point2D(const T& i_x, const T& i_y) : m_X(i_x), m_Y(i_y) {}

        bool operator==(const Point2D<int>& i_point) const
        {
            return (m_X == i_point.m_X && m_Y == i_point.m_Y);
        }
        bool operator==(const Point2D<double>& i_point) const
        {
            return (std::fabs(m_X - i_point.m_X) < TOLERANCE &&
                std::fabs(m_Y - i_point.m_Y) < TOLERANCE);
        }

        Point2D operator+(const Point2D& i_point) const
        {
            return Point2D(m_X + i_point.m_X, m_Y + i_point.m_Y);
        }

        Point2D operator-(const Point2D& i_point) const
        {
            return Point2D(m_X - i_point.m_X, m_Y - i_point.m_Y);
        }

        T GetX() const { return m_X; }
        T GetY() const { return m_Y; }

        void Change(const T& i_x, const T& i_y)
        {
            m_X = i_x;
            m_Y = i_y;
        }
    };
}
