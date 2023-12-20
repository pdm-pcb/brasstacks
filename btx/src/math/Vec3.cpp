#include "brasstacks/math/Vec3.hpp"
#include "brasstacks/math/math.hpp"

namespace btx::math {

// =============================================================================
Vec3::Vec3(float const x, float const y, float const z) :
    x { x }, y { y }, z { z }
{ }

// =============================================================================
bool Vec3::operator==(const Vec3 &other) const {
    // If the absolute value of the difference between this.x and other.x is
    // less than the chosen float epsilon, then the x components of the two
    // vectors are the same. If all members hit this criterion, the vectors are
    // equal.

    return (
        std::abs(x - other.x) < epsilon &&
        std::abs(y - other.y) < epsilon &&
        std::abs(z - other.z) < epsilon
    );
}

Vec3 Vec3::operator+(Vec3 const &other) const {
    return { x + other.x, y + other.y, z + other.z };
}

Vec3 Vec3::operator-(Vec3 const &other) const {
    return { x - other.x, y - other.y, z - other.z };
}

Vec3 Vec3::operator-() const {
    return { -x, -y, -z };
}

Vec3 & Vec3::operator+=(Vec3 const &other) {
    x += other.x;
    y += other.y;
    z += other.z;

    return *this;
}

Vec3 & Vec3::operator-=(Vec3 const &other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;

    return *this;
}

Vec3 & Vec3::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;

    return *this;
}

Vec3 & Vec3::operator/=(float scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;

    return *this;
}

// =============================================================================
Vec3 operator*(float scalar, Vec3 const &v) {
    return { v.x * scalar, v.y * scalar, v.z * scalar };
}

Vec3 operator*(Vec3 const &v, float scalar) {
    return { v.x * scalar, v.y * scalar, v.z * scalar };
}

Vec3 operator/(Vec3 const &v, float scalar) {
    return { v.x / scalar, v.y / scalar, v.z / scalar };
}

// =============================================================================
std::ostream & operator<<(std::ostream& out, Vec3 const& v) {
    out << std::fixed << std::setprecision(print_precs)
        << std::setw(print_width) << v.x << " "
        << std::setw(print_width) << v.y << " "
        << std::setw(print_width) << v.z;

    return out;
}

} // namespace btx::math