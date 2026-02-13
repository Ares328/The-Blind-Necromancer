#pragma once

#include <string>
namespace NecroCore {

    class Game; 
    struct CastResult;

    class ISpell
    {
    public:
        virtual ~ISpell() = default;

        virtual const char* GetName() const = 0;

        virtual CastResult Cast(Game& game, int tx, int ty, const std::string& direction) const = 0;
    };

    const ISpell* FindSpellByName(const std::string& name);
}