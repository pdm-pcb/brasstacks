#ifndef BRASSTACKS_MATH_VEC3_HPP
#define BRASSTACKS_MATH_VEC3_HPP

#include "brasstacks/pch.hpp"

namespace btx::math {

struct Vec3 {
    static Vec3 const zero;
    static Vec3 const unit_x;
    static Vec3 const unit_y;
    static Vec3 const unit_z;

// =============================================================================
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

// =============================================================================
    [[nodiscard]] bool operator==(Vec3 const &other) const;

    [[nodiscard]] Vec3 operator+(Vec3 const &other) const;
    [[nodiscard]] Vec3 operator-(Vec3 const &other) const;

    [[nodiscard]] Vec3 operator-() const;

    Vec3 & operator+=(Vec3 const &other);
    Vec3 & operator-=(Vec3 const &other);

    Vec3 & operator*=(float scalar);
    Vec3 & operator/=(float scalar);

// =============================================================================
    Vec3() = default;
    ~Vec3() = default;

    Vec3(float x, float y, float z);

    Vec3(Vec3 &&) = default;
    Vec3(Vec3 const &) = default;

    Vec3 & operator=(Vec3 &&) = default;
    Vec3 & operator=(Vec3 const &) = default;
};

[[nodiscard]] Vec3 operator*(float scalar, Vec3 const &v);
[[nodiscard]] Vec3 operator*(Vec3 const &v, float scalar);
[[nodiscard]] Vec3 operator/(Vec3 const &v, float scalar);

std::ostream & operator<<(std::ostream& out, Vec3 const& v);

} // namespace btx::math

#endif // BRASSTACKS_MATH_VEC3_HPP