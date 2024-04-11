#include "brasstacks/math/math.hpp"

namespace btx::math {

// "Namespace-global" statics
Vec2 const Vec2::zero   { 0.0f, 0.0f };
Vec2 const Vec2::unit_x { 1.0f, 0.0f };
Vec2 const Vec2::unit_y { 0.0f, 1.0f };

Vec3 const Vec3::zero   { 0.0f, 0.0f, 0.0f };
Vec3 const Vec3::unit_x { 1.0f, 0.0f, 0.0f };
Vec3 const Vec3::unit_y { 0.0f, 1.0f, 0.0f };
Vec3 const Vec3::unit_z { 0.0f, 0.0f, 1.0f };

Vec4 const Vec4::zero   { 0.0f, 0.0f, 0.0f, 0.0f };
Vec4 const Vec4::unit_x { 1.0f, 0.0f, 0.0f, 0.0f };
Vec4 const Vec4::unit_y { 0.0f, 1.0f, 0.0f, 0.0f };
Vec4 const Vec4::unit_z { 0.0f, 0.0f, 1.0f, 0.0f };
Vec4 const Vec4::unit_w { 0.0f, 0.0f, 0.0f, 1.0f };

Mat4 const Mat4::zero { Vec4::zero, Vec4::zero, Vec4::zero, Vec4::zero };
Mat4 const Mat4::identity {
    Vec4::unit_x,
    Vec4::unit_y,
    Vec4::unit_z,
    Vec4::unit_w
};

// =============================================================================
// Normalizing
Vec2 normalize(Vec2 const &v) {
    Vec2 result = v;

    auto const length = length_squared(v);

    // If we're working with the zero vector or a vector that's already
    // normalized, just return
    if(length < epsilon || std::abs(length - 1.0f) < epsilon) {
        return result;
    }

    float const length_recip = 1.0f / std::sqrt(length);

    result.x *= length_recip;
    result.y *= length_recip;

    return result;
}

Vec3 normalize(Vec3 const &v) {
    Vec3 result = v;

    auto const length = length_squared(v);

    // If we're working with the zero vector or a vector that's already
    // normalized, just return
    if(length < epsilon || std::abs(length - 1.0f) < epsilon) {
        return result;
    }

    float const length_recip = 1.0f / std::sqrt(length);

    result.x *= length_recip;
    result.y *= length_recip;
    result.z *= length_recip;

    return result;
}

Vec4 normalize(Vec4 const &v) {
    Vec4 result = v;

    auto const length = length_squared(v);

    // If we're working with the zero vector or a vector that's already
    // normalized, just return
    if(length < epsilon || std::abs(length - 1.0f) < epsilon) {
        return result;
    }

    float const length_recip = 1.0f / std::sqrt(length);

    result.x *= length_recip;
    result.y *= length_recip;
    result.z *= length_recip;
    result.w *= length_recip;

    return result;
}

// =============================================================================
// Dot Product
float dot(Vec2 const &a, Vec2 const &b) {
    return (a.x * b.x) + (a.y * b.y);
}

float dot(Vec3 const &a, Vec3 const &b) {
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

float dot(Vec4 const &a, Vec4 const &b) {
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

// =============================================================================
// Cross Product
Vec3 cross(Vec3 const &a, Vec3 const &b) {
    return Vec3 {
        (a.y * b.z) - (a.z * b.y),
        (a.z * b.x) - (a.x * b.z),
        (a.x * b.y) - (a.y * b.x),
    };
}

// =============================================================================
// Matrix operations
Mat4 transpose(Mat4 const &m) {
    Mat4 result { m.x, m.y, m.z, m.w, };

    std::swap(result.x.y, result.y.x);
    std::swap(result.x.z, result.z.x);
    std::swap(result.x.w, result.w.x);

    std::swap(result.y.z, result.z.y);
    std::swap(result.y.w, result.w.y);

    std::swap(result.z.w, result.w.z);

    return result;
}

Mat4 translate(Mat4 const &m, Vec3 const &v) {
    Mat4 result { m.x, m.y, m.z, m.w, };

    result.w = (m.x * v.x) + (m.y * v.y) + (m.z * v.z) + m.w;
    return result;
}

Mat4 rotate(Mat4 const &m, float const angle_degrees, Vec3 const &axis) {
    Mat4 result { m.x, m.y, m.z, m.w, };

    float const theta = radians(angle_degrees);
    float const cos_theta = std::cos(theta);
    float const sin_theta = std::sin(theta);

    auto const rot_axis = normalize(axis);
    auto const temp = (1.0f - cos_theta) * rot_axis;

    auto rot_mat = Mat4::identity;
    rot_mat.x.x = cos_theta + temp.x * rot_axis.x;
    rot_mat.x.y = temp.x * rot_axis.y + sin_theta * rot_axis.z;
    rot_mat.x.z = temp.x * rot_axis.z - sin_theta * rot_axis.y;

    rot_mat.y.x = temp.y * rot_axis.x - sin_theta * rot_axis.z;
    rot_mat.y.y = cos_theta + temp.y * rot_axis.y;
    rot_mat.y.z = temp.y * rot_axis.z + sin_theta * rot_axis.x;

    rot_mat.z.x = temp.z * rot_axis.x + sin_theta * rot_axis.y;
    rot_mat.z.y = temp.z * rot_axis.y - sin_theta * rot_axis.x;
    rot_mat.z.z = cos_theta + temp.z * rot_axis.z;

    result.x = m.x * rot_mat.x.x + m.y * rot_mat.x.y + m.z * rot_mat.x.z;
    result.y = m.x * rot_mat.y.x + m.y * rot_mat.y.y + m.z * rot_mat.y.z;
    result.z = m.x * rot_mat.z.x + m.y * rot_mat.z.y + m.z * rot_mat.z.z;
    result.w = m.w;

    return result;
}

Mat4 scale(Mat4 const &m, float const &pct) {
    return Mat4 { m.x * pct, m.y * pct, m.z * pct, m.w, };
}

// =============================================================================
// Camera math
Mat4 orient_view_rh(Vec3 const &position, Vec3 const &forward,
                    Vec3 const &side, Vec3 const &up)
{
    auto result = Mat4::identity;

    result.x.x = side.x;
    result.y.x = side.y;
    result.z.x = side.z;
    result.x.y = up.x;
    result.y.y = up.y;
    result.z.y = up.z;
    result.x.z = -forward.x;
    result.y.z = -forward.y;
    result.z.z = -forward.z;
    result.w.x = -math::dot(side, position);
    result.w.y = -math::dot(up, position);
    result.w.z =  math::dot(forward, position);

    return result;
}

Mat4 ortho_proj_rh_zo(float const left, float const right,
                      float const bottom, float const top,
                      float const near_plane, float const far_plane)
{
    auto result = Mat4::identity;

    auto const a = right - left;
    auto const b = top - bottom;
    auto const c = far_plane - near_plane;

    result.x.x = 2.0f / a;
    result.y.y = -2.0f / b;
    result.z.z = -1.0f / c;
    result.w.x = -(right + left) / a;
    result.w.y = -(top + bottom) / b;
    result.w.z = -near_plane / c;

    return result;
}

Mat4 persp_proj_rh_no(float const vfov_degrees, float const aspect_ratio,
                      float const near_plane, float const far_plane)
{
    auto result = Mat4::zero;

    float const fov_rad = math::radians(vfov_degrees);
    float const half_angle = std::tan(fov_rad * 0.5f);

    result.x.x = 1.0f / (aspect_ratio * half_angle);
    result.y.y = 1.0f / half_angle;
    result.z.z = -(near_plane + far_plane) / (far_plane - near_plane);
    result.z.w = -1.0f;
    result.w.z = -(2.0f * far_plane * near_plane) / (far_plane - near_plane);

    return result;
}

Mat4 persp_proj_rh_zo(float const vfov_degrees, float const aspect_ratio,
                      float const near_plane, float const far_plane)
{
    auto result = Mat4::zero;

    float const fov_rad = math::radians(vfov_degrees);
    float const half_angle = std::tan(fov_rad * 0.5f);

    result.x.x = 1.0f / (aspect_ratio * half_angle);
    result.y.y = 1.0f / (half_angle);
    result.z.z = far_plane / (near_plane - far_plane);
    result.z.w = -1.0f;
    result.w.z = -(far_plane * near_plane) / (far_plane - near_plane);

    return result;
}

Mat4 persp_proj_rh_zo_inf(float const vfov_degrees, float const aspect_ratio,
                          float const near_plane)
{
    auto result = Mat4::zero;

    auto const fov_rad = math::radians(vfov_degrees);
    auto const half_angle = std::tan(fov_rad * 0.5f);
    auto const half_angle_recip = 1.0f / half_angle;

    result.x.x = half_angle_recip;
    result.y.y = half_angle_recip / aspect_ratio;
    result.z.z = -1.0f;
    result.z.w = -1.0f;
    result.w.z = -2.0f * near_plane;

    return result;
}

Mat4 persp_proj_rh_oz_inf(float const vfov_degrees, float const aspect_ratio,
                          float const near_plane)
{
    auto result = Mat4::zero;

    auto const fov_rad = math::radians(vfov_degrees);
    auto const half_angle = std::tan(fov_rad * 0.5f);
    auto const half_angle_recip = 1.0f / half_angle;

    result.x.x = half_angle_recip / aspect_ratio;
    result.y.y = -half_angle_recip;
    result.z.w = -1.0f;
    result.w.z = near_plane;

    return result;
}

} // namespace btx::math