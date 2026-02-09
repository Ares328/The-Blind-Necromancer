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

TEST(EnvironmentTest, PlayerTakesDamageStandingOnFire)
{
    Game game("Ares");
    const Player& playerRef = game.GetPlayer();
    Map& map = game.GetMap();

    const int px = playerRef.x;
    const int py = playerRef.y;

    const int initialHp = playerRef.hp;

    map.SetTileState(px, py, StatusEffect::OnFire);

    CommandResult cmd = game.ApplyTurn("wait");

    const Player& playerAfter = game.GetPlayer();
    std::cout << "[Test] Before hp=" << initialHp
        << " after hp=" << playerAfter.hp << "\n";

    EXPECT_TRUE(cmd.success);
    EXPECT_LT(playerAfter.hp, initialHp);
    EXPECT_NE(std::string::npos,
		cmd.description.find("Flames bite at your legs, heat searing your skin."));
}

TEST(EnvironmentTest, PlayerCanDieFromFireAndSetGameOver)
{
    Game game("Ares");
    Player& mutablePlayer = game.GetPlayer();
    Map& map = game.GetMap();

    const int px = mutablePlayer.x;
    const int py = mutablePlayer.y;

    mutablePlayer.hp = 1;
    mutablePlayer.maxHp = 10;
    map.SetTileState(px, py, StatusEffect::OnFire);

    CommandResult cmd = game.ApplyTurn("wait");

    const Player& playerAfter = game.GetPlayer();
    EXPECT_EQ(playerAfter.hp, 0);
    EXPECT_TRUE(cmd.gameOver);
    EXPECT_NE(std::string::npos, cmd.description.find("You finally sink into the blaze, vision drowning in red."));
}

TEST(EnvironmentTest, SummonedAllyTakesDamageStandingOnFire)
{
    Game game("Ares");
    const Player& player = game.GetPlayer();
    Map& map = game.GetMap();

    const int sx = player.x + 1;
    const int sy = player.y;
    map.SetTileState(sx, sy, StatusEffect::OnFire);
    game.SpawnFriendlyWithStatsForTest(sx, sy, 3, 1, "Friendly Undead");

    CommandResult cmd = game.ApplyTurn("wait");

    bool found = false;
    for (const Entity& e : game.GetEntities())
    {
        if (e.faction == Faction::Friendly && e.x == sx && e.y == sy)
        {
            EXPECT_LT(e.hp, 3);
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
    EXPECT_NE(std::string::npos,
        cmd.description.find("A Friendly Undead writhes in fire."));
}

TEST(EnvironmentTest, HostileCanDieFromFireAndBeRemoved)
{
    Game game("Ares");
    const Player& player = game.GetPlayer();
    Map& map = const_cast<Map&>(game.GetMap());

    const int hx = player.x + 2;
    const int hy = player.y;
	
    // Hostile movement before environment effect, put effect where hostile will be
    map.SetTileState(hx - 1, hy - 1, StatusEffect::OnFire);
    EXPECT_TRUE(map.GetTileState(hx - 1, hy - 1) == StatusEffect::OnFire);
    game.SpawnHostileWithStatsForTest(hx, hy, 1, 1, "Skeleton");

    CommandResult cmd = game.ApplyTurn("wait");

    for (const Entity& e : game.GetEntities())
    {
        ASSERT_FALSE(e.faction == Faction::Hostile && e.x == hx && e.y == hy);
    }

    EXPECT_NE(std::string::npos,
        cmd.description.find("A Skeleton is consumed by the flames."));
}
TEST(EnvironmentTest, HostileCanTakeDamageFromFire)
{
    Game game("Ares");
    const Player& player = game.GetPlayer();
    Map& map = const_cast<Map&>(game.GetMap());
    const int hx = player.x + 2;
    const int hy = player.y;
    // Hostile movement before environment effect, put effect where hostile will be
    map.SetTileState(hx - 1, hy - 1, StatusEffect::OnFire);
    EXPECT_TRUE(map.GetTileState(hx - 1, hy - 1) == StatusEffect::OnFire);
    game.SpawnHostileWithStatsForTest(hx, hy, 2, 1, "Skeleton");

    CommandResult cmd = game.ApplyTurn("wait");

    for (const Entity& e : game.GetEntities())
    {
        ASSERT_FALSE(e.faction == Faction::Hostile && e.x == hx && e.y == hy);
    }

    EXPECT_NE(std::string::npos,
        cmd.description.find("A Skeleton writhes in fire."));
}

TEST(EnvironmentTest, EntityOnFire_TakesDamageEachTurn_AndExpires)
{
    Game game("Ares");
    CommandResult result{};
    result.success = true;

    Map& map = game.GetMap();
    auto& entities = game.GetEntities();

    const int ex = map.spawnX + 1;
    const int ey = map.spawnY;
    game.SpawnHostileAt(ex, ey);

    ASSERT_FALSE(entities.empty());
    Entity& e = entities.back();
    const int initialHp = e.hp;

    map.SetTileState(ex, ey, StatusEffect::OnFire);
    // 3 turn damage of fire

    // Turn 1
    result.description.clear();
    ApplyEnvironmentTurn(game, result);
    EXPECT_EQ(e.hp, initialHp - 1);
    EXPECT_NE(std::string::npos, result.description.find("writhes in fire"));

    map.SetTileState(ex, ey, StatusEffect::Normal);

    // Turn 2
    result.description.clear();
    ApplyEnvironmentTurn(game, result);
    EXPECT_EQ(e.hp, initialHp - 2);

    // Turn 3 (last tick)
    result.description.clear();
    ApplyEnvironmentTurn(game, result);
    EXPECT_EQ(e.hp, initialHp - 3);

    // Turn 4: no further damage.
    result.description.clear();
    ApplyEnvironmentTurn(game, result);
    EXPECT_EQ(e.hp, initialHp - 3);
}

TEST(EnvironmentTest, PlayerOnFire_TakesDamageEachTurn_AndCanDie)
{
    Game game("Ares");
    CommandResult result{};
    result.success = true;

    Player& player = game.GetPlayer();
    Map& map = game.GetMap();

    // Put player on an OnFire tile
    const int px = player.x;
    const int py = player.y;
    map.SetTileState(px, py, StatusEffect::OnFire);

    player.hp = 3;
    const int initialHp = player.hp;

    // Turn 1
    result.description.clear();
    ApplyEnvironmentTurn(game, result);
    EXPECT_EQ(player.hp, initialHp - 1);
    EXPECT_NE(std::string::npos, result.description.find("Flames bite at your legs"));

    // Turn 2
    result.description.clear();
    ApplyEnvironmentTurn(game, result);
    EXPECT_EQ(player.hp, initialHp - 2);

    // Turn 3: player may die here depending on initial hp
    result.description.clear();
    ApplyEnvironmentTurn(game, result);
    EXPECT_EQ(player.hp, initialHp - 3);
    EXPECT_LE(player.hp, 0);
    EXPECT_TRUE(result.gameOver);
}

TEST(EnvironmentTest, FriendlyAndHostileUseDifferentEntityNamesInMessages)
{
    Game game("Ares");
    CommandResult result{};
    result.success = true;

    Map& map = game.GetMap();

    // Spawn friendly and hostile on adjacent tiles
    const int fx = map.spawnX + 1;
    const int fy = map.spawnY;
    const int hx = map.spawnX + 2;
    const int hy = map.spawnY;

    game.SpawnFriendlyAt(fx, fy);
    game.SpawnHostileAt(hx, hy);

    auto& entities = game.GetEntities();
    ASSERT_GE(entities.size(), 2u);

    Entity& friendly = entities[entities.size() - 2];
    Entity& hostile = entities[entities.size() - 1];

    // Put both tiles on fire
    map.SetTileState(friendly.x, friendly.y, StatusEffect::OnFire);
    map.SetTileState(hostile.x, hostile.y, StatusEffect::OnFire);

    result.description.clear();
    ApplyEnvironmentTurn(game, result);

    const std::string desc = result.description;

    // Depending on your subject selection in EnvironmentSystem (friendly uses
    // "your summoned ally", hostile uses "a hostile" or entity.name), assert that
    // both patterns appear.
    EXPECT_NE(std::string::npos, desc.find("friendly undead"));
    EXPECT_NE(std::string::npos, desc.find("Skeleton"));
}

TEST(EnvironmentTest, OnFireStatusDoesNotStackMultipleTimesFromTile)
{
    Game game("Ares");
    CommandResult result{};
    result.success = true;

    Map& map = game.GetMap();
    auto& entities = game.GetEntities();

    const int ex = map.spawnX + 1;
    const int ey = map.spawnY;
    game.SpawnHostileAt(ex, ey);

    ASSERT_FALSE(entities.empty());
    Entity& e = entities.back();
    const int initialHp = e.hp;

    // Tile is OnFire from the start
    map.SetTileState(ex, ey, StatusEffect::OnFire);

    result.description.clear();
    ApplyEnvironmentTurn(game, result);
    map.SetTileState(ex, ey, StatusEffect::Normal);

    for (int i = 0; i < 5; ++i)
    {
        result.description.clear();
        ApplyEnvironmentTurn(game, result);
    }

    // total damage should be exactly 3, not 5
    EXPECT_EQ(e.hp, initialHp - 3);
}

TEST(EnvironmentTest, PoisonedStatusHostileCanTakeDamage)
{
    Game game("Ares");
    const Player& player = game.GetPlayer();
    Map& map = const_cast<Map&>(game.GetMap());
    const int hx = player.x + 2;
    const int hy = player.y;
    // Hostile movement before environment effect, put effect where hostile will be
    map.SetTileState(hx - 1, hy - 1, StatusEffect::Poisoned);
    EXPECT_TRUE(map.GetTileState(hx - 1, hy - 1) == StatusEffect::Poisoned);
    game.SpawnHostileWithStatsForTest(hx, hy, 3, 1, "Skeleton");

    CommandResult cmd = game.ApplyTurn("wait");

    for (const Entity& e : game.GetEntities())
    {
        ASSERT_FALSE(e.faction == Faction::Hostile && e.x == hx && e.y == hy);
    }

    EXPECT_NE(std::string::npos,
        cmd.description.find("A Skeleton coughs violently, poisoned."));
}

TEST(EnvironmentTest, PoisonedStatusHostileCanKill)
{
    Game game("Ares");
    const Player& player = game.GetPlayer();
    Map& map = const_cast<Map&>(game.GetMap());
    const int hx = player.x + 2;
    const int hy = player.y;
    // Hostile movement before environment effect, put effect where hostile will be
    map.SetTileState(hx - 1, hy - 1, StatusEffect::Poisoned);
    EXPECT_TRUE(map.GetTileState(hx - 1, hy - 1) == StatusEffect::Poisoned);
    game.SpawnHostileWithStatsForTest(hx, hy, 2, 1, "Skeleton");

    CommandResult cmd = game.ApplyTurn("wait");

    for (const Entity& e : game.GetEntities())
    {
        ASSERT_FALSE(e.faction == Faction::Hostile && e.x == hx && e.y == hy);
    }

    EXPECT_NE(std::string::npos,
        cmd.description.find("A Skeleton succumbs to the poison and collapses."));
}