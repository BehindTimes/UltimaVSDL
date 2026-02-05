#pragma once

#include <vector>
#include <string>
#include "UltimaVResource.h"
#include <filesystem>
#include <string_view>

class GameOptions
{
public:
	explicit GameOptions();

	int Initialize();
	int ParseOptions(std::vector<std::string>& vec_options);

	std::filesystem::path m_game_directory;
	bool m_smooth_scroll;
	RenderMode m_render_mode;

private:
	int CreateOptions();
	bool m_write_options;

	const std::string_view OPTIONS_FILE = "U5Options.ini";
};

