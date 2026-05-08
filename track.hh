#ifndef __ALGORITHMIC_MUSIC_TRACK_HH__
#define __ALGORITHMIC_MUSIC_TRACK_HH__

#include <vector>
#include <cstdint>
#include <utility>

#include "note.hh"
#include "error.hh"

namespace syn
{
    struct TimeSignature
    {
        std::uint8_t tempo  : 8 = 0;
        std::uint8_t top    : 4 = 0;
        std::uint8_t bottom : 4 = 0; // NOT exponential
        std::uint8_t key    : 4 = 0; // half steps to modify pitch
    };

    using Measure = std::vector<Note>; // monophonic

    std::pair<std::uint32_t, std::uint32_t> GetMeasureLengthExp(const std::vector<Note>& measure);
    std::pair<std::uint32_t, std::uint32_t> GetMeasureLength(const std::vector<Note>& measure);

    struct Track
    {
        std::vector<std::vector<Measure>> subtracks{}; // subtracks are monophonic
        std::uint8_t instrument = 0;
        std::vector<std::size_t> currentMeasure{};
        std::vector<std::size_t> currentNote{}; // per each measure across subtracks
        std::vector<double> noteCompletion{}; // 0.0 means start of note in a measure; 1.0 means completion
        
        void Init(const std::vector<std::vector<Measure>>& subtracks);
        ErrorStatus CheckSelf(TimeSignature timeSignature) const;
        // double is how much the note has been played completely (0.0 -> 1.0)
        std::pair<Note, double> GetCurrentNoteSubtrack(std::size_t subtrack) const;
        // first double is how much time the note is played in this delta
        // while the second double is the point in time between 0.0 s and
        // delta that the note started playing on
        // if the track is done, this function will return an empty vector.
        // the bool is true if the note is a new note
        std::vector<std::tuple<Note, double, double, bool>> GoForwardSubtrack(
            std::size_t subtrack,
            std::uint8_t tempo,
            double delta
        );
        // if a subtrack kis done, that element of the parent vector will be an
        // empty vector.
        std::vector<std::vector<std::tuple<Note, double, double, bool>>> GoForward(
            std::uint8_t tempo,
            double duration
        );
        // double is how much the note has been played completely (0.0 -> 1.0)
        std::vector<std::pair<Note, double>> GetCurrentNotes() const;
    };

}

#endif // __ALGORITHMIC_MUSIC_TRACK_HH__