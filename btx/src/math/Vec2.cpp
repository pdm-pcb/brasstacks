#include "brasstacks/math/Vec2.hpp"
#include "brasstacks/math/math.hpp"

namespace btx::math {

// =============================================================================
bool Vec2::operator==(const Vec2 &other) const {
    // If the absolute value of the difference between this.x and other.x is
    // less than the chosen float epsilon, then the x components of the two
    // vectors are the same. If all members hit this criterion, the vectors are
    // equal.

    return (
        std::abs(x - other.x) < epsilon &&
        std::abs(y - other.y) < epsilon
    );
}

Vec2 Vec2::operator+(Vec2 const &other) const {
    return { x + other.x, y + other.y };
}

Vec2 Vec2::operator-(Vec2 const &other) const {
    return { x - other.x, y - other.y };
}

Vec2 Vec2::operator-() const {
    return { -x, -y };
}

Vec2 & Vec2::operator+=(Vec2 const &other) {
    x += other.x;
    y += other.y;

    return *this;
}

Vec2 & Vec2::operator-=(Vec2 const &other) {
    x -= other.x;
    y -= other.y;

    return *this;
}

Vec2 & Vec2::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;

    return *this;
}

Vec2 & Vec2::operator/=(float scalar) {
    x /= scalar;
    y /= scalar;

    return *this;
}

// =============================================================================
Vec2 operator*(float scalar, Vec2 const &v) {
    return { v.x * scalar, v.y * scalar };
}

Vec2 operator*(Vec2 const &v, float scalar) {
    return { v.x * scalar, v.y * scalar };
}

Vec2 operator/(Vec2 const &v, float scalar) {
    return { v.x / scalar, v.y / scalar };
}

// =============================================================================
Vec2::Vec2(float const x_value, float const y_value) :
    x { x_value },
    y { y_value }
{ }

// =============================================================================
std::ostream & operator<<(std::ostream& out, Vec2 const& v) {
    out << std::fixed << std::setprecision(print_precs)
        << std::setw(print_width) << v.x << " "
        << std::setw(print_width) << v.y;

    return out;
}

} // namespace btx::math