#pragma once
#include "common.hpp"
#include "candle.hpp"
#include "indicators.hpp"
#include "labelling.hpp"
#include "normalizer.hpp"
#include "logger.hpp"

namespace program
{
    class preprocessor final
    {
    private:
        std::filesystem::path m_input_file;
        const char* m_out_dir;

        std::vector<std::unique_ptr<candle>> m_candles;

    public:
        preprocessor(std::filesystem::path file_path, const char* out_dir) :
            m_input_file(file_path), m_out_dir(out_dir)
        {

        }
        virtual ~preprocessor()
        {

        }

        const char* file_name()
        {
            return m_input_file.filename().c_str();
        }

        bool read_input_file()
        {
            io::CSVReader<6> input_stream(m_input_file);
            input_stream.read_header(io::ignore_missing_column | io::ignore_extra_column, "event_time", "open", "close", "high", "low", "volume");

            try
            {
                double timestamp, open, close, high, low, volume;
                while (input_stream.read_row(timestamp, open, close, high, low, volume))
                {
                    std::unique_ptr<candle> new_candle = std::make_unique<candle>(timestamp, open, close, high, low, volume);

                    m_candles.push_back(std::move(new_candle));
                }

            }
            catch(const std::exception& e)
            {
                g_log->error("SYMBOL_PROCESSOR", "Failure while reading csv:\n%s", e.what());

                return false;
            }

            g_log->verbose("SYMBOL_PROCESSOR", "Loaded %d candles from %s", m_candles.size(), this->file_name());

            return true;
        }

        void start()
        {
            this->read_input_file();

            indicators indicator_processor(&this->m_candles);
            indicator_processor.calculate_indicators();

            normalizer::normalize_candles(&this->m_candles);

            labelling::label_data(&this->m_candles);

            this->write_binary_out();
        }

        void write_binary_out()
        {
            std::string out_dir = m_out_dir / m_input_file.stem();
            std::ofstream output_stream(out_dir + ".bin", std::ios::binary | std::ios::trunc);

            for (const std::unique_ptr<candle>& candle_struct : m_candles)
                output_stream.write((char*)candle_struct.get(), sizeof(candle));

            output_stream.close();
        }
    };
}