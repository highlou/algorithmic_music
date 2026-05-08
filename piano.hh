#ifndef __ALGORITHMIC_MUSIC_PIANO_HH__
#define __ALGORITHMIC_MUSIC_PIANO_HH__

#include "instrument.hh"

namespace syn
{
    struct Piano : Instrument
    {
        Piano();
    protected:
        float CalculateBaseSample(Note note, double totalTimePlaying) override;
        // float CalculateDampening(Note note, double totalTimePlaying);
    };

}

#endif // __ALGORITHMIC_MUSIC_PIANO_HH__