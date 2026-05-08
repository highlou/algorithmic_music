#ifndef __ALGORITHMIC_MUSIC_SYNTH_INSTRUMENT_HH__
#define __ALGORITHMIC_MUSIC_SYNTH_INSTRUMENT_HH__

#include <cstdint>
#include <utility>
#include <memory>
#include <vector>
#include <cmath>

#include "note.hh"

namespace syn
{
    enum InstrumentType : std::uint8_t
    {
        I_NONE = 0,
        I_PIANO
    };

    struct Instrument
    {
        virtual void Init(std::uint8_t tempo, std::uint32_t sampleRate);
        virtual void AddNote(Note note);

        static std::unique_ptr<Instrument> NewUniquePtr(InstrumentType type);
        InstrumentType GetType();
        float GetNextSample();
        
    protected:
        InstrumentType type;
        std::uint8_t tempo;
        std::uint32_t sampleRate;
        std::vector<std::pair<Note, double>> notesPlayed{};
        std::vector<std::pair<Note, double>> notesReleased{};

        virtual float CalculatePressedNoteSample(Note note, double totalTimePlaying);
        virtual float CalculateReleasedNoteSample(Note note, double totalTimePlaying);
        virtual float CalculateBaseSample(Note note, double totalTimePlaying);
        virtual float CalculateDampening(Note note, double totalTimePlaying);
        virtual float CalculateReleasedDampening(Note note, double totalTimePlaying);
    };

}

#endif // __ALGORITHMIC_MUSIC_SYNTH_INSTRUMENT_HH__