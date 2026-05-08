/* Created 2026-04-18 */

#ifndef ALGORITHMIC_MUSIC_MOREMATH_HH
#define ALGORITHMIC_MUSIC_MOREMATH_HH

#include <utility>

namespace moremath
{
    constexpr double MM_PI = 3.14159265358979323846;
    constexpr double MM_TAU = 2 * MM_PI;
    constexpr double MM_E = 2.7182818284590452353602874713527;

    template<typename T>
    constexpr T sign(T val)
    {
        // Credit: Google AI Overview.
        return (T(0) < val) - (val < T(0));
    }

    double triangle(double x);
    double sawtooth(double x);
}

#endif // ALGORITHMIC_MUSIC_MOREMATH_HH