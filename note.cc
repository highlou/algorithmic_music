#include "note.hh"

#include <cmath>
#include <regex>

namespace syn
{
    //// NOTE //////////////////////////////////////////////////////////////////

    double Note::GetDurationSeconds(std::uint8_t tempo) const
    {
        return 1.0 / std::pow(2.0, duration - 2.0) * (60.0 / tempo) * (1.0 + (0.5 * dotted));
    }

    ////////////////////////////////////////////////////////////////////////////

    //// CHORD /////////////////////////////////////////////////////////////////

    std::vector<Note> Chord::GetNotes()
    {
        std::vector<Note> notes = {
            Note{ .pitch = static_cast<std::uint8_t>(root) },
            Note{ .pitch = static_cast<std::uint8_t>(root + 4) },
            Note{ .pitch = static_cast<std::uint8_t>(root + 7) },
        };

        switch (inversion)
        {
            case 3:
                notes[2].pitch += 12;
                [[fallthrough]];
            case 2:
                notes[1].pitch += 12;
                [[fallthrough]];
            case 1:
                notes[0].pitch += 12;
                [[fallthrough]];
            case 0:
            default:
                break;
        }

        switch (quality)
        {
            case CQ_DIMINISHED:
                --notes[2].pitch;
                [[fallthrough]];
            case CQ_MINOR:
                --notes[1].pitch;
                break;
            case CQ_AUGMENTED:
                ++notes[2].pitch;
                break;
            default:
                break;
        }

        std::int8_t ext_minmajaugdim = 0;
        switch ((static_cast<std::uint8_t>(ext_extra) << 1) | static_cast<std::uint8_t>(ext_minor))
        {
            case 0b00:
                ext_minmajaugdim = 0;
                break;
            case 0b01:
                ext_minmajaugdim = -1;
                break;
            case 0b10:
                ext_minmajaugdim = 1;
                break;
            case 0b11:
                ext_minmajaugdim = -2;
                break;
        }

        if (seventh)
        {
            notes.push_back(Note{
                .pitch = static_cast<std::uint8_t>(root + 11 + ext_minmajaugdim)
            });
        }
        else if (ninth)
        {
            notes.push_back(Note{
                .pitch = static_cast<std::uint8_t>(root + 14 + ext_minmajaugdim)
            });
        }

        for (auto& note : notes)
        {
            note.duration = duration;
            note.intensity = intensity;
            note.dotted = dotted;
            note.tiedNext = tiedNext;
            note.tiedPrev = tiedPrev;
        }

        return notes;
    }

    ////////////////////////////////////////////////////////////////////////////

    std::pair<std::uint8_t, std::uint8_t> GetNoteLengthExp(Note note)
    {
        std::uint8_t top = 1;
        std::uint8_t bottom = note.duration;

        top <<= note.dotted;
        bottom += note.dotted;
        top += note.dotted;

        return { top, bottom };
    }

    std::pair<std::uint8_t, std::uint32_t> GetNoteLength(Note note)
    {
        auto exp = GetNoteLengthExp(note);
        return { exp.first, 1 << exp.second };
    }

    double GetFreqEqualTemperament(std::uint8_t pitch, double a4)
    {
        return a4 * std::pow(2.0, static_cast<double>(pitch - P_A4) / 12.0);
    }

    std::uint8_t GetPitchFromStr(const std::string& str, std::uint8_t keySignature)
    {
        std::regex pattern(
            "^([A-G])(#|b)?(\\d*)$",
            std::regex_constants::icase
        );
        // auto results = std::regex_match(str, pattern);
        std::smatch results;

        if (!std::regex_match(str, results, pattern))
        {
            return 0;
        }
        
        std::uint8_t pitch = 0;
        
        // Convert the note name into note where pitch 0x7f is A4.
        pitch = u8"\x7f\x81\x76\x78\x7a\x7b\x7d"[std::toupper(results[1].str()[0]) - 'A'];
        
        // Augment or diminish the pitch whether it's flat or sharp or natural.
        pitch += results[2].str().size() && results[2].str()[0] == '#';
        pitch -= results[2].str().size() && results[2].str()[0] == 'b';

        pitch += keySignature;
        pitch += static_cast<bool>(results[3].str().size()) * (std::atoi(results[3].str().c_str()) - 4) * 12;

        return pitch;
    }

    std::string GetStrFromPitch(std::uint8_t pitch, std::uint8_t keySignature, bool displayFlat)
    {
        pitch += keySignature;
        signed relToA4 = static_cast<signed>(pitch - P_A4);
        signed relToC4 = static_cast<signed>(pitch) - P_C4;
        const char* noteNamesSharp[] = {
            "A",
            "A#",
            "B",
            "C",
            "C#",
            "D",
            "D#",
            "E",
            "F",
            "F#",
            "G",
            "G#"
        };
        const char* noteNamesFlat[] = {
            "A",
            "Bb",
            "B",
            "C",
            "Db",
            "D",
            "Eb",
            "E",
            "F",
            "Gb",
            "G",
            "Ab"
        };
        const char* noteName = (displayFlat ? noteNamesFlat : noteNamesSharp)[(relToA4 % 12 + 12) % 12];
        std::uint8_t octave = (relToC4 + 1) / 12 + 4 - 1 * (relToC4 < 0);
        return std::string(noteName) + std::to_string(octave);
    }
}