#include "Localization.h"

#include <filesystem>
#include <fstream>
#include <map>
#include <mutex>
#include <unordered_map>

#include <rapidjson/document.h>

namespace Localization
{
	static std::mutex s_mutex;
	static bool s_loaded = false;
	static bool s_failed = false;
	static std::unordered_map<std::string, std::string> s_translations;
	static std::map<std::string, std::string> s_cache;

	void Load()
	{
		std::lock_guard<std::mutex> lock(s_mutex);
		if (s_loaded || s_failed) {
			return;
		}

		// Get DLL path to locate <DLLName>.json
		REX::W32::HMODULE dllHandle = REX::W32::GetModuleHandleW(L"OpenAnimationReplacer.dll");
		if (!dllHandle) {
			logger::warn("Could not get DLL handle for translations"sv);
			s_failed = true;
			return;
		}

		wchar_t dllPath[MAX_PATH];
		if (REX::W32::GetModuleFileNameW(dllHandle, dllPath, MAX_PATH) == 0) {
			logger::warn("Could not get DLL path for translations"sv);
			s_failed = true;
			return;
		}

		std::filesystem::path jsonPath = std::filesystem::path(dllPath).replace_extension(L".json");

		if (!std::filesystem::exists(jsonPath)) {
			logger::warn("Translation file not found: {}"sv, jsonPath.string());
			s_failed = true;
			return;
		}

		// Read JSON file
		std::ifstream file(jsonPath, std::ios::binary);
		if (!file.is_open()) {
			logger::warn("Could not open translation file: {}"sv, jsonPath.string());
			s_failed = true;
			return;
		}

		std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		file.close();

		// Strip UTF-8 BOM if present
		const char* jsonData = content.c_str();
		if (content.size() >= 3 && static_cast<unsigned char>(jsonData[0]) == 0xEF &&
			static_cast<unsigned char>(jsonData[1]) == 0xBB && static_cast<unsigned char>(jsonData[2]) == 0xBF) {
			jsonData += 3;
		}

		rapidjson::Document doc;
		doc.Parse(jsonData);

		if (doc.HasParseError()) {
			logger::error("JSON parse error in: {}"sv, jsonPath.string());
			s_failed = true;
			return;
		}

		if (!doc.IsObject()) {
			logger::error("Translation JSON is not an object"sv);
			s_failed = true;
			return;
		}

		for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it) {
			if (it->name.IsString() && it->value.IsString()) {
				s_translations[it->name.GetString()] = it->value.GetString();
			}
		}

		s_loaded = true;
		logger::info("Loaded {} translations from {}"sv, s_translations.size(), jsonPath.string());
	}

	const char* Translate(const char* a_key)
	{
		Load();

		std::lock_guard<std::mutex> lock(s_mutex);

		std::string key(a_key);

		auto it = s_cache.find(key);
		if (it != s_cache.end()) {
			return it->second.c_str();
		}

		auto transIt = s_translations.find(key);
		if (transIt != s_translations.end()) {
			auto result = s_cache.emplace(key, transIt->second);
			return result.first->second.c_str();
		}

		// Not found: cache the original key
		auto result = s_cache.emplace(key, key);
		return result.first->second.c_str();
	}
}
