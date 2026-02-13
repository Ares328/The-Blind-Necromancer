#include <gtest/gtest.h>
#include "Game.h"
#include "Map.h"
#include "Status.h"

using namespace NecroCore;

TEST(SpellsTest, WaterSpellExtinguishesBurningTrapAndConvertsToFloor)
{
    Game game("Ares");
    Player& player = game.GetPlayer();
    Map& map = game.GetMap();

    const int tx = player.x + 1;
    const int ty = player.y;

    game.SpawnTrapAt(tx, ty, StatusEffect::OnFire);
    ASSERT_TRUE(map.IsTrap(tx, ty));
    ASSERT_TRUE(HasStatus(map.GetTileState(tx, ty), StatusEffect::OnFire));

    CommandResult cmd = game.ApplyTurn("cast water east");

    EXPECT_FALSE(map.IsTrap(tx, ty));
    EXPECT_EQ(map.GetTile(tx, ty), TileType::Floor);
    EXPECT_FALSE(HasStatus(map.GetTileState(tx, ty), StatusEffect::OnFire));

    EXPECT_NE(std::string::npos,
        cmd.description.find("Water splashes over the flames, extinguishing them."));
}

TEST(SpellsTest, WaterSpellExtinguishesBurningPlayer)
{
    Game game("Ares");
    Player& player = game.GetPlayer();

    player.AddStatus(StatusEffect::OnFire);
    ASSERT_TRUE(HasStatus(player.status, StatusEffect::OnFire));

    CommandResult cmd = game.ApplyTurn("cast water self");

    EXPECT_FALSE(HasStatus(player.status, StatusEffect::OnFire));
    EXPECT_NE(std::string::npos,
        cmd.description.find("Water splashes over the flames, extinguishing them."));
}

TEST(SpellsTest, WaterSpellDoesNothingOnColdStone)
{
    Game game("Ares");
    Player& player = game.GetPlayer();
    Map& map = game.GetMap();

    const int tx = player.x + 1;
    const int ty = player.y;

    EXPECT_EQ(map.GetTile(tx, ty), TileType::Floor);
    EXPECT_EQ(map.GetTileState(tx, ty), StatusEffect::Normal);

    CommandResult cmd = game.ApplyTurn("cast water east");

    EXPECT_EQ(map.GetTile(tx, ty), TileType::Floor);
    EXPECT_EQ(map.GetTileState(tx, ty), StatusEffect::Normal);

    EXPECT_NE(std::string::npos,
        cmd.description.find("Water splashes harmlessly onto the cold stone."));
}

TEST(SpellsTest, FireSpellIgnitesNewFireplace)
{
    Game game("Ares");
    Player& player = game.GetPlayer();
    Map& map = game.GetMap();

    const int tx = player.x + 1;
    const int ty = player.y;

    ASSERT_NE(map.GetTile(tx, ty), TileType::Fireplace);

    CommandResult cmd = game.ApplyTurn("cast fire east");

    EXPECT_EQ(map.GetTile(tx, ty), TileType::Fireplace);
    EXPECT_TRUE(HasStatus(map.GetTileState(tx, ty), StatusEffect::OnFire));

    EXPECT_NE(std::string::npos,
        cmd.description.find("You cast a spell of fire towards east, igniting the fireplace."));
}

TEST(SpellsTest, FireSpellIgnitesExistingUnlitFireplace)
{
    Game game("Ares");
    Player& player = game.GetPlayer();
    Map& map = game.GetMap();

    const int tx = player.x + 1;
    const int ty = player.y;

    game.SpawnFireplaceAt(tx, ty, false);
    ASSERT_EQ(map.GetTile(tx, ty), TileType::Fireplace);
    EXPECT_EQ(map.GetTileState(tx, ty), StatusEffect::Normal);

    CommandResult cmd = game.ApplyTurn("cast fire east");

    EXPECT_EQ(map.GetTile(tx, ty), TileType::Fireplace);
    EXPECT_TRUE(HasStatus(map.GetTileState(tx, ty), StatusEffect::OnFire));

    EXPECT_NE(std::string::npos,
        cmd.description.find("You cast a spell of fire towards east, igniting the fireplace."));
}

TEST(SpellsTest, FireSpellOnAlreadyBurningFireplaceDoesNothingSpecial)
{
    Game game("Ares");
    Player& player = game.GetPlayer();
    Map& map = game.GetMap();

    const int tx = player.x + 1;
    const int ty = player.y;

    game.SpawnFireplaceAt(tx, ty, true);
    ASSERT_EQ(map.GetTile(tx, ty), TileType::Fireplace);
    ASSERT_TRUE(HasStatus(map.GetTileState(tx, ty), StatusEffect::OnFire));

    CommandResult cmd = game.ApplyTurn("cast fire east");

    EXPECT_EQ(map.GetTile(tx, ty), TileType::Fireplace);
    EXPECT_TRUE(HasStatus(map.GetTileState(tx, ty), StatusEffect::OnFire));

    EXPECT_TRUE(cmd.success);
}