#include "settings.hpp"
#include "nlohmann/json.hpp"
#include <filesystem>
#include <fstream>
#include <globals.hpp>

#include <nexus/Nexus.h>

using json = nlohmann::json;

void from_json(const nlohmann::json &j, Channel &s)
{
    s.id = j.at("id").get<std::string>();
    s.join_code = j.at("join_code").get<std::string>();
    s.update_code = j.at("update_code").get<std::string>();
    s.delete_code = j.at("delete_code").get<std::string>();
}

void to_json(nlohmann::json &j, const Channel &s)
{
    j = json{
        {"id", s.id},
        {"join_code", s.join_code},
        {"update_code", s.update_code},
        {"delete_code", s.delete_code},
    };
}

namespace Settings
{
json json_settings;
std::mutex mutex;
std::filesystem::path settings_path;
std::vector<Channel> channels;

void load(const std::filesystem::path &path)
{
    json_settings = json::object();
    if (!std::filesystem::exists(path)) {
        return;
    }

    {
        std::lock_guard lock(mutex);
        try {
            if (std::ifstream file(path); file.is_open()) {
                json_settings = json::parse(file);
                file.close();
            }
        } catch (json::parse_error &ex) {
            api->Log(ELogLevel_WARNING, addon_name, "settings.json could not be parsed.");
            api->Log(ELogLevel_WARNING, addon_name, ex.what());
        }
    }
    if (!json_settings["Channels"].is_null())
        json_settings["Channels"].get_to(channels);
    api->Log(ELogLevel_INFO, addon_name, "settings loaded!");
}

void save(const std::filesystem::path &path)
{
    if (!std::filesystem::exists(path.parent_path())) {
        std::filesystem::create_directories(path.parent_path());
    }
    {
        std::lock_guard lock(mutex);
        if (std::ofstream file(path); file.is_open()) {
            file << json_settings.dump(1, '\t') << std::endl;
            file.close();
        }
        api->Log(ELogLevel_INFO, addon_name, "settings saved!");
    }
}
} // namespace Settings
