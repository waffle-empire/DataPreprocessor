#pragma once
#include "common.hpp"
#include "logger.hpp"

namespace program
{
	class settings;
	inline settings* g_settings{};

	class settings {
		nlohmann::json default_options;
		nlohmann::json options;

		struct input
        {
            std::string input_folder = "";
            bool is_binary = false;
        };

        struct output
        {
            std::string output_folder = "";
        };
        

	public:
		settings()
		{
			g_settings = this;
		}

		~settings()
		{
			g_settings = nullptr;
		}

		input input{};
        output output{};

		void from_json(const nlohmann::json& j)
		{
            this->input.input_folder = j["input"]["input_folder"];
			this->input.is_binary = j["input"]["is_binary"];

            this->output.output_folder = j["output"]["output_folder"];
		}

		nlohmann::json to_json()
		{
			return nlohmann::json{
				{
					"input", {
                        { "input_folder", this->input.input_folder },
						{ "is_binary", this->input.is_binary },
					}
				},
                {
                    "output", {
                        { "output_folder", this->output.output_folder }
                    }
                }
			};
		}

		void attempt_save()
		{
			auto j = this->to_json();

			if (deep_compare(this->options, j, true))
				this->save();
		}

		bool load()
		{
			this->default_options = this->to_json();

			std::ifstream file(this->settings_location);
			if (!file.is_open())
			{
				this->write_default_config();

				file.open(this->settings_location);
			}

			try
			{
				file >> this->options;
			}
			catch (const std::exception&)
			{
				g_log->warning("SETTINGS", "Detected corrupt settings, writing default config...");

				this->write_default_config();

				return this->load();
			}

			bool should_save = this->deep_compare(this->options, this->default_options);

			this->from_json(this->options);

			if (should_save)
			{
				g_log->verbose("SETTINGS", "Updating settings...");
				save();
			}

			return true;
		}

	private:
		const char* settings_location = "./settings.json";

		bool deep_compare(nlohmann::json& current_settings, const nlohmann::json& default_settings, bool compare_value = false)
		{
			bool should_save = false;

			for (auto& e : default_settings.items())
			{
				const std::string& key = e.key();

				if (current_settings.count(key) == 0 || (compare_value && current_settings[key] != e.value()))
				{
					current_settings[key] = e.value();

					should_save = true;
				}
				else if (current_settings[key].is_structured() && e.value().is_structured())
				{
					if (deep_compare(current_settings[key], e.value(), compare_value))
						should_save = true;
				}
				else if (!current_settings[key].is_structured() && e.value().is_structured()) {
					current_settings[key] = e.value();

					should_save = true;
				}
			}

			return should_save;
		}

		bool save()
		{
			std::ofstream file(this->settings_location, std::ios::out | std::ios::trunc);
			file << this->to_json().dump(4);
			file.close();

			return true;
		}

		bool write_default_config()
		{
			std::ofstream file(this->settings_location, std::ios::out);
			file << this->to_json().dump(4);
			file.close();

			return true;
		}
	};
}