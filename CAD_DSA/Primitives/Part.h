#pragma once
#include "Loop.h"
#include <vector>
#include <memory>

namespace Primitives
{
    template<typename T>
    class Part
    {
        using sharedLoop = std::shared_ptr<Loop<T>>;
        using constSharedLoop = std::shared_ptr<const Loop<T>>;
    public:
        Part(sharedLoop& i_OuterLoop,
            Point2D<int> i_OuterLoopPixelCell,
            std::vector<sharedLoop>& i_InnerLoops) :
            m_OuterLoop(std::move(i_OuterLoop)), 
            m_OuterLoopPixelCell(i_OuterLoopPixelCell),
            m_InnerLoops(std::move(i_InnerLoops))
        {
        }

        Part(Part&& obj) :
            m_OuterLoop(std::move(obj.m_OuterLoop)),
            m_InnerLoops(std::move(obj.m_InnerLoops))
        {
        }

        Part& operator=(Part&& obj)
        {
            if (this != &obj)
            {
                m_OuterLoop = std::move(obj.m_OuterLoop);
                m_InnerLoops = std::move(obj.m_InnerLoops);
            }
            return *this;
        }

        Part(const Part&) = delete;
        Part& operator=(const Part&) = delete;

    private:
        sharedLoop m_OuterLoop;
        std::vector<sharedLoop> m_InnerLoops;
        Point2D<int> m_OuterLoopPixelCell;

    public:

        const sharedLoop GetLoop(const Point2D<T>& i_point, bool& isInside)const
        {
            //This can be further optimized by checking boundingrect of each loop and point.
            //If they overlap then only check IsPointOnOrInsidePolygon


            for (const auto& innerLoop : m_InnerLoops)
            {
                if (innerLoop->IsPointOnOrInsidePolygon(i_point, isInside))
                {
                    return innerLoop;
                }
            }

            if (m_OuterLoop->IsPointOnOrInsidePolygon(i_point, isInside))
            {
                return m_OuterLoop;
            }
            else
            {
                return nullptr;
            }
        }

        int Triangulate()
        {
            return m_OuterLoop->Triangulate();
        }

        T GetWidth()const
        {
            return m_OuterLoop->GetWidth();
        }

        T GetHeight()const
        {
            return m_OuterLoop->GetHeight();
        }

        char GetOuterLoopName()const
        {
            return m_OuterLoop->GetName();
        }

        Point2D<int> GetOuterLoopPixelCell()const
        {
            return m_OuterLoopPixelCell;
        }

        void GetInnerLoops(std::vector<constSharedLoop>& i_InnerLoops)
        {
            i_InnerLoops.insert(i_InnerLoops.end(), m_InnerLoops.begin(), m_InnerLoops.end());
        }
    };
}