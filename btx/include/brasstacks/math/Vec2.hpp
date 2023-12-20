#ifndef BRASSTACKS_MATH_VEC2_HPP
#define BRASSTACKS_MATH_VEC2_HPP

#include "brasstacks/pch.hpp"

namespace btx::math {

struct Vec2 {
    static Vec2 const zero;
    static Vec2 const unit_x;
    static Vec2 const unit_y;

// =============================================================================
    float x = 0.0f;
    float y = 0.0f;

// =============================================================================
    [[nodiscard]] bool operator==(Vec2 const &other) const;

    [[nodiscard]] Vec2 operator+(Vec2 const &other) const;
    [[nodiscard]] Vec2 operator-(Vec2 const &other) const;

    [[nodiscard]] Vec2 operator-() const;

    Vec2 & operator+=(Vec2 const &other);
    Vec2 & operator-=(Vec2 const &other);

    Vec2 & operator*=(float scalar);
    Vec2 & operator/=(float scalar);

// =============================================================================
    Vec2() = default;
    ~Vec2() = default;

    Vec2(float const x_value, float const y_value);

    Vec2(Vec2 &&) = default;
    Vec2(Vec2 const &) = default;

    Vec2 & operator=(Vec2 &&) = default;
    Vec2 & operator=(Vec2 const &) = default;
};

[[nodiscard]] Vec2 operator*(float scalar, Vec2 const &v);
[[nodiscard]] Vec2 operator*(Vec2 const &v, float scalar);
[[nodiscard]] Vec2 operator/(Vec2 const &v, float scalar);

std::ostream & operator<<(std::ostream& out, Vec2 const& v);

} // namespace btx::math

#endif // BRASSTACKS_MATH_VEC2_HPP