#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace NecroCore
{
    enum class StatusEffect : std::uint8_t
    {
        Normal = 0,
        OnFire = 1 << 0,
		Poisoned = 1 << 1,
		Wet = 1 << 2,
    };

    inline StatusEffect operator|(StatusEffect a, StatusEffect b)
    {
        return static_cast<StatusEffect>(
            static_cast<std::uint8_t>(a) | static_cast<std::uint8_t>(b));
    }

    inline StatusEffect& operator|=(StatusEffect& a, StatusEffect b)
    {
        a = a | b;
        return a;
    }

    inline bool HasStatus(StatusEffect value, StatusEffect flag)
    {
        return (static_cast<std::uint8_t>(value) &
            static_cast<std::uint8_t>(flag)) != 0;
    }

    struct StatusDescriptor
    {
        const char* playerTickMessage;
        const char* entityTickMessage;
        const char* playerDeathMessage;
		const char* entityDeathMessage;
        const char* playerEndMessage;
        const char* entityEndMessage;
        int damagePerTurn;
        int defaultDuration;
    };

    inline std::optional<StatusEffect> StatusEffectFromSpell(const std::string& spellName)
    {
        if (spellName == "fire")
            return StatusEffect::OnFire;
        if (spellName == "poison")
            return StatusEffect::Poisoned;
        if (spellName == "water")
			return StatusEffect::Wet;

        return std::nullopt;
    }

    inline std::optional<StatusDescriptor> GetStatusDescriptor(StatusEffect status)
    {
        switch (status)
        {
        case StatusEffect::OnFire:
            return StatusDescriptor{
                "Flames bite at your legs, heat searing your skin.",
                "A {entity} writhes in fire.",
				"You finally sink into the blaze, vision drowning in red.",
				"A {entity} is consumed by the flames.",
                "The flames around you die out.",
                "The flames consuming the {entity} die out.",
                1,
                3
            };
        case StatusEffect::Poisoned:
            return StatusDescriptor{
                "The poison courses through your veins, burning with acidic pain.",
				"A {entity} coughs violently, poisoned.",
				"The poison overwhelms you, and you collapse to the ground.",
                "A {entity} succumbs to the poison and collapses.",
                "The poison's grip on you loosens, and you feel relief.",
                "The {entity} looks healthier as the poison wears off.",
                2,
                3
			};
        case StatusEffect::Wet:
            return StatusDescriptor{
                "Water drips from your soaked clothes, chilling you to the bone.",
                "A {entity} is drenched, water pooling around it.",
                "You shiver as the water saps your strength.",
                "A {entity} struggles to move through the water.",
                "The water drips away, leaving you feeling warm.",
                "The {entity} shakes off the water, regaining its composure.",
                1,
                2
            };
        default:
            return std::nullopt;
        }
    }
}