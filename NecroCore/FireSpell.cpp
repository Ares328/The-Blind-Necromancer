#include "Spell.h" 
#include "Game.h" 
#include "Map.h" 
#include "Status.h"

namespace NecroCore 
{
    class FireSpell : public ISpell 
    {
    public:
        const char* GetName() const override { return "fire"; }
        CastResult Cast(Game& game, int tx, int ty, const std::string& direction) const override
        {
            CastResult result{};

            Map& map = game.GetMap();

            bool affectedFireplace = false;

            if (!map.IsFireplace(tx, ty))
            {
                map.convertTile(tx, ty, TileType::Fireplace);
                map.SetTileState(tx, ty, StatusEffect::OnFire);
                affectedFireplace = true;
            }
            else
            {
                StatusEffect tileState = map.GetTileState(tx, ty);
                if (!HasStatus(tileState, StatusEffect::OnFire))
                {
                    map.SetTileState(tx, ty, StatusEffect::OnFire);
                    affectedFireplace = true;
                }
            }

            if (affectedFireplace)
            {
                result.description = "You cast a spell of fire towards " + direction + ", igniting the fireplace.";
            }
            else
            {
                result.description = "You cast a spell of fire towards " + direction + ", but nothing happens.";
            }

            return result;
        }
    };

    const ISpell& GetFireSpell()
    {
        static FireSpell instance;
        return instance;
    }
}