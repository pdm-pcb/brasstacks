#ifndef BRASSTACKS_MATH_VEC4_HPP
#define BRASSTACKS_MATH_VEC4_HPP

#include "brasstacks/pch.hpp"

namespace btx::math {

struct Vec4 {
    static Vec4 const zero;
    static Vec4 const unit_x;
    static Vec4 const unit_y;
    static Vec4 const unit_z;
    static Vec4 const unit_w;

// =============================================================================
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;

// =============================================================================
    [[nodiscard]] bool operator==(Vec4 const &other) const;

    [[nodiscard]] Vec4 operator+(Vec4 const &other) const;
    [[nodiscard]] Vec4 operator-(Vec4 const &other) const;

    [[nodiscard]] Vec4 operator-() const;

    Vec4 & operator+=(Vec4 const &other);
    Vec4 & operator-=(Vec4 const &other);

    Vec4 & operator*=(float scalar);
    Vec4 & operator/=(float scalar);

// =============================================================================
    Vec4() = default;
    ~Vec4() = default;

    Vec4(float x, float y, float z, float w);

    Vec4(Vec4 &&) = default;
    Vec4(Vec4 const &) = default;

    Vec4 & operator=(Vec4 &&) = default;
    Vec4 & operator=(Vec4 const &) = default;
};

[[nodiscard]] Vec4 operator*(float scalar, Vec4 const &v);
[[nodiscard]] Vec4 operator*(Vec4 const &v, float scalar);
[[nodiscard]] Vec4 operator/(Vec4 const &v, float scalar);

std::ostream & operator<<(std::ostream& out, Vec4 const& v);

} // namespace btx::math

#endif // BRASSTACKS_MATH_VEC4_HPP