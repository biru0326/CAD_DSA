#pragma once

#include <cmath>
#include <vector>
#include <optional>
#include <algorithm>
#include <stdexcept>

#include "Point2D.h"
#include "Line2D.h"
#include "Arc2D.h"
#include "Rect.h"
#include "Constants.h"

namespace Primitives
{
template<typename T>
class Intersector2D
{
private:
    static T Dot(const Point2D<T>& a, const Point2D<T>& b)
    {
        return a.GetX()*b.GetX() + a.GetY()*b.GetY();
    }

    static T Length(const Point2D<T>& v)
    {
        return std::sqrt(v.GetX()*v.GetX() + v.GetY()*v.GetY());
    }

public:

    // =========================
    // Line - Arc
    // =========================
    static std::vector<Point2D<T>> Intersect(
        const Line2D<T>& line,
        const Arc2D<T>& arc)
    {
        std::vector<Point2D<T>> result;

        const Point2D<T> p0 = line.GetStart();
        const Point2D<T> d  = line.GetEnd() - line.GetStart();

        const Point2D<T> c = arc.GetCenter();
        const T r = arc.GetRadius();

        const Point2D<T> f = p0 - c;

        const double a = Dot(d, d);
        const double b = 2.0 * Dot(f, d);
        const double cval = Dot(f, f) - r*r;

        double disc = b*b - 4*a*cval;

        if (disc < -TOLERANCE)
            return result;

        disc = std::max(0.0, disc);
        const double sqrtDisc = std::sqrt(disc);

        const double t1 = (-b - sqrtDisc) / (2*a);
        const double t2 = (-b + sqrtDisc) / (2*a);

        auto check = [&](double t)
        {
            if (t >= -TOLERANCE && t <= 1 + TOLERANCE)
            {
                Point2D<T> pt = line.PointAtParameter(t);
                if (arc.IsPointOnArc(pt))
                    result.push_back(pt);
            }
        };

        check(t1);
        if (disc > TOLERANCE) check(t2);

        return result;
    }

    // =========================
    // Arc - Line
    // =========================
    static std::vector<Point2D<T>> Intersect(
        const Arc2D<T>& arc,
        const Line2D<T>& line)
    {
        return Intersect(line, arc);
    }

    // =========================
    // Arc - Arc
    // =========================
    static std::vector<Point2D<T>> Intersect(
        const Arc2D<T>& a1,
        const Arc2D<T>& a2)
    {
        std::vector<Point2D<T>> result;

        const Point2D<T> c1 = a1.GetCenter();
        const Point2D<T> c2 = a2.GetCenter();

        const T r1 = a1.GetRadius();
        const T r2 = a2.GetRadius();

        const Point2D<T> diff = c2 - c1;
        const double d = Length(diff);

        // Early rejection
        if (d > r1 + r2 + TOLERANCE) return result;
        if (d < std::fabs(r1 - r2) - TOLERANCE) return result;
        if (d < TOLERANCE) return result; // coincident centers

        const double a = (r1*r1 - r2*r2 + d*d) / (2*d);
        double h_sq = r1*r1 - a*a;

        if (h_sq < -TOLERANCE) return result;

        h_sq = std::max(0.0, h_sq);
        const double h = std::sqrt(h_sq);

        const Point2D<T> dir(diff.GetX()/d, diff.GetY()/d);

        const Point2D<T> p(
            c1.GetX() + dir.GetX()*a,
            c1.GetY() + dir.GetY()*a
        );

        const Point2D<T> perp(-dir.GetY(), dir.GetX());

        auto check = [&](const Point2D<T>& pt)
        {
            if (a1.IsPointOnArc(pt) && a2.IsPointOnArc(pt))
                result.push_back(pt);
        };

        const Point2D<T> i1 = p + perp * h;
        const Point2D<T> i2 = p - perp * h;

        check(i1);
        if (h > TOLERANCE) check(i2);

        return result;
    }
};
} // namespace Primitives
