#pragma once
#include <cinttypes>

struct unmodified_candle
{
    std::uint64_t m_timestamp;
    double m_open;
    double m_close;
    double m_high;
    double m_low;
    double m_volume;
};
