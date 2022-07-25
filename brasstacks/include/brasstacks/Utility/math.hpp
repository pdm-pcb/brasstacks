#ifndef BRASSTACKS_UTILITY_MATH_HPP
#define BRASSTACKS_UTILITY_MATH_HPP

namespace btx {
namespace math {

static constexpr float pi           = 3.14159265f;
static constexpr float two_pi       = pi * 2.0f;
static constexpr float pi_over_two  = pi / 2.0f;
static constexpr float pi_over_2    = pi_over_two;
static constexpr float pi_over_four = pi / 4.0f;
static constexpr float pi_over_4    = pi_over_four;
static constexpr float pi_over_180  = pi / 180.0f;

static constexpr float one_over_sixty  = 1 / 60.0f;
static constexpr float one_over_thirty = 1 / 30.0f;

static constexpr double one_over_sixty_ms  = 1 / 60.0 * 1000;
static constexpr double one_over_thirty_ms = 1 / 30.0 * 1000;

static constexpr double one_over_sixty_us  = 1 / 60.0 * 1'000'000;
static constexpr double one_over_thirty_us = 1 / 30.0 * 1'000'000;

} // namespace math
} // namespace btx

#endif // BRASSTACKS_UTILITY_MATH_HPP