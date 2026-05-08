#ifndef __ALGORITHMIC_MUSIC_SYNTH_WAVE_GENERATOR_HH__
#define __ALGORITHMIC_MUSIC_SYNTH_WAVE_GENERATOR_HH__

#include <functional>

#include "note.hh"

namespace syn
{
    constexpr std::uint8_t W_SINE       = 0;
    constexpr std::uint8_t W_SQUARE     = 1;
    constexpr std::uint8_t W_TRIANGLE   = 2;
    constexpr std::uint8_t W_SAWTOOTH   = 3;

    struct WaveGenerator
    {
        std::uint8_t type : 8   = 0;
        double       phase      = 0.0;

        void GenerateSamplesSeconds(
            std::uint32_t sampleRate,
            double duration,
            double frequency,
            double amplitude,
            const std::function<void(float, std::size_t)>& fn
        );

        void GenerateSamples(
            std::uint32_t sampleRate,
            std::uint32_t nSamples,
            double frequency,
            double amplitude,
            const std::function<void(float, std::size_t)>& fn,
            bool stepPhaseForward = true
        );
        
        void ShiftPhaseSamples(std::uint32_t sampleRate, std::uint32_t samples);
    };

}

#endif // __ALGORITHMIC_MUSIC_SYNTH_WAVE_GENERATOR_HH__