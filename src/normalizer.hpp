#pragma once
#include "common.hpp"
#include "candle.hpp"

namespace program::normalizer
{
    inline void clone_candle(candle* dst, candle* src)
    {
        memcpy(dst, src, sizeof(candle));
    }

    inline void normalize_candles(std::vector<std::unique_ptr<candle>>* candles)
    {
        candle curr_candle_backup, previous_candle;
        for (size_t i = 0; i < candles->size(); i++)
        {
            std::unique_ptr<candle>& curr_candle = candles->at(i);
            normalizer::clone_candle(&curr_candle_backup, curr_candle.get());

            if (i)
                curr_candle->normalize(&previous_candle);

            normalizer::clone_candle(&previous_candle, &curr_candle_backup);
        }
        candles->erase(candles->begin());
    }
}