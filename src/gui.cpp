#include "httplib/httplib.h"
#include <format>
#include <globals.hpp>
#include <gui.hpp>
#include <imgui/imgui.h>
#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXUserAgent.h>
#include <ixwebsocket/IXWebSocket.h>
#include <settings.hpp>

bool tmp_open = true;
void render_window()
{
    ImGui::SetNextWindowPos(ImVec2(300, 400), ImGuiCond_FirstUseEver);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
    if (ImGui::Begin("Chat##AlternateChat", &tmp_open, flags)) {
        ImGui::Text("Hello from template!");
        ImGui::End();
    }
}

// TODO: join, update, leave, delete channel in settings
#define CHANNEL_NAME_LENGTH 257
char channel_name[CHANNEL_NAME_LENGTH];
#define CHANNEL_CODE_LENGTH 129
char channel_code[CHANNEL_CODE_LENGTH];
#define USERNAME_LENGTH 65
char username[USERNAME_LENGTH];
ix::WebSocket web_socket;
void render_options()
{
    ImGui::Text("Create channel");
    ImGui::InputText("Channel Name##AlternateChatChannelName", reinterpret_cast<char *>(&channel_name),
                     CHANNEL_NAME_LENGTH);
    ImGui::SameLine();
    ImGui::InputText("Username##AlternateChatUsernameCreate", reinterpret_cast<char *>(&username), USERNAME_LENGTH);
    if (ImGui::Button("Create##AlternateChatCreate")) {
        httplib::Client cli("http://localhost:3000");
        nlohmann::json body_json;
        body_json["name"] = channel_name;
        auto res = cli.Post("/channels", body_json.dump(), "application/json");
        if (res->status == 201) {
            nlohmann::json json = nlohmann::json::parse(res->body);
            std::string id = json.value("id", "");
            std::string join_code = json.value("joinCode", "");
            std::string update_code = json.value("updateCode", "");
            std::string delete_code = json.value("deleteCode", "");
            Settings::channels.emplace_back(id, join_code, update_code, delete_code);
            Settings::json_settings["Channels"] = Settings::channels;
            Settings::save(Settings::settings_path);
            memset(channel_name, 0, CHANNEL_NAME_LENGTH);
            api->Log(ELogLevel_DEBUG, addon_name, "connecting to WS");
            std::string url("ws://localhost:3001");
            web_socket.setUrl(url);
            web_socket.setOnMessageCallback(
                [](const ix::WebSocketMessagePtr &msg)
                {
                    if (msg->type == ix::WebSocketMessageType::Message) {
                        std::cout << "received message: " << msg->str << std::endl;
                        std::cout << "> " << std::flush;
                    } else if (msg->type == ix::WebSocketMessageType::Open) {
                        std::cout << "Connection established" << std::endl;
                        std::cout << "> " << std::flush;
                    } else if (msg->type == ix::WebSocketMessageType::Error) {
                        std::cout << "Connection error: " << msg->errorInfo.reason << std::endl;
                        std::cout << "> " << std::flush;
                    }
                });
            web_socket.start();
        }
    }
    ImGui::Separator();
    ImGui::Text("Join channel");
    ImGui::InputText("Code##AlternateChatCode", reinterpret_cast<char *>(&channel_code), CHANNEL_CODE_LENGTH);
    ImGui::SameLine();
    ImGui::InputText("Username##AlternateChatUsernameJoin", reinterpret_cast<char *>(&username), USERNAME_LENGTH);
    if (ImGui::Button("Paste Code from Clipboard##AlternateChatPasteCode")) {
        //TODO: get code from clipboard and put it in `channel_code`
    }
    ImGui::SameLine();
    if (ImGui::Button("Join##AlternateChatJoin")) {
        httplib::Client cli("http://localhost:3000");
        nlohmann::json body_json;
        body_json["joinCode"] = channel_code;
        auto res = cli.Post(std::format("/channels/join", body_json.dump(), "application/json"));
        if (res && res->status == 200) {
            api->Log(ELogLevel_DEBUG, addon_name, "connecting to WS");
            std::string url("ws://localhost:3001");
            web_socket.setUrl(url);
            web_socket.setOnMessageCallback(
                [](const ix::WebSocketMessagePtr &msg)
                {
                    if (msg->type == ix::WebSocketMessageType::Message) {
                        std::cout << "received message: " << msg->str << std::endl;
                        std::cout << "> " << std::flush;
                    } else if (msg->type == ix::WebSocketMessageType::Open) {
                        std::cout << "Connection established" << std::endl;
                        std::cout << "> " << std::flush;
                    } else if (msg->type == ix::WebSocketMessageType::Error) {
                        std::cout << "Connection error: " << msg->errorInfo.reason << std::endl;
                        std::cout << "> " << std::flush;
                    }
                });
            web_socket.start();
            // Channel exists, now join via WebSocket
            // 1. Connect to WS
            // 2. Send join message { type: "join", joinCode: joinCode }
            // 3. Receive ephemeral ID from server
            // 4. Optionally, ask user for a temporary "display name" for this channel
            //    (for UI only, not tied to global identity)
        } else {
            api->Log(ELogLevel_WARNING, addon_name, "Channel does not exist!");
        }
        memset(channel_code, 0, CHANNEL_CODE_LENGTH);
    }
    // TODO: list channels
    ImGui::Separator();
    for (auto &channel : Settings::channels) {
        ImGui::Text("%s", channel.id.c_str());
        ImGui::SameLine();
        //TODO: add button to copy joinCode
        if (ImGui::Button(std::format("Leave##AlternateChat{}", channel.id).c_str())) {
            api->Log(ELogLevel_DEBUG, addon_name, channel.delete_code.c_str());
            httplib::Client cli("http://localhost:3000");
            if (!channel.delete_code.empty()) {
                nlohmann::json body_json;
                body_json["deleteCode"] = channel.delete_code;
                std::string body_str = body_json.dump();
                auto res = cli.Delete(std::format("/channels/{}", channel.id), body_json.dump(), "application/json");
                if (res->status == 200) {
                    Settings::channels.erase(std::remove_if(Settings::channels.begin(), Settings::channels.end(),
                                                            [&](const auto &c) { return c.id == channel.id; }),
                                             Settings::channels.end());
                }
            } else {
                auto res = cli.Get(std::format("/channels/{}/leave", channel.id));
                if (res->status == 200) {
                    Settings::channels.erase(std::remove_if(Settings::channels.begin(), Settings::channels.end(),
                                                            [&](const auto &c) { return c.id == channel.id; }),
                                             Settings::channels.end());
                }
            }
            Settings::save(Settings::settings_path);
        }
    }
}
