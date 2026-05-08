#include "piano.hh"

#include <cmath>
#include <algorithm>

#include "moremath.hh"

using namespace moremath;

namespace syn
{
    //// PIANO /////////////////////////////////////////////////////////////////

    Piano::Piano()
    {
        type = I_PIANO;
    }

    float Piano::CalculateBaseSample(Note note, double totalTimePlaying)
    {
        const std::tuple<double, double> harmonics[] = {
            { 1.0, 1.0 }, // thing itself
            { std::pow(2.0, 12.0 / 12.0), 0.5 }, // +1o
            { std::pow(2.0, (12.0 + 7.0) / 12.0), 0.25 }, // +1o +p5th
            { std::pow(2.0, 24.0 / 12.0), 0.125 }, // +2o
        };

        float sample = 0.0;
        
        for (const auto& harmonic : harmonics)
        {
            double multiplier = std::get<0>(harmonic);
            double amplitude = std::get<1>(harmonic);
            sample += std::sin(
                MM_TAU * totalTimePlaying * GetFreqEqualTemperament(note.pitch) * multiplier
            ) * amplitude;
        }

        sample /= sizeof(harmonics) / sizeof(harmonics[0]);

        sample *= note.intensity / 127.0;
        
        return sample;
    }

    ////////////////////////////////////////////////////////////////////////////
    
}