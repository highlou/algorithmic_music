/* Created 2026-04-19 */

#include "moremath.hh"

#include <cmath>

namespace moremath
{
    double triangle(double x)
    {
        // Credit to ChatGPT for optimizing.
        x = std::fmod(x, 2 * MM_PI);
        x += 2.0 * MM_PI * (x < 0.0);
        return 1.0 - 2.0 * std::abs(x / MM_PI - 1.0);
    }

    double sawtooth(double x)
    {
        return 2.0 * ((x / MM_TAU) - std::floor(x / MM_TAU)) - 1.0;
    }

    // template<class T>
    // std::pair<T, T> simplifyFraction(const std::pair<T, T>& fraction)
    // {

    // }
}