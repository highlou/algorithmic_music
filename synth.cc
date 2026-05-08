/* Created 2026-04-17 */

#include "synth.hh"

#include <stdexcept>
#include <string>
#include <regex>
#include <cstdio>
#include <cmath>

#include "moremath.hh"

using namespace moremath;

namespace syn
{
    //// SYNTH /////////////////////////////////////////////////////////////////

    bool Synth::Init(
        std::uint32_t sampleRate_,
        TimeSignature signature_,
        const std::vector<Track>& tracks_,
        const std::vector<InstrumentType>& instrumentTypes_
    ) {
        sampleRate = sampleRate_;
        signature = signature_;
        tracks = std::vector<Track>(tracks_);
        instruments.clear();

        for (InstrumentType instrumentType : instrumentTypes_)
        {
            instruments.push_back(Instrument::NewUniquePtr(instrumentType));
            instruments.back()->Init(signature.tempo, sampleRate);
        }
        
        return CheckSelf();
    }

    bool Synth::CheckSelf() const
    {
        // Make sure each track follows the time signature.
        for (const auto& track : tracks)
        {
            if (track.CheckSelf(signature) != ES_SUCCESS)
            {
                return false;
            }
        }

        if (instruments.size() != tracks.size())
        {
            return false;
        }

        return true;
    }

    std::vector<float> Synth::Step(double delta)
    {
        std::vector<std::size_t> sampleNTracks{};
        std::vector<float> samples{};

        for (std::size_t trackI = 0; trackI < tracks.size(); ++trackI)
        {
            std::vector<float> trackSamples = StepTrack(trackI, delta);

            if (samples.size() < trackSamples.size())
            {
                samples.resize(trackSamples.size());
                sampleNTracks.resize(trackSamples.size());
            }
            
            for (std::size_t i = 0; i < trackSamples.size(); ++i)
            {
                samples[i] =
                    samples[i] * sampleNTracks[i]
                    / (++sampleNTracks[i])
                    + trackSamples[i] / sampleNTracks[i];
            }
        }

        return samples;
    }

    std::vector<float> Synth::StepTrack(std::size_t trackNo, double delta)
    {
        constexpr std::size_t chunkSize = 1024;
        std::size_t nSamples = delta * sampleRate;
        std::vector<float> samples{};

        samples.resize(chunkSize);

        std::size_t i;
        for (i = 0; i < nSamples; ++i)
        {
            if (i >= samples.size())
            {
                samples.resize(samples.size() + chunkSize);
            }
            float sample = StepTrackSample(trackNo);
            if (sample == -INFINITY)
            {
                break;
            }
            samples[i] = sample;
        }

        samples.resize(i);

        return samples;
    }

    float Synth::StepTrackSample(std::size_t trackNo)
    {
        double delta = 1.0 / sampleRate;
        Track& track = tracks[trackNo];
        std::unique_ptr<Instrument>& instrument = instruments[trackNo];

        auto tracksPlayInfo = track.GoForward(signature.tempo, delta);
        float sample = 0.0;
        
        bool atLeastOneNote = false;
        
        for (const auto& subtrackPlayInfo : tracksPlayInfo)
        {
            // float subSample = 0.0;

            for (const auto& measurePlayInfo : subtrackPlayInfo)
            {
                auto note = std::get<0>(measurePlayInfo);
                auto isNewNote = std::get<3>(measurePlayInfo);

                if (isNewNote)
                {
                    instrument->AddNote(note);
                }

                atLeastOneNote = true;
            }
        }

        sample = instruments[trackNo]->GetNextSample();

        if (!atLeastOneNote) // all tracks are finished because empty values
        {
            return -INFINITY;
        }

        return sample;
    }

    ////////////////////////////////////////////////////////////////////////////
}