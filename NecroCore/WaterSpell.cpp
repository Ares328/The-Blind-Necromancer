#include "Spell.h"
#include "Game.h"
#include "Map.h"
#include "Status.h"
#include "Actor.h"
#include "Entity.h"

#include <iostream>

namespace NecroCore
{
    class WaterSpell : public ISpell
    {
    public:
        const char* GetName() const override { return "water"; }

        CastResult Cast(Game& game, int tx, int ty, const std::string& direction) const override
        {
            CastResult result{};

            Map& map = game.GetMap();
            auto& entities = game.GetEntities();
            Player& player = game.GetPlayer();

            bool didAnything = false;

            StatusEffect tileState = map.GetTileState(tx, ty);
            if (HasStatus(tileState, StatusEffect::OnFire))
            {
                map.SetTileState(tx, ty, StatusEffect::Normal);
                if (map.IsTrap(tx, ty))
                {
                    map.convertTile(tx, ty, TileType::Floor);
                }
                didAnything = true;
            }

            for (Entity& e : entities)
            {
                if (e.x == tx && e.y == ty && e.IsAlive() &&
                    HasStatus(e.status, StatusEffect::OnFire))
                {
                    e.ClearStatus(StatusEffect::OnFire);
                    didAnything = true;
                }
            }

            if (map.IsFireplace(tx, ty))
            {
                map.SetTileState(tx, ty, StatusEffect::Normal);
                didAnything = true;
            }

            if (player.x == tx && player.y == ty &&
                player.IsAlive() &&
                HasStatus(player.status, StatusEffect::OnFire))
            {
                std::cout << "[CastSpell] Water spell extinguishes fire on player.\n";
                player.ClearStatus(StatusEffect::OnFire);
                didAnything = true;
            }

            if (didAnything)
            {
                result.description = "Water splashes over the flames, extinguishing them.";
            }
            else
            {
                result.description = "Water splashes harmlessly onto the cold stone.";
            }

            return result;
        }
    };

    const ISpell& GetWaterSpell()
    {
        static WaterSpell instance;
        return instance;
    }
}