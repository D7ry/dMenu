#include "imgui.h"
#include "imgui_internal.h"
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "bin/Utils.h"
#include "Trainer.h"

namespace World
{
	namespace Time
	{
		void show()
		{
			ImGui::Text("Time");
			auto calendar = RE::Calendar::GetSingleton();
			float* ptr = nullptr;
			if (calendar) {
				ptr = &(calendar->gameHour->value);
				if (ptr) {
					ImGui::SliderFloat("Time", ptr, 0.0f, 24.f);
					return;
				}
			}
			ImGui::Text("Time address not found");
		}

	}

	namespace Weather
	{
		// maps to store formEditorID as names for regions and weathers as they're discarded by bethesda
		std::unordered_map<RE::TESRegion*, std::string> _regionNames;
		std::unordered_map<RE::TESWeather*, std::string> _weatherNames;
		
		std::vector<RE::TESWeather*> _weathersToSelect;
		static std::vector<RE::TESFile*> _mods;  // plugins containing weathers
		static uint8_t _mods_i = 0;              // selected weather plugin
		bool _cached = false;
		
		bool _showCurrRegionOnly = true; // only show weather corresponding to current region
		

		static std::vector<std::pair<std::string, bool>> _filters = {
			{ "Pleasant", false },
			{ "Cloudy", false },
			{ "Rainy", false },
			{ "Snow", false },
			{ "Permanent aurora", false },
			{ "Aurora follows sun", false }
		};

		void cache()
		{
			_weathersToSelect.clear();
			for (auto weather : RE::TESDataHandler::GetSingleton()->GetFormArray<RE::TESWeather>()) {
				if (!weather) {
					continue;
				}
				auto flags = weather->data.flags;
				if (_filters[0].second) {
					if (!flags.any(RE::TESWeather::WeatherDataFlag::kPleasant)) {
						continue;
					}
				}
				if (_filters[1].second) {
					if (!flags.any(RE::TESWeather::WeatherDataFlag::kCloudy)) {
						continue;
					}
				}
				if (_filters[2].second) {
					if (!flags.any(RE::TESWeather::WeatherDataFlag::kRainy)) {
						continue;
					}
				}
				if (_filters[3].second) {
					if (!flags.any(RE::TESWeather::WeatherDataFlag::kSnow)) {
						continue;
					}
				}
				if (_filters[4].second) {
					if (!flags.any(RE::TESWeather::WeatherDataFlag::kPermAurora)) {
						continue;
					}
				}
				if (_filters[5].second) {
					if (!flags.any(RE::TESWeather::WeatherDataFlag::kAuroraFollowsSun)) {
						continue;
					}
				}
				_weathersToSelect.push_back(weather);
			}
		}
	

		void show()
		{
			RE::TESRegion* currRegion = nullptr;
			RE::TESWeather* currWeather = nullptr;
			if (RE::Sky::GetSingleton()) {
				currRegion = RE::Sky::GetSingleton()->region;
				currWeather = RE::Sky::GetSingleton()->currentWeather;
			}
			
			ImGui::Text("Weather");
			if (currRegion) {
				ImGui::Text("Current region: %s", _regionNames[currRegion]);
			}
	
			if (ImGui::BeginCombo("WeatherMod", _mods[_mods_i]->GetFilename().data())) {
				for (int i = 0; i < _mods.size(); i++) {
					bool isSelected = (_mods[_mods_i] == _mods[i]);
					if (ImGui::Selectable(_mods[i]->GetFilename().data(), isSelected)) {
						_mods_i = i;
						_cached = false;
					}
					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			ImGui::Text("Flags");
			for (int i = 0; i < _filters.size(); i++) {
				if (ImGui::Checkbox(_filters[i].first.c_str(), &_filters[i].second)) {
					_cached = false;
				}
				if (i < _filters.size() - 1) {
					ImGui::SameLine();
				}
			}
			
			if (ImGui::Checkbox("current region only", &_showCurrRegionOnly)) {
				_cached = false;
			}

			if (!_cached) {
				cache();
			}

			// list of weathers to choose
			if (ImGui::BeginCombo("Weathers", _weatherNames[currWeather].data())) {
				for (int i = 0; i < _weathersToSelect.size(); i++) {
					bool isSelected = (_weathersToSelect[i] == currWeather);
					if (ImGui::Selectable(_weatherNames[_weathersToSelect[i]].data(), isSelected)) {
						if (!isSelected) {							
							INFO("updating weather!");
							RE::Sky::GetSingleton()->currentWeather = _weathersToSelect[i];
						}
					}
					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			
		}

		void init()
		{
			_cached = false;
			// load list of plugins with available weathers
			std::unordered_set<RE::TESFile*> plugins;
			Utils::loadUsefulPlugins<RE::TESWeather>(plugins);
			for (auto plugin : plugins) {
				_mods.push_back(plugin);
			}
			
			auto data = RE::TESDataHandler::GetSingleton();
			// load region&weather names
			for (RE::TESWeather* weather : data->GetFormArray<RE::TESWeather>()) {
				_weatherNames.insert({ weather, Utils::getFormEditorID(weather) });
			}
			for (RE::TESRegion* region : data->GetFormArray<RE::TESRegion>()) {
				_regionNames.insert({ region, Utils::getFormEditorID(region) });
			}
		}
	}

	void show() 
	{
		if (!RE::Sky::GetSingleton() || !RE::Sky::GetSingleton()->currentWeather) {
			ImGui::Text("World not found");
		}
		Time::show();
		Weather::show();
	}

	void init()
	{
		Weather::init();
	}
}


void Trainer::show() 
{
	ImGui::PushID("World");
	if (ImGui::CollapsingHeader("World")) {
		World::show();
	}
	ImGui::PopID();
}

void Trainer::init()
{
	// Weather
	World::init();


	INFO("Trainer initialized.");
}