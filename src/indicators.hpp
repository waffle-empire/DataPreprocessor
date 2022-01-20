#pragma once
#include "common.hpp"
#include "candle.hpp"

namespace program
{
    class indicators final
    {
        std::vector<std::unique_ptr<candle>>* m_candles;

        double* m_open;
        double* m_close;
        double* m_high;
        double* m_low;
        double* m_volume;

        const size_t m_alloc_size;

    public:
        indicators(std::vector<std::unique_ptr<candle>>* candles)
            : m_candles(candles), m_alloc_size(candles->size())
        {
            m_open = new double[m_alloc_size];
            m_close = new double[m_alloc_size];
            m_high = new double[m_alloc_size];
            m_low = new double[m_alloc_size];
            m_volume = new double[m_alloc_size];
        }

        virtual ~indicators()
        {
            delete[] m_open;
            delete[] m_close;
            delete[] m_high;
            delete[] m_low;
            delete[] m_volume;
        }

        void calculate_indicators()
        {
            this->calculate_adosc();
            this->calculate_atr();
            this->calculate_bollinger_bands();
            this->calculate_macd();
            this->calculate_mfi();
            this->calculate_rsi();
        }
        
    private:
        void calculate_adosc(const size_t fast_period = 24, const size_t slow_period = 45)
        {
            double* tmp_adosc = new double[m_alloc_size];

            int beginIdx, endIdx;
            TA_ADOSC(0, m_alloc_size, m_high, m_low, m_close, m_volume, fast_period, slow_period, &beginIdx, &endIdx, tmp_adosc);

            for (size_t i = beginIdx; i < endIdx; i++)
                m_candles->at(i)->m_adosc = tmp_adosc[i];

            delete[] tmp_adosc;
        }

        void calculate_atr(const size_t period_range = 24)
        {
            double* tmp_atr = new double[m_alloc_size];

            int beginIdx, endIdx;
            TA_ATR(0, m_alloc_size, m_high, m_low, m_close, period_range, &beginIdx, &endIdx, tmp_atr);

            for (size_t i = beginIdx; i < endIdx; i++)
                m_candles->at(i)->m_atr = tmp_atr[i];

            delete[] tmp_atr;
        }

        void calculate_bollinger_bands(const size_t period_range = 20, const size_t optInNbDevUp = 2, const size_t optInNbDevDown = 2)
        {
            // optInNbDevUp & optInNbDevDown = standard deviation for upper and lower band, usually 2 is used
            double* tmp_upper_band = new double[m_alloc_size];
            double* tmp_middle_band = new double[m_alloc_size];
            double* tmp_lower_band = new double[m_alloc_size];

            int beginIdx, endIdx;
            TA_MAType MAtype = TA_MAType_SMA;
            TA_BBANDS(0, m_alloc_size, m_close, period_range, optInNbDevUp, optInNbDevDown, MAtype, &beginIdx, &endIdx, tmp_upper_band, tmp_middle_band, tmp_lower_band);

            for (size_t i = beginIdx; i < endIdx; i++)
            {
                const std::unique_ptr<candle>& candle = m_candles->at(i);

                candle->m_upper_band = tmp_upper_band[i];
                candle->m_middle_band = tmp_middle_band[i];
                candle->m_lower_band = tmp_lower_band[i];
            }

            delete[] tmp_upper_band;
            delete[] tmp_middle_band;
            delete[] tmp_lower_band;
        }

        void calculate_macd(const size_t fast_period = 12, const size_t slow_period = 26, const size_t signal_period = 9)
        {
            double* tmp_macd = new double[m_alloc_size];
            double* tmp_macd_signal = new double[m_alloc_size];
            double* tmp_macd_hist = new double[m_alloc_size];

            int beginIdx, endIdx;
            TA_MACD(0, m_alloc_size, m_close, fast_period, slow_period, signal_period, &beginIdx, &endIdx, tmp_macd, tmp_macd_signal, tmp_macd_hist);

            for (size_t i = beginIdx; i < endIdx; i++)
            {
                const std::unique_ptr<candle>& candle = m_candles->at(i);

                candle->m_macd = tmp_macd[i];
                candle->m_macd_signal = tmp_macd_signal[i];
                candle->m_macd_hist = tmp_macd_hist[i];
            }

            delete[] tmp_macd;
            delete[] tmp_macd_signal;
            delete[] tmp_macd_hist;
        }

        void calculate_mfi(const size_t period_range = 30)
        {
            double* tmp_mfi = new double[m_alloc_size];

            int beginIdx, endIdx;
            TA_MFI(0, m_alloc_size, m_high, m_low, m_close, m_volume, period_range, &beginIdx, &endIdx, tmp_mfi);

            for (size_t i = beginIdx; i < endIdx; i++)
                m_candles->at(i)->m_mfi = tmp_mfi[i];

            delete[] tmp_mfi;
        }

        void calculate_rsi(const size_t period_range = 14)
        {
            double* tmp_rsi = new double[m_alloc_size];

            int beginIdx, endIdx;
            TA_RSI(0, m_alloc_size, m_close, period_range, &beginIdx, &endIdx, tmp_rsi);

            for (size_t i = beginIdx; i < endIdx; i++)
                m_candles->at(i)->m_rsi = tmp_rsi[i];

            delete[] tmp_rsi;
        }
    }; 
}