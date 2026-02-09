#include <gtest/gtest.h>
#include "Game.h"
#include "Map.h"
#include "EnvironmentSystem.h"

using namespace NecroCore;

namespace
{
    void ApplyEnvironmentTurn(Game& game, CommandResult& result)
    {
        static EnvironmentSystem envSystem;
        envSystem.ApplyTurn(game, result);
    }
}

TEST(TrapTest, SteppingOnFireTrap_AppliesOnFireStatusToPlayer)
{
    Game game("Ares");
    Map& map = game.GetMap();
    Player& player = game.GetPlayer();

    const int trapX = player.x + 1;
    const int trapY = player.y;

    game.SpawnTrapAt(trapX, trapY, StatusEffect::OnFire);

    CommandResult cmd = game.ApplyTurn("move east");

    EXPECT_TRUE(HasStatus(player.status, StatusEffect::OnFire));
    EXPECT_NE(std::string::npos,
        cmd.description.find("A hidden fire trap erupts beneath Ares!"));


    const int hpAfterTrap = player.hp;
    CommandResult envResult{};
    envResult.success = true;
	ApplyEnvironmentTurn(game, envResult);

    EXPECT_LT(player.hp, hpAfterTrap);
    EXPECT_NE(std::string::npos,
        envResult.description.find("Flames bite at your legs, heat searing your skin."));
}

TEST(TrapTest, SteppingOnPoisonTrap_AppliesOnPoisonStatusToPlayer)
{
    Game game("Ares");
    Map& map = game.GetMap();
    Player& player = game.GetPlayer();

    const int trapX = player.x + 1;
    const int trapY = player.y;

    game.SpawnTrapAt(trapX, trapY, StatusEffect::Poisoned);

    CommandResult cmd = game.ApplyTurn("move east");

    EXPECT_TRUE(HasStatus(player.status, StatusEffect::Poisoned));
    EXPECT_NE(std::string::npos,
        cmd.description.find("A hidden poison trap is sprung beneath Ares!"));


    const int hpAfterTrap = player.hp;
    CommandResult envResult{};
    envResult.success = true;
    ApplyEnvironmentTurn(game, envResult);

    EXPECT_LT(player.hp, hpAfterTrap);
    EXPECT_NE(std::string::npos,
        envResult.description.find("The poison courses through your veins, burning with acidic pain."));
}

TEST(TrapTest, TrapTriggersOnlyOnce)
{
    Game game("Ares");
    Map& map = game.GetMap();
    Player& player = game.GetPlayer();

    const int trapX = player.x + 1;
    const int trapY = player.y;

    game.SpawnTrapAt(trapX, trapY, StatusEffect::OnFire);

    CommandResult first = game.ApplyTurn("move east");
    EXPECT_NE(std::string::npos,
        first.description.find("A hidden fire trap erupts beneath Ares!"));

    CommandResult second = game.ApplyTurn("move west");

    CommandResult third = game.ApplyTurn("move east");

    EXPECT_EQ(std::string::npos,
        third.description.find("A hidden fire trap erupts beneath Ares!"));
}

TEST(TrapTest, PulseCommandReportsNearbyTrapsInDescription)
{
    Game game("Ares");
    Map& map = game.GetMap();
    const Player& player = game.GetPlayer();

    const int px = player.x;
    const int py = player.y;

    game.SpawnTrapAt(px + 1, py, StatusEffect::OnFire);
    game.SpawnTrapAt(px, py - 2, StatusEffect::Poisoned);

    CommandResult cmd = game.ApplyTurn("pulse");

    const std::string desc = cmd.description;

    EXPECT_NE(std::string::npos,
        desc.find("Your senses extend outward."));

    EXPECT_NE(std::string::npos,
        desc.find("You sense a fire trap to the east"));
    EXPECT_NE(std::string::npos,
        desc.find("You sense a poison trap to the north"));
}

TEST(TrapTest, PulseCommandReportsFarAwayTrapsInDescription)
{
    Game game("Ares");
    Map& map = game.GetMap();
    const Player& player = game.GetPlayer();
    const int px = player.x;
    const int py = player.y;
    game.SpawnTrapAt(px + 5, py, StatusEffect::OnFire);
    game.SpawnTrapAt(px, py - 3, StatusEffect::Poisoned);
    game.SpawnTrapAt(px - 5, py + 3, StatusEffect::OnFire);
    CommandResult cmd = game.ApplyTurn("pulse");
    const std::string desc = cmd.description;
    EXPECT_NE(std::string::npos,
        desc.find("Your senses extend outward."));
    EXPECT_NE(std::string::npos,
        desc.find("You sense a fire trap to the east"));
    EXPECT_NE(std::string::npos,
		desc.find("You sense a poison trap to the north"));
    EXPECT_NE(std::string::npos,
        desc.find("You sense a fire trap to the south-west"));
}