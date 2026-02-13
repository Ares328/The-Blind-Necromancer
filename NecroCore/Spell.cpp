#include "Spell.h"

namespace NecroCore
{
    const ISpell& GetWaterSpell();
    const ISpell& GetFireSpell();

    const ISpell* FindSpellByName(const std::string& name)
    {
        if (name == "water") return &GetWaterSpell();
        if (name == "fire")  return &GetFireSpell();
        return nullptr;
    }
}