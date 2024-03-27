#ifndef BRASSTACKS_MATH_COLORS_HPP
#define BRASSTACKS_MATH_COLORS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/math/Vec3.hpp"

namespace btx {

using Color = math::Vec3;

} // namespace btx

namespace btx::colors {

static ::btx::Color const red            { 1.00f, 0.00f, 0.00f };
static ::btx::Color const green          { 0.00f, 1.00f, 0.00f };
static ::btx::Color const blue           { 0.00f, 0.00f, 1.00f };
static ::btx::Color const white          { 1.00f, 1.00f, 1.00f };
static ::btx::Color const black          { 0.00f, 0.00f, 0.00f };

static ::btx::Color const quarter_white  { 0.25f, 0.25f, 0.25f };
static ::btx::Color const half_white     { 0.50f, 0.50f, 0.50f };

static ::btx::Color const american_green { 0.15f, 0.65f, 0.25f };
static ::btx::Color const denim_blue     { 0.15f, 0.25f, 0.65f };
static ::btx::Color const terra_cotta    { 0.85f, 0.45f, 0.35f };

static ::btx::Color const sunlight       { 0.39f, 0.37f, 0.25f };
static ::btx::Color const glacier_gray   { 0.77f, 0.78f, 0.78f };
static ::btx::Color const simple_gray    { 0.34f, 0.34f, 0.33f };

} // namespace btx::colors

#endif // BRASSTACKS_MATH_COLORS_HPP