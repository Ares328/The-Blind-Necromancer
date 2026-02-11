#include <gtest/gtest.h>
#include "Game.h"
#include "Map.h"
#include "EnvironmentSystem.h"

using namespace NecroCore;

TEST(ElementalTest, WaterSpellExtinguishesFireTileAndActorStatus)
{
    Game game("Ares");
    Map& map = game.GetMap();
    Player& player = game.GetPlayer();

    const int tx = player.x + 1;
    const int ty = player.y;

    map.SetTileState(tx, ty, StatusEffect::OnFire);
    game.SpawnHostileAt(tx, ty);
    auto& entities = game.GetEntities();
    ASSERT_FALSE(entities.empty());
    Entity& hostile = entities.back();
    hostile.AddStatus(StatusEffect::OnFire);

    CommandResult cmd = game.ApplyTurn("cast water east");

    EXPECT_EQ(map.GetTileState(tx, ty), StatusEffect::Normal);
    EXPECT_FALSE(HasStatus(hostile.status, StatusEffect::OnFire));
    EXPECT_NE(std::string::npos, cmd.description.find("Water splashes over the flames, extinguishing them."));
}

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

TEST(ElementalTest, WaterSpellExtinguishesFireOnPlayer)
{
    Game game("Ares");
    Map& map = game.GetMap();
    Player& player = game.GetPlayer();

    const int tx = player.x + 1;
    const int ty = player.y;

    map.SetTileState(tx, ty, StatusEffect::OnFire);

    CommandResult moveCmd = game.ApplyTurn("move east");
    ASSERT_TRUE(HasStatus(player.status, StatusEffect::OnFire));

    CommandResult castCmd = game.ApplyTurn("cast water self");
	std::cout << "[Test] Cast water self: " << castCmd.description << "\n";

    EXPECT_FALSE(HasStatus(player.status, StatusEffect::OnFire));
    EXPECT_NE(std::string::npos,
        castCmd.description.find("Water splashes over the flames, extinguishing them."));
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

    CommandResult cmd = game.ApplyTurn("cast fire east");

    EXPECT_NE(std::string::npos,
        cmd.description.find("You cast a spell of fire towards east, but nothing happens."));
}