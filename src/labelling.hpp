#pragma once
#include "common.hpp"
#include "candle.hpp"

namespace program::labelling
{
    inline void label_data(std::vector<std::unique_ptr<candle>>* candles)
    {
        std::vector<double> cum_down;
        std::vector<double> cum_up;

        double last_max = 0;
        bool allow_sell = false;
        int last_max_index = 0;

        double last_min = 0;
        bool allow_buy = false;
        int last_min_index = 0;

        for (size_t i = 0; i < candles->size(); i++)
        {
            std::unique_ptr<candle>& candle = candles->at(i);

            if (cum_up.size() && cum_down.size())
            {
                double prev_up = cum_up[cum_up.size() - 1];
                double prev_down = cum_down[cum_down.size() - 1];

                if (candle->m_close > 0)
                {
                    if (prev_up - prev_down > 0.01)
                    {
                        if (prev_up - prev_down > last_max)
                        {
                            last_max_index = i;
                            last_max = prev_up - prev_down;
                            allow_sell = true;
                        }

                        if (allow_buy)
                        {
                            // set sell at last_max
                            candles->at(last_min_index)->m_target = eTarget::BUY;
                            last_min_index = 0;
                            last_min = 0;
                            allow_buy = false;
                        }

                        cum_up.clear();
                        cum_down.clear();
                    }
                }
                else if (candle->m_close < 0)
                {
                    if (prev_down - prev_up > 0.01)
                    {
                        if (allow_sell)
                        {
                            // set sell at last_max
                            candles->at(last_max_index)->m_target = eTarget::SELL;
                            last_max_index = 0;
                            last_max = 0;
                            allow_sell = false;
                        }

                        if (prev_down - prev_up > last_min)
                        {
                            last_min_index = i;
                            last_max = prev_down - prev_up;
                            allow_buy = true;
                        }

                        // set buy and sell

                        cum_up.clear();
                        cum_down.clear();
                    }

                }
            }

            if (candle->m_close > 0)
            {
                double prev = cum_up.size() ? cum_up[cum_up.size() - 1] : 0;

                cum_up.push_back(prev + candle->m_close);
            }
            else
            {
                double prev = cum_down.size() ? cum_down[cum_down.size() - 1] : 0;

                cum_down.push_back(prev + abs(candle->m_close));
            }
        }
    }
}