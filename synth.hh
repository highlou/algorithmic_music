/* Created 2026-04-17 */

#ifndef ALGORITHMIC_MUSIC_SYNTH_HH
#define ALGORITHMIC_MUSIC_SYNTH_HH

#include <vector>
#include <cstdint>

namespace synth
{
    constexpr std::uint8_t P_A4 = 0x7f;
    constexpr std::uint8_t P_C4 = P_A4 + 3;
    
    constexpr std::uint8_t I_SINE       = 0;
    constexpr std::uint8_t I_SQUARE     = 1;
    constexpr std::uint8_t I_TRIANGLE   = 2;
    constexpr std::uint8_t I_SAWTOOTH   = 3;

    constexpr std::uint8_t W_SINE       = 0;
    constexpr std::uint8_t W_SQUARE     = 1;
    constexpr std::uint8_t W_TRIANGLE   = 2;
    constexpr std::uint8_t W_SAWTOOTH   = 3;

    struct Note
    {
        std::uint8_t pitch      : 8; // 0 means rest
        std::uint8_t duration   : 4; // 0 means whole, 1 means half, etc.
        std::uint8_t intensity  : 7;
        bool dotted             : 1;
    };

    struct Chord
    {
        std::uint8_t root       : 8;
        std::uint8_t inversion  : 2;
        std::uint8_t quality    : 2;

        bool         seventh    : 1;
        bool         ninth      : 1;
        bool         eleventh   : 1;
        bool         ext_minor  : 1; // are the extended note(s) minor? (if applicable)

        std::uint8_t intensity : 7;
        bool         dotted    : 1;
    };

    struct TimeSignature
    {
        std::uint8_t tempo  : 8;
        std::uint8_t top    : 4;
        std::uint8_t bottom : 4;
    };

    struct WaveGenerator
    {
        std::uint8_t type : 8;
        double t = 0.0;

        std::vector<double>& GenerateSamples(
            double duration,
            double frequency,
            double amplitude
        );
    };

    struct Synth
    {
        using Measure = std::vector<Note>; // monophonic
        using Playhead = std::size_t;

        struct Track
        {
            std::vector<std::vector<Measure>> subtrack; // subtracks are monophonic
            std::uint8_t instrument;
            std::vector<std::size_t> currentMeasure;
            std::vector<std::size_t> currentNote; // per each measure
            std::vector<double> noteCompletion; // 0.0 means start of note in a measure
        };

        TimeSignature signature;
        std::vector<Track> tracks;
        
        void StepTrack(std::size_t trackNo, double delta);
        void Step(double delta);
    };
}

#endif // ALGORITHMIC_MUSIC_SYNTH_HH