#pragma once

namespace Localization
{
	void Load();

	// returns translated string or the original if not found
	[[nodiscard]] const char* Translate(const char* a_key);
}
