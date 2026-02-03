#pragma once

#include "Command.h"
#include "PulseResult.h"
#include "MoveResult.h"
#include "SummonResult.h"

#include <string>
#include <sstream>
#include <variant>

namespace NecroCore
{
    inline std::string EscapeJsonString(const std::string& input)
    {
        std::ostringstream oss;
        for (char c : input)
        {
            switch (c)
            {
            case '\"': oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            case '\b': oss << "\\b";  break;
            case '\f': oss << "\\f";  break;
            case '\n': oss << "\\n";  break;
            case '\r': oss << "\\r";  break;
            case '\t': oss << "\\t";  break;
            default:
                if (static_cast<unsigned char>(c) < 0x20)
                {
                    oss << "\\u"
                        << std::hex
                        << std::uppercase
                        << static_cast<int>(static_cast<unsigned char>(c));
                }
                else
                {
                    oss << c;
                }
                break;
            }
        }
        return oss.str();
    }

    inline std::string CommandActionToString(CommandAction action)
    {
        switch (action)
        {
        case CommandAction::Pulse:  return "pulse";
        case CommandAction::Move:   return "move";
        case CommandAction::Summon: return "summon";
        case CommandAction::Unknown:
        default:                    return "unknown";
        }
    }

    inline std::string CommandResultToJson(const CommandResult& result)
    {
        std::ostringstream oss;
        oss << "{";

        oss << "\"success\":" << (result.success ? "true" : "false") << ",";
        oss << "\"action\":\"" << CommandActionToString(result.action) << "\",";

        oss << "\"description\":\"" << EscapeJsonString(result.description) << "\",";

        oss << "\"args\":{";
        bool firstArg = true;
        for (const auto& [key, value] : result.args)
        {
            if (!firstArg) oss << ",";
            firstArg = false;
            oss << "\"" << EscapeJsonString(key) << "\":";
            if (std::holds_alternative<int>(value))
            {
                oss << std::get<int>(value);
            }
            else
            {
                oss << "\"" << EscapeJsonString(std::get<std::string>(value)) << "\"";
            }
        }
        oss << "},";

        oss << "\"payload\":{";
        bool hasPayload = false;

        if (std::holds_alternative<PulseResult>(result.payload))
        {
            const auto& p = std::get<PulseResult>(result.payload);
            oss << "\"type\":\"pulse\","
                << "\"detectedHostileCount\":" << p.detectedHostileCount << ","
                << "\"detectedFriendlyCount\":" << p.detectedFriendlyCount;
            hasPayload = true;
        }
        else if (std::holds_alternative<MoveResult>(result.payload))
        {
            const auto& m = std::get<MoveResult>(result.payload);
            oss << "\"type\":\"move\","
                << "\"oldX\":" << m.oldX << ","
                << "\"oldY\":" << m.oldY << ","
                << "\"newX\":" << m.newX << ","
                << "\"newY\":" << m.newY;
            hasPayload = true;
        }
        else if (std::holds_alternative<SummonResult>(result.payload))
        {
            const auto& s = std::get<SummonResult>(result.payload);
            oss << "\"type\":\"summon\","
                << "\"entityId\":" << s.entityId << ","
                << "\"x\":" << s.x << ","
                << "\"y\":" << s.y;
            hasPayload = true;
        }

        if (!hasPayload)
        {
            oss << "\"type\":\"none\"";
        }

        oss << "}";
        oss << "}";

        return oss.str();
    }
}