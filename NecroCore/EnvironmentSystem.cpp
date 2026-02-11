#include "EnvironmentSystem.h"
#include "Game.h"
#include "Map.h"
#include "Entity.h"

#include <iostream>
#include <sstream>
#include <vector>

namespace NecroCore
{
    void EnvironmentSystem::ApplyTurn(Game& game, CommandResult& result)
    {
        auto& entities = game.GetEntities();
        Player& player = game.GetPlayer();
        Map& map = game.GetMap();

        std::ostringstream oss;
        oss << result.description;

        bool firstAppend = result.description.empty();
        auto appendSeparator = [&]()
            {
                if (!firstAppend)
                {
                    oss << "\n";
                }
                firstAppend = false;
            };

        auto formatEntityMessage = [&](const char* tmpl, const std::string& entityName) -> std::string
            {
                if (!tmpl) return {};
                std::string msg = tmpl;
                const std::string placeholder = "{entity}";
                const auto pos = msg.find(placeholder);
                if (pos != std::string::npos)
                {
                    msg.replace(pos, placeholder.size(), entityName);
                }
                return msg;
            };

        auto effectFromTile = [&](Actor& actor, StatusEffect status)
            {
                if (map.GetTileState(actor.x, actor.y) == status && actor.status != status)
                {
                    actor.AddStatus(status);
                }
            };

        auto tickStatuses = [&](Actor& actor, bool isPlayer)
            {
                std::vector<StatusEffect> toProcess;
                toProcess.reserve(actor.statusDurations.size());
                for (auto& kv : actor.statusDurations)
                {
                    toProcess.push_back(kv.first);
                }

                for (StatusEffect status : toProcess)
                {
                    auto it = actor.statusDurations.find(status);
                    if (it == actor.statusDurations.end())
                        continue;

                    int& duration = it->second;
                    if (duration <= 0)
                    {
                        actor.ClearStatus(status);
                        continue;
                    }

                    auto descOpt = GetStatusDescriptor(status);
                    if (!descOpt)
                    {
                        --duration;
                        if (duration <= 0)
                            actor.ClearStatus(status);
                        continue;
                    }

                    const StatusDescriptor& desc = *descOpt;

                    if (desc.damagePerTurn > 0 && actor.IsAlive())
                    {
                        actor.ApplyDamage(desc.damagePerTurn);
                    }

                    if (isPlayer)
                    {
                        appendSeparator();
                        if (actor.IsAlive())
                        {
                            oss << desc.playerTickMessage;
                        }
                        else
                        {
                            oss << desc.playerDeathMessage;
                        }
                    }
                    else
                    {
                        appendSeparator();
                        if (actor.IsAlive())
                        {
                            oss << formatEntityMessage(desc.entityTickMessage, actor.name);
                        }
                        else
                        {
                            oss << formatEntityMessage(desc.entityDeathMessage, actor.name);
                        }
                    }

                    --duration;
                    if (duration <= 0)
                    {
                        if (isPlayer)
                        {
                            appendSeparator();
                            oss << desc.playerEndMessage;
                        }
                        else
                        {
                            appendSeparator();
                            oss << formatEntityMessage(desc.entityEndMessage, actor.name);
                        }
                        actor.ClearStatus(status);
                    }
                }
            };

        // Player tile/tick effects
        StatusEffect playerTileEffect = map.GetTileState(player.x, player.y);
        effectFromTile(player, playerTileEffect);

        tickStatuses(player, true);
        if (!player.IsAlive() && !result.gameOver)
        {
            result.gameOver = true;
        }

        // Entity tile/tick effects
        for (Entity& entity : entities)
        {
            if (!entity.IsAlive()) continue;

            StatusEffect entityTileEffect = map.GetTileState(entity.x, entity.y);

            std::cout << "[Env] Entity " << entity.name << " at (" << entity.x << "," << entity.y
                << ") tile status: " << (int)entityTileEffect << "\n";
            effectFromTile(entity, entityTileEffect);

            tickStatuses(entity, false);
		}

        result.description = oss.str();
    }
}