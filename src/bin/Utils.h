#pragma once
#include <unordered_set>
inline size_t strl(const char* a_str)
{
	if (a_str == nullptr)
		return 0;
	size_t len = 0;
	while (a_str[len] != '\0' && len < 0x7FFFFFFF) {
		len++;
	}
	return len;
}

namespace Utils
{
	namespace imgui
	{

		void HoverNote(const char* text, const char* note = "*");
	}


	template <class T>
	void loadUsefulPlugins(std::unordered_set<RE::TESFile*>& mods)
	{
		auto data = RE::TESDataHandler::GetSingleton();
		for (auto form : data->GetFormArray<T>()) {
			if (form) {
				if (!mods.contains(form->GetFile())) {
					mods.insert(form->GetFile());
				}
			}
		}
	};

	using _GetFormEditorID = const char* (*)(std::uint32_t);
	
	std::string getFormEditorID(const RE::TESForm* a_form);
}

/*Helper class to load from a simple ini file.*/
class settingsLoader
{
private:
	CSimpleIniA _ini;
	const char* _section;
	int _loadedSettings;
	int _savedSettings;
	const char* _settingsFile;

public:
	settingsLoader(const char* settingsFile);
	~settingsLoader();
	/*Set the active section. Load() will load keys from this section.*/
	void setActiveSection(const char* section);
	/*Load a boolean value if present.*/
	void load(bool& settingRef, const char* key);
	/*Load a float value if present.*/
	void load(float& settingRef, const char* key);
	/*Load an unsigned int value if present.*/
	void load(uint32_t& settingRef, const char* key);

	void save(bool& settingRef, const char* key);
	void save(float& settingRef, const char* key);
	void save(uint32_t& settingRef, const char* key);

	void flush();

	/*Load an integer value if present.*/
	void load(int& settingRef, const char* key);

	void log()
	{
		logger::info("Loaded {} settings, saved {} settings from {}.", _loadedSettings, _savedSettings, _settingsFile);
	}
};


namespace ImGui
{
	bool SliderFloatWithSteps(const char* label, float* v, float v_min, float v_max, float v_step);
	void HoverNote(const char* text, const char* note = "(?)");

	bool ToggleButton(const char* str_id, bool* v);

	bool InputTextRequired(const char* label, std::string* str, ImGuiInputTextFlags flags = 0);
	bool InputTextWithPaste(const char* label, std::string& text, const ImVec2& size = ImVec2(0, 0), bool multiline = false, ImGuiInputTextFlags flags = 0);
	bool InputTextWithPasteRequired(const char* label, std::string& text, const ImVec2& size = ImVec2(0, 0), bool multiline = false, ImGuiInputTextFlags flags = 0);

}
