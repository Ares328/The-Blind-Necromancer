#include <iostream>
#include <string>

#include "httplib.h"
#include "Game.h"
#include "CommandSerialization.h"

using namespace NecroCore;

std::unordered_map<std::string, std::unique_ptr<Game>> g_sessions;

std::string GenerateSessionId()
{
    static std::mt19937_64 rng{ std::random_device{}() };
    static const char chars[] =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    std::uniform_int_distribution<> pick(0, static_cast<int>(sizeof(chars) - 2));

    std::string id;
    id.reserve(16);
    for (int i = 0; i < 16; ++i)
    {
        id.push_back(chars[pick(rng)]);
    }
    return id;
}

Game* GetOrNull(const std::string& sessionId)
{
    auto it = g_sessions.find(sessionId);
    if (it == g_sessions.end())
        return nullptr;
    return it->second.get();
}

int main()
{
    httplib::Server svr;

    svr.set_pre_routing_handler([](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        if (req.method == "OPTIONS")
        {
            res.status = 204;
            return httplib::Server::HandlerResponse::Handled;
        }
        return httplib::Server::HandlerResponse::Unhandled;
        });

    svr.Get("/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(R"({"status":"ok"})", "application/json");
        });

    svr.Post("/new-game", [](const httplib::Request& req, httplib::Response& res) {
        const std::string& body = req.body;
        std::string playerName;
        std::string mapName;

        auto extractField = [&](const std::string& key, std::string& out) {
            const std::string pattern = "\"" + key + "\":\"";
            auto pos = body.find(pattern);
            if (pos == std::string::npos) return;
            pos += pattern.size();
            auto end = body.find("\"", pos);
            if (end == std::string::npos) return;
            out = body.substr(pos, end - pos);
            };

        extractField("playerName", playerName);
        extractField("mapName", mapName);

        if (playerName.empty())
        {
            playerName = "TestPlayer123";
        }
        if (mapName.empty())
        {
            mapName = "map1";
        }

        std::string sessionId = GenerateSessionId();
        g_sessions[sessionId] = std::make_unique<Game>(playerName, mapName);

        std::string json = R"({"sessionId":")" + sessionId + R"("})";
        res.set_content(json, "application/json");
        });

    svr.Post("/turn", [](const httplib::Request& req, httplib::Response& res) {
        const std::string& body = req.body;
        std::string command;
        std::string sessionId;

        auto extractField = [&](const std::string& key, std::string& out) {
            const std::string pattern = "\"" + key + "\":\"";
            auto pos = body.find(pattern);
            if (pos == std::string::npos) return;
            pos += pattern.size();
            auto end = body.find("\"", pos);
            if (end == std::string::npos) return;
            out = body.substr(pos, end - pos);
            };

        extractField("sessionId", sessionId);
        extractField("command", command);

        if (sessionId.empty())
        {
            res.status = 400;
            res.set_content(R"({"error":"Missing 'sessionId' field"})", "application/json");
            return;
        }
        if (command.empty())
        {
            res.status = 400;
            res.set_content(R"({"error":"Missing 'command' field"})", "application/json");
            return;
        }

        Game* game = GetOrNull(sessionId);
        if (!game)
        {
            res.status = 404;
            res.set_content(R"({"error":"Unknown sessionId"})", "application/json");
            return;
        }

        CommandResult result = game->ApplyTurn(command);
        std::string json = CommandResultToJson(result);

        res.set_content(json, "application/json");
    });

    std::cout << "Server listening on http://localhost:8080\n";
    svr.listen("0.0.0.0", 8080);

    return 0;
}