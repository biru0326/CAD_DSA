#pragma once

namespace Primitives
{
    template<typename T>
    class Rect
    {
    private:
        T m_xMin;
        T m_yMin;
        T m_xMax;
        T m_yMax;

    public:
        Rect() : m_xMin(0), m_yMin(0), m_xMax(0), m_yMax(0) {}

        Rect(const T& i_xMin, const T& i_yMin, const T& i_xMax, const T& i_yMax)
            : m_xMin(i_xMin), m_yMin(i_yMin), m_xMax(i_xMax), m_yMax(i_yMax) {}

        void Change(const T& i_xMin, const T& i_yMin, const T& i_xMax, const T& i_yMax)
        {
            m_xMin = i_xMin;
            m_yMin = i_yMin;
            m_xMax = i_xMax;
            m_yMax = i_yMax;
        }

        T getXMin() const { return m_xMin; }
        T getYMin() const { return m_yMin; }
        T getXMax() const { return m_xMax; }
        T getYMax() const { return m_yMax; }

        bool overlaps(const Rect<T>& other) const
        {
            // No overlap cases
            if (m_xMax < other.m_xMin || other.m_xMax < m_xMin)
                return false;
    
            if (m_yMax < other.m_yMin || other.m_yMax < m_yMin)
                return false;
    
            return true;
        }
    };
}
