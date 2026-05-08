/* Created 2026-04-17 */

#ifndef __ALGORITHMIC_MUSIC_SYNTH_HH__
#define __ALGORITHMIC_MUSIC_SYNTH_HH__

#include <utility>
#include <vector>
#include <functional>
#include <cstdint>
#include <string>
#include <memory>

#include "note.hh"
#include "wavegenerator.hh"
#include "instrument.hh"
#include "track.hh"
#include "error.hh"

namespace syn
{
    struct Synth
    {
        std::uint32_t sampleRate = 0;
        TimeSignature signature{};
        std::vector<Track> tracks;
        std::vector<std::unique_ptr<Instrument>> instruments; // one instrument per track
        
        bool Init(std::uint32_t sampleRate,
                  TimeSignature signature,
                  const std::vector<Track>& tracks,
                  const std::vector<InstrumentType>& instruments);
        bool CheckSelf() const;
        std::vector<float> StepTrack(std::size_t trackNo, double delta);
        std::vector<float> Step(double delta);
        // if the track is finished, it'll output -INFINITY.
        float StepTrackSample(std::size_t trackNo);
    };
}

#endif // __ALGORITHMIC_MUSIC_SYNTH_HH__