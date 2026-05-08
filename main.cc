/* Created 2026-04-17. */

#include <SFML/Audio.hpp>
#include <SDL2/SDL.h>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cmath>
#include <ctime>
#include <string>

#include "synth.hh"
#include "moremath.hh"

using namespace moremath;

// TODO: rename syn:: to syn::

namespace
{
    // syn::WaveGenerator gen{ .type = syn::W_TRIANGLE };
    syn::Synth synth;
    SDL_AudioSpec spec;

    void callback(void*, Uint8*, int)
    // void callback(void* userdata, Uint8* buffer, int len)
    {
        // std::size_t nSamples = len / sizeof(float);
        // float* floatBuffer = (float*)buffer;
        // gen.GenerateSamples(
        //     spec.freq,
        //     nSamples,
        //     440.,
        //     0.05,
        //     [&floatBuffer](float sample, std::size_t i) -> void
        //     {
        //         floatBuffer[i] = sample;
        //     }
        // );
    }

    // This does indeed work.
    void TestWaveGenerator()
    {
        syn::WaveGenerator waveGen{ .type = syn::W_TRIANGLE };

        double durationSec = 2.0;
        std::size_t nSamples = durationSec * 44100;
        std::int16_t* data = new std::int16_t[nSamples];
        
        for (std::size_t i = 0; i < nSamples; ++i)
        {
            float sample;
            waveGen.GenerateSamples(
                44100,
                1,
                syn::GetFreqEqualTemperament(syn::P_A4),
                0.5,
                [&sample](float x, std::size_t)
                {
                    sample = x;
                },
                false
            );
            waveGen.ShiftPhaseSamples(44100, 1);

            data[i] = static_cast<std::int16_t>(sample * INT16_MAX);
        }

        sf::SoundBuffer sb;

        sb.loadFromSamples(data, nSamples, 1, 44100);

        sb.saveToFile("test.wav");

        // TDOO: test this out
        delete[] data;
    }
}

int main()
{
    // TestWaveGenerator();
    // return 0;
    
    // Test out syn::GetPitchFromStr().
    // {
    //     while (true)
    //     {
    //         std::string s;
    //         std::printf("> ");
    //         std::fflush(stdout);
    //         while (s += std::fgetc(stdin), s.back() != '\n');
    //         s.resize(s.size()-1);
    //         std::printf("%s\n", s.c_str());
            
    //         std::uint8_t pitch = syn::GetPitchFromStr(s, 0);
    //         std::printf("pitch: %hhu\n", pitch);
    //         double freq = syn::GetFreqEqualTemperament(pitch);
    //         std::printf("freq: %f\n", freq);

    //         std::string noteName = syn::GetStrFromPitch(pitch, 0, true);
    //         std::printf("noteName == %s\n", noteName.c_str());
    //     }
    // }

    // return 0;
    
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        std::fprintf(stderr, "%s\n", SDL_GetError());
    }

    SDL_AudioSpec desiredSpec = {
        .freq = 44100,
        .format = AUDIO_F32,
        .channels = 1,
        .silence = 0, // calculated by SDL2; to suppress -Wunused-variable
        .samples = 2048,
        .padding = 0, // to suppress -Wunused-variable
        .size = 0, // calculated by SDL2; to suppress -Wunused-variable
        .callback = &callback,
        .userdata = NULL
    };

    SDL_AudioDeviceID devid = SDL_OpenAudioDevice(
        NULL,
        0,
        &desiredSpec,
        &spec,
        0
    );

    SDL_PauseAudioDevice(devid, 0);

    syn::Track track1{}, track2{};
    track1.Init({
        {
            { // Measure
                syn::Note{syn::P_C4 + 4, 2, 0x7f},
                syn::Note{syn::P_C4 + 2, 2, 0x7f},
                syn::Note{syn::P_C4 + 0, 2, 0x7f},
                // syn::Note{0, 2, 0x7f},
                // syn::Note{0, 2, 0x7f},
                syn::Note{syn::P_C4 + 2, 2, 0x7f},
            },
            {
                syn::Note{syn::P_C4 + 4, 2, 0x7f},
                syn::Note{syn::P_C4 + 4, 2, 0x7f},
                syn::Note{syn::P_C4 + 4, 1, 0x7f},
            },
            {
                syn::Note{syn::P_C4 + 2, 2, 0x7f},
                syn::Note{syn::P_C4 + 2, 2, 0x7f},
                syn::Note{syn::P_C4 + 2, 1, 0x7f},
            },
            {
                syn::Note{syn::P_C4 + 4, 2, 0x7f},
                syn::Note{syn::P_C4 + 4, 2, 0x7f},
                syn::Note{syn::P_C4 + 4, 1, 0x7f},
            },
            {
                syn::Note{syn::P_C4 + 4, 2, 0x7f},
                syn::Note{syn::P_C4 + 2, 2, 0x7f},
                syn::Note{syn::P_C4 + 0, 2, 0x7f},
                syn::Note{syn::P_C4 + 2, 2, 0x7f},
            },
            {
                syn::Note{syn::P_C4 + 4, 2, 0x7f},
                syn::Note{syn::P_C4 + 4, 2, 0x7f},
                syn::Note{syn::P_C4 + 4, 1, 0x7f},
            },
            {
                syn::Note{syn::P_C4 + 2, 2, 0x7f},
                syn::Note{syn::P_C4 + 2, 2, 0x7f},
                syn::Note{syn::P_C4 + 4, 2, 0x7f},
                syn::Note{syn::P_C4 + 2, 2, 0x7f},
            },
            {
                syn::Note{syn::P_C4 + 0, 0, 0x7f},
            },
        },
    });
    track2.Init({
        {
            {
                syn::Note{syn::P_C4 - 12 + 0, 1, 0x7f},
                syn::Note{syn::P_C4 - 12 - 5, 1, 0x7f},
            },
            {
                syn::Note{syn::P_C4 - 12 + 0, 1, 0x7f},
                syn::Note{syn::P_C4 - 12 - 5, 1, 0x7f},
            },
            {
                syn::Note{syn::P_C4 - 12 - 5, 1, 0x7f},
                syn::Note{syn::P_C4 - 12 - 1, 1, 0x7f},
            },
            {
                syn::Note{syn::P_C4 - 12 + 0, 1, 0x7f},
                syn::Note{syn::P_C4 - 12 - 5, 1, 0x7f},
            },
            {
                syn::Note{syn::P_C4 - 12 + 0, 1, 0x7f},
                syn::Note{syn::P_C4 - 12 - 5, 1, 0x7f},
            },
            {
                syn::Note{syn::P_C4 - 12 + 0, 1, 0x7f},
                syn::Note{syn::P_C4 - 12 - 5, 1, 0x7f},
            },
            {
                syn::Note{syn::P_C4 - 12 - 5, 1, 0x7f},
                syn::Note{syn::P_C4 - 12 - 1, 1, 0x7f},
            },
            {
                syn::Note{syn::P_C4 - 12 + 0, 0, 0x7f},
            },
        }
    });

    // syn::TimeSignature timeSign{
    //     .top = 7,
    //     .bottom = 8
    // };

    // syn::ErrorStatus es = track1.CheckSelf(timeSign);

    // std::string esString = syn::GetStringErrorStatus(es);

    // std::printf("Error check: %s\n", esString.data());

    if (!synth.Init(
            spec.freq,
            syn::TimeSignature{ 80, 4, 4 },
            { track1, track2 },
            { syn::I_PIANO, syn::I_PIANO }
        ))
    {
        std::fprintf(stderr, "Failed to initialize synth.\n");
        return 1;
    }
    
    // sf::SoundBuffer sb{};
    
    // This works. So why does StepTrackSample(0) not work?
    // for (std::size_t i = 0; ; ++i)
    // {
    //     auto playInfo = track1.GoForward(120, 0.5);
    //     bool isNotesPlayed = false;
    //     for (const auto& subtrackPlayInfo : playInfo)
    //     {
    //         if (subtrackPlayInfo.size() != 0)
    //         {
    //             isNotesPlayed = true;
    //         }
    //     }
    //     if (!isNotesPlayed)
    //     {
    //         break;
    //     }
    // }

    // while (synth.StepTrackSample(0) != -INFINITY)
    std::size_t sampleI;
    float sample = NAN;
    std::vector<float> samples{};

    // samples = synth.StepTrack(0, 99999999);
    samples = synth.Step(9999999);

    // for (sampleI = 0; sample != -INFINITY; samples.push_back(sample), ++sampleI)
    // {
    //     sample = synth.StepTrackSample(0);
    // }

    std::printf("ended. made %zu samples, samples[100] == %f\n", samples.size(), samples[100]);
    
    std::size_t nSamples = samples.size();
    sf::Int16* intBuffer = new sf::Int16[nSamples];

    for (std::size_t i = 0; i < nSamples; i++)
    {
        intBuffer[i] = samples[i] * INT16_MAX;
    }
    
    sf::SoundBuffer sb{};

    if (!sb.loadFromSamples(intBuffer, samples.size(), 1, synth.sampleRate))
    {
        std::fprintf(stderr, "sb.loadFromSamples() failed.\n");
        return 1;
    }

    delete[] intBuffer;

    sb.saveToFile("out.wav");
    
    // std::printf("finished\n");

    // auto noteDouble = track1.GetCurrentNoteSubtrack(0);
    // std::printf("dur: %f\tcom: %f\n", noteDouble.first.GetDurationSeconds(120), noteDouble.second);

    // auto notesPlayed = track1.GoForwardSubtrack(0, 120, 0.6);

    // noteDouble = track1.GetCurrentNoteSubtrack(0);
    // std::printf("dur: %f\tcom: %f\n", noteDouble.first.GetDurationSeconds(120), noteDouble.second);

    // std::printf("notesPlayed = {\n");
    // for (const auto& tuple : notesPlayed)
    // {
    //     const auto& note = std::get<0>(tuple);
    //     const auto& timeNotePlayed = std::get<1>(tuple);
    //     const auto& timeNoteStarted = std::get<2>(tuple);
    //     std::printf(
    //         "\tnotesPlayed: note<pitch=%u, intensity=%u>, timeplayed: %f, timestarted: %f\n",
    //         note.pitch, note.intensity, timeNotePlayed, timeNoteStarted
    //     );
    // }
    // std::printf("}\n");

    // track1.subtracks[0][0];

    // synth.tracks.push_back();

    SDL_CloseAudioDevice(devid);
    
    SDL_Quit();
    return 0;
}