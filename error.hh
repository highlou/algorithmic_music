#ifndef __ALGORITHMIC_MUSIC_ERROR_HH__
#define __ALGORITHMIC_MUSIC_ERROR_HH__

#include <string>
#include <cstdint>

namespace syn
{
    enum ErrorStatus : std::uint8_t
    {
        ES_SUCCESS = 0,
        ES_N_SUBTRACKS_NOT_CONSISTENT,
        ES_FIRST_SUBTRACK_N_MEASURE_ZERO,
        ES_SUBTRACK_N_MEASURES_NOT_CONSISTENT,
        ES_MEASURE_LENGTH_NOT_TIME_SIGNATURE
    };
    
    std::string GetStringErrorStatus(ErrorStatus ss);
}

#endif // __ALGORITHMIC_MUSIC_ERROR_HH__