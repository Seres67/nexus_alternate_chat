#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <mutex>
#include <nlohmann/json.hpp>

struct Channel
{
    std::string id;
    std::string join_code;
    std::string update_code;
    std::string delete_code;
};
void from_json(const nlohmann::json &j, Channel &s);
void to_json(nlohmann::json &j, const Channel &s);
namespace Settings
{

void load(const std::filesystem::path &path);
void save(const std::filesystem::path &path);

extern nlohmann::json json_settings;
extern std::filesystem::path settings_path;
extern std::mutex mutex;

extern std::vector<Channel> channels;
} // namespace Settings

#endif // SETTINGS_HPP
