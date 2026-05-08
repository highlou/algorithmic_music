#include "error.hh"

namespace syn
{
    std::string GetStringErrorStatus(ErrorStatus ss)
    {
        std::string s;
        switch (ss)
        {
            case ES_SUCCESS:
                s = "ES_SUCCESS";
                break;
            case ES_N_SUBTRACKS_NOT_CONSISTENT:
                s = "ES_N_SUBTRACKS_NOT_CONSISTENT";
                break;
            case ES_FIRST_SUBTRACK_N_MEASURE_ZERO:
                s = "ES_FIRST_SUBTRACK_N_MEASURE_ZERO";
                break;
            case ES_SUBTRACK_N_MEASURES_NOT_CONSISTENT:
                s = "ES_SUBTRACK_N_MEASURES_NOT_CONSISTENT";
                break;
            case ES_MEASURE_LENGTH_NOT_TIME_SIGNATURE:
                s = "ES_MEASURE_LENGTH_NOT_TIME_SIGNATURE";
                break;
            default:
                s = "Unknown error status " + std::to_string(ss);
                break;
        }

        return s;
    }
}