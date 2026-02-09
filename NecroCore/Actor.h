#pragma once

#include "Entity.h"
#include "Status.h"
#include <unordered_map>
#include <string>

namespace NecroCore
{
    struct Actor
    {
        int id;
        std::string name;
        int x = 0;
        int y = 0;
        int hp = 10;
        int maxHp = 10;
        int attackDamage = 1;

		StatusEffect status = StatusEffect::Normal;
        std::unordered_map<StatusEffect, int> statusDurations;

        static bool IsAdjacent(int x1, int y1, int x2, int y2)
        {
            const int dx = std::abs(x1 - x2);
            const int dy = std::abs(y1 - y2);
            return (dx <= 1 && dy <= 1) && !(dx == 0 && dy == 0);
        }

        bool IsAdjacentTo(int otherX, int otherY) const
        {
            return IsAdjacent(x, y, otherX, otherY);
        }

        void ApplyDamage(int damage)
        {
            hp -= damage;
            if (hp < 0) hp = 0;
        }

        bool IsAlive() const
        {
            return hp > 0;
        }

        void MoveTo(int newX, int newY)
        {
            x = newX;
            y = newY;
		}

        void AddStatus(StatusEffect s, int duration = -1)
        {
            status |= s;
            if (duration < 0)
            {
                if (auto desc = GetStatusDescriptor(s))
                    duration = desc->defaultDuration;
                else
                    duration = 1;
            }
            statusDurations[s] = duration;
        }

        void ClearStatus(StatusEffect s)
        {
            auto mask = static_cast<unsigned char>(status);
            mask &= ~static_cast<unsigned char>(s);
            status = static_cast<StatusEffect>(mask);
            statusDurations.erase(s);
        }
    };
}