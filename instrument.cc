#include "instrument.hh"

#include <exception>
#include <stdexcept>
#include <algorithm>
#include <cmath>

#include "piano.hh"
#include "moremath.hh"

using namespace moremath;

namespace syn
{
    //// INSTRUMENT ////////////////////////////////////////////////////////////
    
    std::unique_ptr<Instrument> Instrument::NewUniquePtr(InstrumentType type)
    {
        switch (type)
        {
            case I_NONE:
                return std::make_unique<Instrument>();
            case I_PIANO:
                return std::make_unique<Piano>();
            default:
                throw std::invalid_argument(std::string("Unknown InstrumentType ") + std::to_string(type));
                break;
        }
    }

    InstrumentType Instrument::GetType()
    {
        return type;
    }

    void Instrument::Init(std::uint8_t tempo_, std::uint32_t sampleRate_)
    {
        tempo = tempo_;
        sampleRate = sampleRate_;
    }

    void Instrument::AddNote(Note note)
    {
        notesPlayed.push_back({ note, 0.0 });
    }

    float Instrument::GetNextSample()
    {
        double delta = 1.0 / sampleRate;
        float sample = 0.0;
        std::size_t nPlayingNotes = 0;

        for (std::size_t i = 0; i < notesPlayed.size(); ++i)
        {
            auto& pair = notesPlayed[i];
            const Note& note = pair.first;
            double& timePlayed = pair.second;

            // // Remove the note(s) from notesReleased that have the same pitch as
            // // the note being played if present.
            // for (std::size_t j = 0; j < notesReleased.size(); ++j)
            // {
            //     // TODO: remove "cutting" artifacts from the previous notes not
            //     // being released already?
            //     const auto& released = notesReleased[j];
            //     if (released.first.pitch == note.pitch)
            //     {
            //         notesReleased.erase(notesReleased.begin() + j);
            //         --j;
            //     }
            // }

            // Remove note if it's completed.
            if (timePlayed >= note.GetDurationSeconds(tempo))
            {
                // Add it to the array containing the released notes.
                notesReleased.push_back({ });
                notesReleased.back().first = note;
                notesReleased.back().second = timePlayed;

                notesPlayed.erase(notesPlayed.begin() + i);
                --i;
            }
            else
            {
                sample += CalculatePressedNoteSample(note, timePlayed);
                ++nPlayingNotes;

                // Update the completion of the note.
                timePlayed += delta;
            }
        }

        for (std::size_t i = 0; i < notesReleased.size(); ++i)
        {
            auto& pair = notesReleased[i];
            const Note& note = pair.first;
            double& timePlayed = pair.second;
            
            float result = CalculateReleasedNoteSample(note, timePlayed);

            timePlayed += delta;

            if (result == -INFINITY)
            {
                notesReleased.erase(notesReleased.begin() + i);
                --i;
            }
            else
            {
                sample += result;
            }
        }

        if (nPlayingNotes > 0) { sample /= nPlayingNotes; }

        return sample;
    }

    float Instrument::CalculatePressedNoteSample(
        Note note,
        double totalTimePlaying
    ) {
        float sample = CalculateBaseSample(note, totalTimePlaying);
        
        // Dampening.
        sample *= CalculateDampening(note, totalTimePlaying);

        return sample;
    }

    float Instrument::CalculateReleasedNoteSample(Note note, double totalTimePlaying)
    {
        float sample = CalculateBaseSample(note, totalTimePlaying);
        float amp = CalculateReleasedDampening(note, totalTimePlaying);
        sample *= amp;
        return std::abs(amp) > 0.001 ? sample : -INFINITY;
    }

    float Instrument::CalculateBaseSample(Note, double)
    {
        return 0.0;
    }

    float Instrument::CalculateDampening(Note, double totalTimePlaying)
    {
        // I know this math is very inefficient but oh well.
        const double x = totalTimePlaying;
        constexpr double a = 44.0;
        constexpr double m = 0.02;
        constexpr double n = 4.0;
        // Thanks ChatGPT for helping me to get the maximum value for the
        // function in the numerator for f(x).
        const double f_x = (std::pow(x, MM_E / a) * std::exp(-x)) / (std::pow(a, -MM_E / a));
        // Thanks, Google AI Overview, for giving me this function where m is
        // the constant 1.
        const double g_x = std::pow(x, n) / (std::pow(x, n) + std::pow(m - x, n));
        return f_x * g_x;
    }
    
    float Instrument::CalculateReleasedDampening(Note note, double totalTimePlaying)
    {
        const double noteDuration = note.GetDurationSeconds(tempo);
        const double x = totalTimePlaying - noteDuration;
        float amp = CalculateDampening(note, totalTimePlaying);
        // amp *= std::clamp(1.0 / (1.0 + std::exp(8.0 * (x-0.1) * std::exp(2.0))), 0.0, 1.0);
        amp *= std::clamp(-500.0 * (x*x) + 1.0, 0.0, 1.0);
        return amp;
        // return 0.0;
    }
    ////////////////////////////////////////////////////////////////////////////
}