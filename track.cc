#include "track.hh"

#include <stdexcept>

namespace syn
{
    //// MEASURE ///////////////////////////////////////////////////////////////

    std::pair<std::uint32_t, std::uint32_t> GetMeasureLengthExp(const std::vector<Note>& measure)
    {
        std::pair<std::uint32_t, std::uint32_t> measureLength{};
        measureLength.second = 2;

        for (auto note : measure)
        {
            std::pair<std::uint32_t, std::uint32_t> noteLength = GetNoteLengthExp(note);

            std::int64_t denominatorExpDiff =
                static_cast<std::int32_t>(noteLength.second)
                - static_cast<std::int32_t>(measureLength.second);

            if (denominatorExpDiff > 0) // noteLength denominator greater than measureLength
            {
                measureLength.first <<= denominatorExpDiff;
                measureLength.second += denominatorExpDiff;
            }
            else if (denominatorExpDiff < 0) // measureLength denominator greater than noteLength
            {
                noteLength.first <<= -denominatorExpDiff;
                noteLength.second += -denominatorExpDiff;
            }
            
            measureLength.first += noteLength.first;
        }

        return measureLength;
    }

    std::pair<std::uint32_t, std::uint32_t> GetMeasureLength(const std::vector<Note>& measure)
    {
        auto measureLengthExp = GetMeasureLengthExp(measure);
        std::pair<std::uint32_t, std::uint32_t> measureLength = {
            measureLengthExp.first,
            1 << measureLengthExp.second
        };

        // simplify the fraction
        bool topHalvable, bottomHalvable;
        while ((topHalvable = !(measureLength.first & 1))
            && (bottomHalvable = !(measureLength.second & 1)))
        {
            measureLength.first >>= 1;
            measureLength.second >>= 1;
        }

        return measureLength;
    }

    ////////////////////////////////////////////////////////////////////////////

    //// TRACK /////////////////////////////////////////////////////////////////

    void Track::Init(const std::vector<std::vector<Measure>>& subtracks_)
    {
        // reset self.
        *this = Track{};

        subtracks = std::vector<std::vector<Measure>>(subtracks_);

        if (subtracks.size() != 0)
        {
            currentMeasure.resize(subtracks.size());
            currentNote.resize(subtracks.size());
            noteCompletion.resize(subtracks.size());
        }
        else
        {
        }
    }

    ErrorStatus Track::CheckSelf(TimeSignature timeSignature) const
    {
        std::size_t nSubtracks = subtracks.size();
        
        if (nSubtracks != currentMeasure.size()
            || nSubtracks != currentNote.size()
            || nSubtracks != noteCompletion.size())
        {
            return ES_N_SUBTRACKS_NOT_CONSISTENT;
        }

        if (nSubtracks)
        {
            std::pair<std::uint8_t, std::uint8_t> timeSigFrac;
            timeSigFrac.first = timeSignature.top;
            timeSigFrac.second = timeSignature.bottom;
            
            // simplify the fraction
            bool topHalvable, bottomHalvable;
            while ((topHalvable = !(timeSigFrac.first & 1))
                && (bottomHalvable = !(timeSigFrac.second & 1)))
            {
                timeSigFrac.first >>= 1;
                timeSigFrac.second >>= 1;
            }

            std::size_t nMeasures = subtracks[0].size();

            if (!nMeasures)
            {
                return ES_FIRST_SUBTRACK_N_MEASURE_ZERO;
            }

            for (const auto& subtrack : subtracks)
            {
                if (subtrack.size() != nMeasures)
                {
                    return ES_SUBTRACK_N_MEASURES_NOT_CONSISTENT;
                }

                for (const auto& measure : subtrack)
                {
                    auto measureLength = GetMeasureLength(measure);

                    if (measureLength.first != timeSigFrac.first
                        || measureLength.second != timeSigFrac.second)
                    {
                        return ES_MEASURE_LENGTH_NOT_TIME_SIGNATURE;
                    }
                }
            }
        }

        return ES_SUCCESS;
    }

    // Credit to ChatGPT for the error-throwing functionality.
    std::pair<Note, double> Track::GetCurrentNoteSubtrack(
        std::size_t subtrackIndex
    ) const {
        // Check subtrack index
        if (subtrackIndex >= subtracks.size() ||
            subtrackIndex >= currentMeasure.size() ||
            subtrackIndex >= currentNote.size() ||
            subtrackIndex >= noteCompletion.size()) {
            throw std::out_of_range("Invalid subtrackIndex");
        }

        const auto& subtrack = subtracks[subtrackIndex];

        // Check measure index
        if (currentMeasure[subtrackIndex] >= subtrack.size()) {
            throw std::out_of_range("Invalid measure index");
        }

        const auto& measure = subtrack[currentMeasure[subtrackIndex]];

        // Check note index
        if (currentNote[subtrackIndex] >= measure.size()) {
            throw std::out_of_range("Invalid note index");
        }

        return {
            measure[currentNote[subtrackIndex]],
            noteCompletion[subtrackIndex]
        };
    }

    std::vector<std::tuple<Note, double, double, bool>> Track::GoForwardSubtrack(
        std::size_t idx,
        std::uint8_t tempo,
        double delta
    ) {
        std::vector<std::tuple<Note, double, double, bool>> playedNotes{};
        double totalTimePlayed = 0.0;
        bool isNewNote = currentMeasure[idx] == 0 && currentNote[idx] == 0 && noteCompletion[idx] == 0.0;

        while (totalTimePlayed < delta && subtracks[idx].size() > currentMeasure[idx])
        {
            syn::Note note = GetCurrentNoteSubtrack(idx).first;
            double noteFullDuration = note.GetDurationSeconds(tempo);
            playedNotes.push_back(std::tuple<Note, double, double, bool>{
                // The note itself
                note,
                // The amount of time since then that the note has played in this
                // delta
                std::min(delta - totalTimePlayed, noteFullDuration * (1.0 - noteCompletion[idx])),
                // The amount of time passed since the point in time that delta will
                // be added to before this note started playing
                totalTimePlayed,
                isNewNote// how to determine whether it's new note or not??
            });

            isNewNote = false;

            double diff = (delta - totalTimePlayed) - std::get<1>(playedNotes.back());

            totalTimePlayed += std::get<1>(playedNotes.back());
            noteCompletion[idx] += totalTimePlayed / noteFullDuration;

            if (diff > 0.0) // This note will be finished in this duration.
            {
                // TODO: wher eto put isNewNote to true???
                // Increment each of the indices and reset noteCompletion[idx].
                if (subtracks[idx][currentMeasure[idx]].size() <= currentNote[idx] + 1)
                {
                    // the current measure within this subtrack doesn't have
                    // another note after this current note which has been
                    // completed.

                    // Increment to the next measure and reset the sub-measure
                    // playhead (currentNote).
                    ++currentMeasure[idx];
                    currentNote[idx] = 0;

                    if (subtracks[idx].size() <= currentMeasure[idx])
                    {
                        // this subtrack is finished
                        break;
                    }
                }
                else
                {
                    ++currentNote[idx];
                }
                
                noteCompletion[idx] = 0.0;
                isNewNote = true;
            }
            else // note is not finished in this delta, delta exhausted
            {
                break;
            }
        }

        return playedNotes;
    }

    std::vector<std::vector<std::tuple<Note, double, double, bool>>> Track::GoForward(
        std::uint8_t tempo,
        double duration
    ) {
        std::vector<std::vector<std::tuple<Note, double, double, bool>>> results;

        for (std::size_t i = 0; i < subtracks.size(); ++i)
        {
            auto result = GoForwardSubtrack(i, tempo, duration);
            results.push_back(result);
        }

        return results;
    }
    
    std::vector<std::pair<Note, double>> Track::GetCurrentNotes() const
    {
        std::vector<std::pair<Note, double>> currentPairs{};
        
        for (std::size_t i = 0; i < subtracks.size(); ++i)
        {
            std::pair<Note, double> pair = GetCurrentNoteSubtrack(i);
            currentPairs.push_back(pair);
        }

        return currentPairs;
    }

    // ::GoForward()

    ////////////////////////////////////////////////////////////////////////////

}