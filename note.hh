#ifndef __ALGORITHMIC_MUSIC_SYNTH_NOTE_HH__
#define __ALGORITHMIC_MUSIC_SYNTH_NOTE_HH__

#include <cstdint>
#include <string>

namespace syn
{
    constexpr std::uint8_t P_A4 = 0x7f;
    constexpr std::uint8_t P_C4 = P_A4 - 9;

    constexpr std::uint8_t D_WHOLE      = 0;
    constexpr std::uint8_t D_HALF       = 1;
    constexpr std::uint8_t D_QUARTER    = 2;
    constexpr std::uint8_t D_EIGHTH     = 3;
    constexpr std::uint8_t D_SIXTEENTH  = 4;
    constexpr std::uint8_t D_THIRTYSECOND=5;
    constexpr std::uint8_t D_SIXTYFOURTH =6;

    struct Note
    {
        std::uint8_t pitch      : 8 = 0; // 0 means rest
        std::uint8_t duration   : 4 = 0; // 0 means whole, 1 means half, 2 is Q,etc.
        std::uint8_t intensity  : 7 = 0;
        bool         dotted     : 1 = 0;
        bool         tiedNext   : 1 = 0; // is it tied to the next note?
        bool         tiedPrev   : 1 = 0; // is it tied to the previous note?

        double GetDurationSeconds(std::uint8_t tempo) const;
    };

    struct Chord
    {
        std::uint8_t root       : 8 = 0;
        std::uint8_t inversion  : 2 = 0;
        std::uint8_t quality    : 2 = 0;

        bool         seventh    : 1 = 0;
        bool         ninth      : 1 = 0;
        bool         eleventh   : 1 = 0;
        bool         ext_minor  : 1 = 0; // are the extended note(s) minor? (if applicable)

        std::uint8_t intensity  : 7 = 0;
        bool         dotted     : 1 = 0;
        bool         tiedNext   : 1 = 0; // is it tied to the next note?
        bool         tiedPrev   : 1 = 0; // is it tied to the previous note?
    };

    std::pair<std::uint8_t, std::uint8_t> GetNoteLengthExp(Note note);
    std::pair<std::uint8_t, std::uint32_t> GetNoteLength(Note note);

    double GetFreqEqualTemperament(std::uint8_t pitch, double a4 = 440.0);

    // Example: C4, Gb3, D#9. The pitch, stored in 8-bits, can overflow,
    // however.
    std::uint8_t GetPitchFromStr(const std::string& str, std::uint8_t keySignature = 0);
    std::string GetStrFromPitch(std::uint8_t pitch, std::uint8_t keySignature = 0, bool displayFlat = false);
}

#endif // __ALGORITHMIC_MUSIC_SYNTH_NOTE_HH__