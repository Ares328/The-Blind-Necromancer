#include <gtest/gtest.h>
#include "Game.h"
#include "Map.h"
#include "EnvironmentSystem.h"

using namespace NecroCore;

TEST(ElementalTest, WaterSpellDisarmsFireTrap)
{
    Game game("Ares");
    Map& map = game.GetMap();
    Player& player = game.GetPlayer();

    const int tx = player.x + 1;
    const int ty = player.y;

    game.SpawnTrapAt(tx, ty, StatusEffect::OnFire);

    CommandResult castCmd = game.ApplyTurn("cast water east");

    CommandResult moveCmd = game.ApplyTurn("move east");
    std::string moveDesc = moveCmd.description;

    EXPECT_EQ(std::string::npos, moveDesc.find("A hidden fire trap erupts beneath"));
}

TEST(ElementalTest, WaterSpellInvalidDirectionGivesErrorMessage)
{
    Game game("Ares");

    CommandResult cmd = game.ApplyTurn("cast water sideways");

    EXPECT_NE(std::string::npos,
        cmd.description.find("The arcane forces reject your unknown direction: sideways."));
}

TEST(ElementalTest, WaterSpellOnEmptyTileDoesNothingUseful)
{
    Game game("Ares");
    Map& map = game.GetMap();
    Player& player = game.GetPlayer();

    const int tx = player.x + 1;
    const int ty = player.y;

    map.SetTileState(tx, ty, StatusEffect::Normal);

    CommandResult cmd = game.ApplyTurn("cast water east");

    EXPECT_EQ(map.GetTileState(tx, ty), StatusEffect::Normal);
    EXPECT_NE(std::string::npos,
        cmd.description.find("Water splashes harmlessly onto the cold stone."));
}

TEST(ElementalTest, UnknownElementSpellDoesNothing)
{
    Game game("Ares");

    CommandResult cmd = game.ApplyTurn("cast rock east");
	std::cout << cmd.description << "\n";
    EXPECT_NE(std::string::npos,
        cmd.description.find("The arcane forces reject your unknown element: rock."));
}

TEST(ElementalTest, KnownElementUnknownSpellDoesNothing)
{
    Game game("Ares");

    CommandResult cmd = game.ApplyTurn("cast poison east");
    std::cout << cmd.description << "\n";
    EXPECT_NE(std::string::npos,
        cmd.description.find("You cast a spell of poison towards east, but nothing happens."));
}