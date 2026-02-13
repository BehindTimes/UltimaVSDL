#include "GameOptions.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <stdexcept>
#include "UltimaVResource.h"

GameOptions::GameOptions() :
    m_write_options(false),
    m_smooth_scroll(true),
    m_console_smooth_scroll(true),
    m_render_mode(RenderMode::EGA),
    m_game_directory(std::filesystem::current_path())
{
}

int GameOptions::Initialize()
{
    std::string line;
    std::string options_file_str(OPTIONS_FILE);
    std::ifstream options_file(options_file_str);
    std::vector<std::string> vec_options;

    if (!options_file.is_open())
    {
        m_write_options = true;
    }

    // Read the file line by line and print to console
    while (std::getline(options_file, line))
    {
        vec_options.push_back(line);
    }

    options_file.close();

    ParseOptions(vec_options);

	return 0;
}

int GameOptions::ParseOptions(std::vector<std::string>& vec_options)
{
    std::map<std::string, bool> option_types = { {"directory", false}, {"smooth_scroll", false}, {"console_smooth_scroll", false},
        {"smooth_scroll", false}, {"graphics_type", false} };
    std::map<std::string, std::string> parsed_vals;
    // Loop through vector
    for (auto& curval : vec_options)
    {
        size_t position = curval.find_first_of('=');
        if (position != std::string::npos)
        {
            std::string strKey = curval.substr(0, position);
            std::string strVal = curval.substr(position + 1);
            if (strKey == "directory")
            {
                std::filesystem::path p{ strVal };
                if (std::filesystem::exists(p) && std::filesystem::is_directory(p))
                {
                    option_types["directory"] = true;
                    m_game_directory = p;
                }
            }
            else if (strKey == "smooth_scroll")
            {
                try
                {
                    std::size_t pos{};
                    const int i{ std::stoi(strVal, &pos) };
                    if (i == 0)
                    {
                        m_smooth_scroll = false;
                        option_types["smooth_scroll"] = true;
                    }
                    else if (i == 1)
                    {
                        m_smooth_scroll = true;
                        option_types["smooth_scroll"] = true;
                    }
                }
                catch (std::invalid_argument const& )
                {
                }
                catch (std::out_of_range const& )
                {
                }
            }
            else if (strKey == "console_smooth_scroll")
            {
                try
                {
                    std::size_t pos{};
                    const int i{ std::stoi(strVal, &pos) };
                    if (i == 0)
                    {
                        m_console_smooth_scroll = false;
                        option_types["console_smooth_scroll"] = true;
                    }
                    else if (i == 1)
                    {
                        m_console_smooth_scroll = true;
                        option_types["console_smooth_scroll"] = true;
                    }
                }
                catch (std::invalid_argument const&)
                {
                }
                catch (std::out_of_range const&)
                {
                }
            }
            else if (strKey == "graphics_type")
            {
                if (strVal == "EGA")
                {
                    m_render_mode = RenderMode::EGA;
                    option_types["graphics_type"] = true;
                }
                else if(strVal == "CGA")
                {
                    m_render_mode = RenderMode::CGA;
                    option_types["graphics_type"] = true;
                }
            }
        }
    }
    // Check to see if all options have been initialized.  If not, create the file
    for (auto& curval : option_types)
    {
        if (curval.second == false)
        {
            m_write_options = true;
            break;
        }
    }
    if (m_write_options)
    {
        return CreateOptions();
    }
    return 0;
}

int GameOptions::CreateOptions()
{
    std::string options_file_str(OPTIONS_FILE);
    std::ofstream options_file(options_file_str);

    if (!options_file.is_open())
    {
        return -1;
    }
    options_file << "directory=" << m_game_directory.string() << std::endl;
    options_file << "smooth_scroll=" << m_smooth_scroll << std::endl;
    options_file << "console_smooth_scroll=" << m_console_smooth_scroll << std::endl;
    options_file << "graphics_type=" << (m_render_mode == RenderMode::EGA ? "EGA" : "CGA") << std::endl;

    options_file.close();
    return 0;
}
