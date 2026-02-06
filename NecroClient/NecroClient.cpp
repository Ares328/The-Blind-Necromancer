#include <iostream>
#include <string>

#include "httplib.h"
#include "Game.h"
#include "CommandSerialization.h"

using namespace NecroCore;

int main()
{
    Game game("Ares328", "map1");

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

    svr.Post("/turn", [&game](const httplib::Request& req, httplib::Response& res) {
        std::string body = req.body;
        std::string command;

        const std::string key = "\"command\":\"";
        auto pos = body.find(key);
        if (pos != std::string::npos)
        {
            pos += key.size();
            auto end = body.find("\"", pos);
            if (end != std::string::npos)
            {
                command = body.substr(pos, end - pos);
            }
        }

        if (command.empty())
        {
            res.status = 400;
            res.set_content(R"({"error":"Missing 'command' field"})", "application/json");
            return;
        }

        CommandResult result = game.ApplyTurn(command);
        std::string json = CommandResultToJson(result);

        res.set_content(json, "application/json");
        });

    std::cout << "Server listening on http://localhost:8080\n";
    svr.listen("0.0.0.0", 8080);

    return 0;
}