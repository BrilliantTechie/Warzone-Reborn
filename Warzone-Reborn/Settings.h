#pragma once
#include "GUI.h"
#include "Main.hpp"
#include <filesystem>

namespace Settings
{
	// Menu
	extern bool bMenu;

	// Game Settings
	extern bool bIsInGame;
	extern int iMaxPlayerCount;
	
	class menu
	{
	public:
		explicit menu() = default;
		~menu() = default;

		nlohmann::json options;
		nlohmann::json defaultOptions =
			R"({
				"AimLock": false,
				"Crosshair": false,
				"FOV": false,
				"AimPrediction": false,
				"AimPoint": false,
				"DebugMode": false,
				"StanceCheck": false,
				"VisibleCheck": false,
				"AlliesCheck": false,
				"NoRecoil": false,
				"AimSmooth": 18,
				"MaxDistance": 300,
				"AimSpeed": 1.250,
				"BulletSpeed": 3670.0,
				"FOVSize": 40.0,
				"CrosshairSize": 10.0,
				"AimPointSize": 4.0
			})"_json;

		void Save()
		{
			std::string settings_file = std::getenv("appdata");
			settings_file += "\\WarzoneReloaded\\settings.json";

			std::ofstream file(settings_file, std::ios::out | std::ios::trunc);
			file << options.dump(4);
			file.close();
		}

		void Load()
		{
			std::filesystem::path m_file_path = std::getenv("appdata");
			m_file_path /= "WarzoneReloaded";
			if (!std::filesystem::exists(m_file_path)) {
				std::filesystem::create_directory(m_file_path);
			} else if (!std::filesystem::is_directory(m_file_path)) {
				std::filesystem::remove(m_file_path);
				std::filesystem::create_directory(m_file_path);
			}

			std::string settings_file = std::getenv("appdata");
			settings_file += "\\WarzoneReloaded\\settings.json";
			std::ifstream file(settings_file);

			if (!file.is_open()) {
				WriteDefaultConfig();
				file.open(settings_file);
			}

			file >> options;

			bool bShouldSave = false;
			for (auto& e : defaultOptions.items())
			{
				if (options.count(e.key()) == 0)
				{
					bShouldSave = true;
					options[e.key()] = e.value();
				}
			}

			GUI::Log("Loaded settings.");

			if (bShouldSave)
			{
				GUI::Log("Updated settings.");
				Save();
			}
		}

	private:
		void WriteDefaultConfig() {
			std::string settings_file = std::getenv("appdata");
			settings_file += "\\WarzoneReloaded\\settings.json";

			std::ofstream file(settings_file, std::ios::out);
			file << defaultOptions.dump(4);
			file.close();

			options.clear();
			options = defaultOptions;
		}
	};

	inline menu g_menuSettings;

}