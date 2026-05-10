#pragma once

#include <cmath>
#include <stdexcept>

#include "Point2D.h"
#include "Constants.h"

namespace Primitives
{
    template<typename T>
    class Matrix2D
    {
    private:

        // 3x3 homogeneous transformation matrix
        //
        // | m00  m01  m02 |
        // | m10  m11  m12 |
        // | m20  m21  m22 |
        //
        // For 2D affine transforms:
        //
        // | R  Tx |
        // | R  Ty |
        // | 0   1 |
        //
        // Last row is usually:
        //
        // [0 0 1]

        T m[3][3];

    public:

        // =========================
        // Constructors
        // =========================

        Matrix2D()
        {
            SetIdentity();
        }

        Matrix2D(
            T m00, T m01, T m02,
            T m10, T m11, T m12,
            T m20, T m21, T m22)
        {
            m[0][0] = m00; m[0][1] = m01; m[0][2] = m02;
            m[1][0] = m10; m[1][1] = m11; m[1][2] = m12;
            m[2][0] = m20; m[2][1] = m21; m[2][2] = m22;
        }

        // =========================
        // Identity
        // =========================

        void SetIdentity()
        {
            m[0][0] = 1; m[0][1] = 0; m[0][2] = 0;
            m[1][0] = 0; m[1][1] = 1; m[1][2] = 0;
            m[2][0] = 0; m[2][1] = 0; m[2][2] = 1;
        }

        static Matrix2D Identity()
        {
            return Matrix2D();
        }

        // =========================
        // Translation
        // =========================

        static Matrix2D Translation(T tx, T ty)
        {
            return Matrix2D(
                1, 0, tx,
                0, 1, ty,
                0, 0, 1
            );
        }

        // =========================
        // Rotation about origin
        // =========================

        static Matrix2D Rotation(double angleRad)
        {
            double c = std::cos(angleRad);
            double s = std::sin(angleRad);

            return Matrix2D(
                 c, -s, 0,
                 s,  c, 0,
                 0,  0, 1
            );
        }

        // =========================
        // Rotation about arbitrary point
        // =========================

        static Matrix2D RotationAboutPoint(
            const Point2D<T>& center,
            double angleRad)
        {
            return
                Translation(center.GetX(), center.GetY()) *
                Rotation(angleRad) *
                Translation(-center.GetX(), -center.GetY());
        }

        // =========================
        // Scaling
        // =========================

        static Matrix2D Scaling(T sx, T sy)
        {
            return Matrix2D(
                sx, 0,  0,
                0, sy,  0,
                0,  0,  1
            );
        }

        // =========================
        // Uniform scaling
        // =========================

        static Matrix2D Scaling(T s)
        {
            return Scaling(s, s);
        }

        // =========================
        // Scaling about point
        // =========================

        static Matrix2D ScalingAboutPoint(
            const Point2D<T>& center,
            T sx,
            T sy)
        {
            return
                Translation(center.GetX(), center.GetY()) *
                Scaling(sx, sy) *
                Translation(-center.GetX(), -center.GetY());
        }

        // =========================
        // Reflection
        // =========================

        static Matrix2D ReflectionX()
        {
            return Matrix2D(
                 1,  0, 0,
                 0, -1, 0,
                 0,  0, 1
            );
        }

        static Matrix2D ReflectionY()
        {
            return Matrix2D(
                -1,  0, 0,
                 0,  1, 0,
                 0,  0, 1
            );
        }

        // =========================
        // Shear
        // =========================

        static Matrix2D Shear(T shx, T shy)
        {
            return Matrix2D(
                1,   shx, 0,
                shy, 1,   0,
                0,   0,   1
            );
        }

        // =========================
        // Matrix multiplication
        // =========================

        Matrix2D operator*(const Matrix2D& other) const
        {
            Matrix2D result;

            for (int r = 0; r < 3; ++r)
            {
                for (int c = 0; c < 3; ++c)
                {
                    result.m[r][c] =
                        m[r][0] * other.m[0][c] +
                        m[r][1] * other.m[1][c] +
                        m[r][2] * other.m[2][c];
                }
            }

            return result;
        }

        // =========================
        // Transform point
        // =========================

        Point2D<T> TransformPoint(
            const Point2D<T>& pt) const
        {
            T x =
                m[0][0] * pt.GetX() +
                m[0][1] * pt.GetY() +
                m[0][2];

            T y =
                m[1][0] * pt.GetX() +
                m[1][1] * pt.GetY() +
                m[1][2];

            T w =
                m[2][0] * pt.GetX() +
                m[2][1] * pt.GetY() +
                m[2][2];

            if (std::fabs(w) < TOLERANCE)
                throw std::runtime_error("Invalid homogeneous coordinate");

            return Point2D<T>(x / w, y / w);
        }

        // =========================
        // Transform vector
        // Translation ignored
        // =========================

        Point2D<T> TransformVector(
            const Point2D<T>& v) const
        {
            return Point2D<T>(
                m[0][0] * v.GetX() +
                m[0][1] * v.GetY(),

                m[1][0] * v.GetX() +
                m[1][1] * v.GetY()
            );
        }

        // =========================
        // Determinant
        // =========================

        T Determinant() const
        {
            return
                m[0][0] * (m[1][1]*m[2][2] - m[1][2]*m[2][1]) -
                m[0][1] * (m[1][0]*m[2][2] - m[1][2]*m[2][0]) +
                m[0][2] * (m[1][0]*m[2][1] - m[1][1]*m[2][0]);
        }

        // =========================
        // Inverse
        // =========================

        Matrix2D Inverse() const
        {
            T det = Determinant();

            if (std::fabs(det) < TOLERANCE)
                throw std::runtime_error("Matrix not invertible");

            T invDet = 1 / det;

            Matrix2D r;

            r.m[0][0] =
                (m[1][1]*m[2][2] - m[1][2]*m[2][1]) * invDet;

            r.m[0][1] =
                (m[0][2]*m[2][1] - m[0][1]*m[2][2]) * invDet;

            r.m[0][2] =
                (m[0][1]*m[1][2] - m[0][2]*m[1][1]) * invDet;

            r.m[1][0] =
                (m[1][2]*m[2][0] - m[1][0]*m[2][2]) * invDet;

            r.m[1][1] =
                (m[0][0]*m[2][2] - m[0][2]*m[2][0]) * invDet;

            r.m[1][2] =
                (m[0][2]*m[1][0] - m[0][0]*m[1][2]) * invDet;

            r.m[2][0] =
                (m[1][0]*m[2][1] - m[1][1]*m[2][0]) * invDet;

            r.m[2][1] =
                (m[0][1]*m[2][0] - m[0][0]*m[2][1]) * invDet;

            r.m[2][2] =
                (m[0][0]*m[1][1] - m[0][1]*m[1][0]) * invDet;

            return r;
        }

        // =========================
        // Access
        // =========================

        T Get(int row, int col) const
        {
            return m[row][col];
        }
    };
}
