#ifndef BRASSTACKS_MATH_MATH_HPP
#define BRASSTACKS_MATH_MATH_HPP

#include "brasstacks/math/Vec2.hpp"
#include "brasstacks/math/Vec3.hpp"
#include "brasstacks/math/Vec4.hpp"

#include "brasstacks/math/Mat4.hpp"

namespace btx::math {

// Insist on keeping the data simple
static_assert(sizeof(Vec2) == sizeof(float) * 2);
static_assert(sizeof(Vec3) == sizeof(float) * 3);
static_assert(sizeof(Vec4) == sizeof(float) * 4);
static_assert(sizeof(Mat4) == sizeof(float) * 16);

static_assert(std::is_trivially_copyable_v<Vec2>);
static_assert(std::is_trivially_copyable_v<Vec3>);
static_assert(std::is_trivially_copyable_v<Vec4>);
static_assert(std::is_trivially_copyable_v<Mat4>);

static_assert(std::is_trivially_copy_constructible_v<Vec2>);
static_assert(std::is_trivially_copy_constructible_v<Vec3>);
static_assert(std::is_trivially_copy_constructible_v<Vec4>);
static_assert(std::is_trivially_copy_constructible_v<Mat4>);

// For pretty-printing
static uint8_t constexpr print_precs = 8u;
static uint8_t constexpr print_width = 2u * print_precs + 2u;

// General constants
static float constexpr epsilon = 1.0e-4f;
static float constexpr pi = std::numbers::pi_v<float>;
static float constexpr pi_over_one_eighty = pi / 180.0f;
static float constexpr one_eighty_over_pi = 180.0f / pi;

// Conversions
[[nodiscard]] inline float radians(float const deg) {
    return deg * pi_over_one_eighty;
}
[[nodiscard]] inline float degrees(float const rad) {
    return rad * one_eighty_over_pi;
}

// =============================================================================
// Vector length
[[nodiscard]] inline float length_squared(Vec2 const &v) {
    return (v.x * v.x) + (v.y * v.y);
}
[[nodiscard]] inline float length_squared(Vec3 const &v) {
    return (v.x * v.x) + (v.y * v.y) + (v.z * v.z);
}
[[nodiscard]] inline float length_squared(Vec4 const &v) {
    return (v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w);
}

template <typename Vector>
[[nodiscard]] inline float length(Vector const &v) {
    float length_sq = length_squared(v);

    // If we're dealing with the zero vector, return zero
    if(std::abs(length_sq) < epsilon) {
        return 0.0f;
    }

    // If it's a unit vector, skip the square root calculation
    if(std::abs(length_sq - 1.0f) < epsilon) {
        return length_sq;
    }

    return std::sqrt(length_sq);
}

// =============================================================================
// Normalizing
[[nodiscard]] Vec2 normalize(Vec2 const &v);
[[nodiscard]] Vec3 normalize(Vec3 const &v);
[[nodiscard]] Vec4 normalize(Vec4 const &v);

// =============================================================================
// Dot Product
[[nodiscard]] float dot(Vec2 const &a, Vec2 const &b);
[[nodiscard]] float dot(Vec3 const &a, Vec3 const &b);
[[nodiscard]] float dot(Vec4 const &a, Vec4 const &b);

// =============================================================================
// Cross Product
[[nodiscard]] Vec3 cross(Vec3 const &a, Vec3 const &b);

// =============================================================================
// Matrix operations
Mat4 transpose(Mat4 const &m);
Mat4 translate(Mat4 const &m, Vec3 const &v);
Mat4 rotate(Mat4 const &m, float const angle_degrees, Vec3 const &axis);
Mat4 scale(Mat4 const &m, float const &pct);

// =============================================================================
// Camera math
Mat4 look_at_rh(Vec3 const &position, Vec3 const &target, Vec3 const &up);

Mat4 orient_view_rh(Vec3 const &position, Vec3 const &forward,
                    Vec3 const &side, Vec3 const &up);

Mat4 persp_proj_rh_no(float const vfov_degrees, float const aspect_ratio,
                      float const near_plane, float const far_plane);

Mat4 ortho_proj_rh_zo(float const near_plane, float const far_plane,
                      float const left, float const right,
                      float const bottom, float const top);

Mat4 persp_proj_rh_zo_inf(float const near_plane, float const vfov_degrees,
                          float const aspect_ratio);

Mat4 persp_proj_rh_oz_inf(float const near_plane, float const vfov_degrees,
                          float const aspect_ratio);

} // namespace btx::math

#endif // BRASSTACKS_MATH_MATH_HPP