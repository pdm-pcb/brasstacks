#include "brasstacks/math/Vec4.hpp"
#include "brasstacks/math/math.hpp"

namespace btx::math {

// =============================================================================
Vec4::Vec4(float const x_value, float const y_value, float const z_value,
           float const w_value) :
    x { x_value }, y { y_value }, z { z_value }, w { w_value }
{ }

// =============================================================================
Vec4::Vec4(Vec3 const &v, float const w_value) :
    x { v.x }, y { v.y }, z { v.z }, w { w_value }
{ }

// =============================================================================
bool Vec4::operator==(const Vec4 &other) const {
    // If the absolute value of the difference between this.x and other.x is
    // less than the chosen float epsilon, then the x components of the two
    // vectors are the same. If all members hit this criterion, the vectors are
    // equal.

    return (
        std::abs(x - other.x) < epsilon &&
        std::abs(y - other.y) < epsilon &&
        std::abs(z - other.z) < epsilon &&
        std::abs(w - other.w) < epsilon
    );
}

Vec4 Vec4::operator+(Vec4 const &other) const {
    return { x + other.x, y + other.y, z + other.z, w + other.w };
}

Vec4 Vec4::operator-(Vec4 const &other) const {
    return { x - other.x, y - other.y, z - other.z, w - other.w };
}

Vec4 Vec4::operator-() const {
    return { -x, -y, -z, -w };
}

Vec4 & Vec4::operator+=(Vec4 const &other) {
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;

    return *this;
}

Vec4 & Vec4::operator-=(Vec4 const &other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;

    return *this;
}

Vec4 & Vec4::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    w *= scalar;

    return *this;
}

Vec4 & Vec4::operator/=(float scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;
    w /= scalar;

    return *this;
}

// =============================================================================
Vec4 operator*(float scalar, Vec4 const &v) {
    return { v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar };
}

Vec4 operator*(Vec4 const &v, float scalar) {
    return { v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar };
}

Vec4 operator/(Vec4 const &v, float scalar) {
    return { v.x / scalar, v.y / scalar, v.z / scalar, v.w / scalar };
}

// =============================================================================
std::ostream & operator<<(std::ostream& out, Vec4 const& v) {
    out << std::fixed << std::setprecision(print_precs)
        << std::setw(print_width) << v.x << " "
        << std::setw(print_width) << v.y << " "
        << std::setw(print_width) << v.z;

    return out;
}

} // namespace btx::math