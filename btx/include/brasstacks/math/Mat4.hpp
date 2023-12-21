#ifndef BRASSTACKS_MATH_MAT4_HPP
#define BRASSTACKS_MATH_MAT4_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/math/Vec4.hpp"

namespace btx::math {

struct Mat4 final {
    static Mat4 const zero;
    static Mat4 const identity;

// =============================================================================
    Vec4 x = Vec4::zero;
    Vec4 y = Vec4::zero;
    Vec4 z = Vec4::zero;
    Vec4 w = Vec4::zero;

// =============================================================================
    [[nodiscard]] bool operator==(Mat4 const &other) const;

    [[nodiscard]] Mat4 operator+(Mat4 const &other) const;
    [[nodiscard]] Mat4 operator-(Mat4 const &other) const;

    [[nodiscard]] Mat4 operator-() const;

    Mat4 & operator+=(Mat4 const &other);
    Mat4 & operator-=(Mat4 const &other);

    Mat4 & operator*=(Mat4 const &other);

    Mat4 operator*(Mat4 const &other) const;

    Vec4 operator*(Vec4 const &v) const;

// =============================================================================
    Mat4() = default;
    ~Mat4() = default;

    Mat4(Vec4 vx, Vec4 vy, Vec4 vz, Vec4 vw);

    Mat4(float x_x, float x_y, float x_z, float x_w,
         float y_x, float y_y, float y_z, float y_w,
         float z_x, float z_y, float z_z, float z_w,
         float w_x, float w_y, float w_z, float w_w);

    Mat4(Mat4 &&) = default;
    Mat4(Mat4 const &) = default;

    Mat4 & operator=(Mat4 &&) = default;
    Mat4 & operator=(Mat4 const &) = default;
};

} // namespace btx::math

#endif // BRASSTACKS_MATH_MAT4_HPP