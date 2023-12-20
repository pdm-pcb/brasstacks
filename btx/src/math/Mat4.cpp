#include "brasstacks/math/Mat4.hpp"
#include "brasstacks/math/math.hpp"

namespace btx::math {

// =============================================================================
Mat4::Mat4(Vec4 x, Vec4 y, Vec4 z, Vec4 w) :
    x { x }, y { y }, z { z }, w { w }
{ }

// =============================================================================
Mat4::Mat4(float x_x, float x_y, float x_z, float x_w,
           float y_x, float y_y, float y_z, float y_w,
           float z_x, float z_y, float z_z, float z_w,
           float w_x, float w_y, float w_z, float w_w) :
    x { x_x, x_y, x_z, x_w },
    y { y_x, y_y, y_z, y_w },
    z { z_x, z_y, z_z, z_w },
    w { w_x, w_y, w_z, w_w }
{ }

// =============================================================================
bool Mat4::operator==(Mat4 const &other) const {
    return x == other.x &&
           y == other.y &&
           z == other.z &&
           w == other.w;
}

Mat4 Mat4::operator+(Mat4 const &other) const {
    return {
        x + other.x,
        y + other.y,
        z + other.z,
        w + other.w
    };
}

Mat4 Mat4::operator-(Mat4 const &other) const {
    return {
        x - other.x,
        y - other.y,
        z - other.z,
        w - other.w
    };
}

Mat4 Mat4::operator-() const {
    return { -x, -y, -z, -w };
}

Mat4 & Mat4::operator+=(Mat4 const &other) {
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;

    return *this;
}

Mat4 & Mat4::operator-=(Mat4 const &other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;

    return *this;
}

Mat4 & Mat4::operator*=(Mat4 const &other) {
    auto const Ax = x;
    auto const Ay = y;
    auto const Az = z;
    auto const Aw = w;

    auto const Bx = other.x;
    auto const By = other.y;
    auto const Bz = other.z;
    auto const Bw = other.w;

    x = (Ax * Bx.x) + (Ay * Bx.y) + (Az * Bx.z) + (Aw * Bx.w);
    y = (Ax * By.x) + (Ay * By.y) + (Az * By.z) + (Aw * By.w);
    z = (Ax * Bz.x) + (Ay * Bz.y) + (Az * Bz.z) + (Aw * Bz.w);
    w = (Ax * Bw.x) + (Ay * Bw.y) + (Az * Bw.z) + (Aw * Bw.w);

    return *this;
}

Mat4 Mat4::operator*(Mat4 const &other) const {
    auto result = *this;
    result *= other;
    return result;
}

Vec4 Mat4::operator*(Vec4 const &v) const {
    return {
        (x.x * v.x) + (x.y * v.y) + (x.z * v.z) + (x.w * v.w),
        (y.x * v.x) + (y.y * v.y) + (y.z * v.z) + (y.w * v.w),
        (z.x * v.x) + (z.y * v.y) + (z.z * v.z) + (z.w * v.w),
        (w.x * v.x) + (w.y * v.y) + (w.z * v.z) + (w.w * v.w),
    };
}

} // namespace btx::math