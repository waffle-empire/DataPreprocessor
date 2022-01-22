#pragma once
#include "common.hpp"

enum eTarget : int64_t
{
    NONE,
    BUY,
    SELL
};

struct candle
{
    //std::string m_timestamp;
    uint64_t m_timestamp;
    double m_open;
    double m_close;
    double m_high;
    double m_low;
    double m_volume;

    // indicators
    double m_adosc;

    double m_atr;

    double m_macd;
    double m_macd_signal;
    double m_macd_hist;

    double m_mfi;

    double m_upper_band;
    double m_middle_band;
    double m_lower_band;

    double m_rsi;

    double m_difference_lowhigh;
    double m_difference_openclose;

    eTarget m_target = eTarget::NONE;

    candle() = default;
    candle(double timestamp, double open, double close, double high, double low, double volume)
    {
        m_timestamp = (uint64_t)timestamp;
        m_open = open;
        m_close = close;
        m_high = high;
        m_low = low;
        m_volume = volume;
    }

    void normalize(candle* other)
    {
        // don't update timestamp since we're on the newest candle of the two
        auto calc = [](double a1, double a2) { return a1 == 0 ? a1 : (a2 - a1) / a1; };

        this->m_open = calc(this->m_open, other->m_open);
        this->m_close = calc(this->m_close, other->m_close);
        this->m_high = calc(this->m_high, other->m_high);
        this->m_low = calc(this->m_low, other->m_low);
        this->m_volume = calc(this->m_volume, other->m_volume);

        this->m_adosc = calc(this->m_adosc, other->m_adosc);
        this->m_atr = calc(this->m_atr, other->m_atr);

        this->m_macd = calc(this->m_macd, other->m_macd);
        this->m_macd_hist = calc(this->m_macd_hist, other->m_macd_hist);
        this->m_macd_signal = calc(this->m_macd_signal, other->m_macd_signal);

        this->m_upper_band = calc(this->m_upper_band, other->m_upper_band);
        this->m_middle_band = calc(this->m_middle_band, other->m_middle_band);
        this->m_lower_band = calc(this->m_lower_band, other->m_lower_band);

        this->m_mfi /= 100;
        this->m_rsi /= 100;

        this->m_difference_lowhigh = calc(other->m_low, other->m_high);
        this->m_difference_openclose = calc(other->m_open, other->m_close);
    }
};
