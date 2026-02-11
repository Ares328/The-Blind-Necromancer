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

TEST(FireplaceTest, WalkingIntoFireplaceDamagesPlayer)
{
    Game game("Ares");
    Map& map = game.GetMap();
    Player& player = game.GetPlayer();
    game.SpawnFireplaceAt(player.x + 1, player.y, true);
    CommandResult cmd = game.ApplyTurn("move east");
    EXPECT_TRUE(HasStatus(player.status, StatusEffect::OnFire));
    EXPECT_NE(std::string::npos,
        cmd.description.find("Flames bite at your legs, heat searing your skin."));
}
TEST(FireplaceTest, WalkingSummonIntoFireplaceDamagesSummon)
{
    Game game("Ares");
    Map& map = game.GetMap();
    Player& player = game.GetPlayer();
    game.SpawnFriendlyWithStatsForTest(player.x + 1, player.y,2, 1, "skely");
    game.SpawnFireplaceAt(player.x + 2, player.y, true);
    CommandResult cmd = game.ApplyTurn("command skely move east");
    Entity* summon = game.GetEntityByName("skely");
    ASSERT_NE(summon, nullptr);
    EXPECT_TRUE(HasStatus(summon->status, StatusEffect::OnFire));
    EXPECT_NE(std::string::npos,
		cmd.description.find("A skely writhes in fire."));
}
TEST(FireplaceTest, FireplaceDoesNotDamagePlayerWhenNotOnFire)
{
    Game game("Ares");
    Map& map = game.GetMap();
    Player& player = game.GetPlayer();
    game.SpawnFireplaceAt(player.x + 1, player.y, false);
    CommandResult cmd = game.ApplyTurn("move east");
    EXPECT_FALSE(HasStatus(player.status, StatusEffect::OnFire));
    EXPECT_NE(std::string::npos,
        cmd.description.find("You move east."));
}
TEST(FireplaceTest, FireplaceWaterSpellExtinguishesFireplace)
{
    Game game("Ares");
    Map& map = game.GetMap();
    Player& player = game.GetPlayer();
    game.SpawnFireplaceAt(player.x + 1, player.y, true);
    CommandResult cmd = game.ApplyTurn("cast water east");
    EXPECT_FALSE(map.GetTileState(player.x + 1, player.y) == StatusEffect::OnFire);
    EXPECT_NE(std::string::npos,
        cmd.description.find("Water splashes over the flames, extinguishing them."));
}