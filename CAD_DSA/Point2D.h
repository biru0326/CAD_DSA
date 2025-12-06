#pragma once

template<typename T>
class Point2D
{
private:
    T m_X;
    T m_Y;

public:
    Point2D() : m_X(0), m_Y(0) {}
    Point2D(const T& i_x, const T& i_y) : m_X(i_x), m_Y(i_y) {}

    T getX() const { return m_X; }
    T getY() const { return m_Y; }

    void Change(const T& i_x, const T& i_y)
    { 
        m_X = i_x;
        m_Y = i_y;
    }
};
