#include "common.hpp"
#include "logger.hpp"
#include "processing/preprocessor.hpp"
#include "settings.hpp"
#include "thread_pool.hpp"

using namespace program;

/**
 * @param {number} argc Argument count
 * @param {Array<char*>} argv Array of arguments
 */
int main(int argc, const char** argv)
{
    std::unique_ptr<Logger> logger_instance = std::make_unique<Logger>();

#ifdef RELEASE
    g_log->set_log_level(LogLevel::Info);
#endif

    g_log->info("MAIN", "Reading settings file.");
    std::unique_ptr<settings> settings_instance = std::make_unique<settings>();
    settings_instance->load();

    const char* input_folder;
    const char* output_folder;

    if (argc < 3)
    {
        input_folder = g_settings->input.input_folder.c_str();
        output_folder = g_settings->output.output_folder.c_str();

        g_log->info("MAIN", "No input or output folder arguments given, falling back to settings.json.");
    }
    else
    {
        input_folder = argv[1];
        output_folder = argv[2];
    }
    
    if (
        !std::filesystem::exists(input_folder) || !std::filesystem::exists(output_folder) ||
        !std::filesystem::is_directory(input_folder) || !std::filesystem::is_directory(output_folder)
        )
    {
        g_log->error("MAIN", "Input and/or output directories don't exist.");

        return 1;
    }

    g_log->info("MAIN", "Initiating thread pool.");
    auto thread_pool_instance = std::make_unique<thread_pool>();

    try
    {
        if (TA_RetCode code = TA_Initialize(); code != TA_SUCCESS)
        {
            throw std::runtime_error("TA_Initialize did not return TA_SUCCESS.");
        }
    }
    catch(const std::exception& e)
    {
        TA_Shutdown();

        g_log->error("MAIN", "Error occurred while called TA_Initialize:\n%s", e.what());

        return 1;
    }

    std::chrono::time_point start_time = std::chrono::system_clock::now();
    g_log->info("MAIN", "Starting parsing of files...");
    for (const auto item : std::filesystem::directory_iterator(input_folder))
    {
        if (item.is_regular_file())
        {
            g_thread_pool->push([item, output_folder]()
            {
                g_log->verbose("THREAD", "Processing file: %s", item.path().c_str());

                preprocessor processor(item, output_folder);
                processor.start();
            });
        }
    }

    while (thread_pool_instance->has_jobs())
    {
        std::this_thread::sleep_for(500ms);
    }

    std::chrono::duration seconds = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start_time);
    std::chrono::duration minutes = std::chrono::duration_cast<std::chrono::minutes>(seconds);
    seconds -= minutes;
    g_log->info("MAIN", "Finished processing files in %dmin, %dsecs",
        minutes,
        seconds
    );

    g_log->info("MAIN", "Waiting for all threads to exit...");
    thread_pool_instance->destroy();
    thread_pool_instance.reset();

    settings_instance.reset();

    g_log->info("MAIN", "Farewell!");

    logger_instance.reset();

    return 0;
}
