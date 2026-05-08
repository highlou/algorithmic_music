#include "wavegenerator.hh"

#include <cmath>

#include "moremath.hh"

using namespace moremath;

namespace syn
{
    //// WAVE GENERATOR ////////////////////////////////////////////////////////

    void WaveGenerator::GenerateSamplesSeconds(
        std::uint32_t sampleRate,
        double duration,
        double frequency,
        double amplitude,
        const std::function<void(float, std::size_t)>& fn
    ) {
        std::uint32_t nSamples = duration * sampleRate;
        GenerateSamples(
            sampleRate,
            nSamples,
            frequency,
            amplitude,
            fn
        );
    }

    void WaveGenerator::GenerateSamples(
        std::uint32_t sampleRate,
        std::uint32_t nSamples,
        double frequency,
        double amplitude,
        const std::function<void(float, std::size_t)>& fn,
        bool stepPhaseForward
    ) {
        double step = 1.0 / sampleRate;
        
        double now = phase;

        for (unsigned i = 0; i < nSamples; i++)
        {
            double sample = 0.0;
            
            switch (type)
            {
                case W_SINE:
                    sample = std::sin(2.0 * MM_PI * now * frequency) * amplitude;
                    break;
                case W_SQUARE:
                    sample = sign(std::sin(2.0 * MM_PI * now * frequency)) * amplitude * 0.5;
                    break;
                case W_TRIANGLE:
                    sample = triangle(2.0 * MM_PI * now * frequency) * amplitude;
                    break;
                case W_SAWTOOTH:
                    sample = sawtooth(2.0 * MM_PI * now * frequency) * amplitude;
                    break;
                default:
                    break;
            }

            fn((float)sample, i);
            now += step;

            now -= phase > 1.0;
        }

        if (stepPhaseForward)
        {
            phase = now;
        }
    }

    void WaveGenerator::ShiftPhaseSamples(std::uint32_t sampleRate, std::uint32_t samples)
    {
        double step = 1.0 / sampleRate;
        phase += step * samples;
        
        while (phase > 1.0)
        {
            phase -= 1.0;
        }
    }

    ////////////////////////////////////////////////////////////////////////////
}