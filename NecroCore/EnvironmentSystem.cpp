#include "EnvironmentSystem.h"
#include "Game.h"
#include "Map.h"

#include <iostream>
#include <sstream>

namespace NecroCore
{
    void EnvironmentSystem::ApplyTurn(Game& game, CommandResult& result)
    {
        std::ostringstream oss;
        oss << result.description;

        bool firstAppend = true;
        auto appendSeparator = [&]()
        {
            if (firstAppend)
            {
                oss << "\n";
                firstAppend = false;
            }
            else
            {
                oss << "\n";
            }
        };
        Map& map = const_cast<Map&>(game.GetMap());
        // Player fire damage
        {
            Player& player = const_cast<Player&>(game.GetPlayer());
            const int px = player.x;
            const int py = player.y;

            if (map.IsOnFire(px, py) && player.hp > 0)
            {
                player.hp -= 1;
                if (player.hp < 0) player.hp = 0;

                appendSeparator();
                oss << "Flames bite at your legs, heat searing your skin.";

                if (player.hp == 0 && !result.gameOver)
                {
                    appendSeparator();
                    oss << "You finally sink into the blaze, vision drowning in red.";
                    result.gameOver = true;
                }
            }
        }

		// Entity fire damage
        {
            Map& map = const_cast<Map&>(game.GetMap());
            auto& entities = const_cast<std::vector<Entity>&>(game.GetEntities());

			std::cout << "[EnvironmentSystem] Processing " << entities.size() << " entities for fire damage.\n";
            for (Entity& entity : entities)
            {
                if (entity.hp <= 0) continue;
				std::cout << "[EnvironmentSystem] Checking entity " << entity.id << " at (" << entity.x << "," << entity.y << ") for fire damage.\n";
                std::cout << "[EnvironmentSystem] is map on fire " << map.IsOnFire(entity.x, entity.y) << ".\n";
                if (!map.IsOnFire(entity.x, entity.y)) continue;

                entity.hp -= 1;
                if (entity.hp < 0) entity.hp = 0;

                if (entity.faction == Faction::Friendly)
                {
                    appendSeparator();
                    if (entity.hp == 0)
                    {
                        oss << "Your summoned ally succumbs to the flames.";
                    }
                    else
                    {
                        oss << "Your summoned ally writhes in fire.";
					}
                }
                else if (entity.faction == Faction::Hostile)
                {
                    appendSeparator();
                    if (entity.hp == 0)
                    {
                        oss << "A nearby hostile creature is consumed by the flames, leaving only ashes.";
                    }
                    else
                    {
                        oss << "Somewhere nearby, something snarls in burning agony.";
                    }
                }
            }
        }

        result.description = oss.str();
    }
}