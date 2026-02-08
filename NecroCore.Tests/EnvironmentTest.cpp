#include <gtest/gtest.h>
#include "Game.h"
#include "Map.h"

using namespace NecroCore;

TEST(EnvironmentTest, PlayerTakesDamageStandingOnFire)
{
    Game game("Ares");
    const Player& playerRef = game.GetPlayer();
    Map& map = const_cast<Map&>(game.GetMap());

    const int px = playerRef.x;
    const int py = playerRef.y;

    const int initialHp = playerRef.hp;

    map.SetOnFire(px, py, true);

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
    Player& mutablePlayer = const_cast<Player&>(game.GetPlayer());
    Map& map = const_cast<Map&>(game.GetMap());

    const int px = mutablePlayer.x;
    const int py = mutablePlayer.y;

    mutablePlayer.hp = 1;
    mutablePlayer.maxHp = 10;
    map.SetOnFire(px, py, true);

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
    Map& map = const_cast<Map&>(game.GetMap());

    const int sx = player.x + 1;
    const int sy = player.y;
    map.SetOnFire(sx, sy, true);
    game.SpawnFriendlyWithStatsForTest(sx, sy, 3, 1);
    

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
        cmd.description.find("Your summoned ally writhes in fire."));
}

TEST(EnvironmentTest, HostileCanDieFromFireAndBeRemoved)
{
    Game game("Ares");
    const Player& player = game.GetPlayer();
    Map& map = const_cast<Map&>(game.GetMap());

    const int hx = player.x + 2;
    const int hy = player.y;
	
    // Hostile movement before environment effect, put effect where hostile will be
    map.SetOnFire(hx - 1, hy - 1, true);
    EXPECT_TRUE(map.IsOnFire(hx - 1, hy - 1));
    game.SpawnHostileWithStatsForTest(hx, hy, 1, 1);

    CommandResult cmd = game.ApplyTurn("wait");

    for (const Entity& e : game.GetEntities())
    {
        ASSERT_FALSE(e.faction == Faction::Hostile && e.x == hx && e.y == hy);
    }

    EXPECT_NE(std::string::npos,
        cmd.description.find("A nearby hostile creature is consumed by the flames, leaving only ashes."));
}
TEST(EnvironmentTest, HostileCanTakeDamageFromFire)
{
    Game game("Ares");
    const Player& player = game.GetPlayer();
    Map& map = const_cast<Map&>(game.GetMap());
    const int hx = player.x + 2;
    const int hy = player.y;
    // Hostile movement before environment effect, put effect where hostile will be
    map.SetOnFire(hx - 1, hy - 1, true);
    EXPECT_TRUE(map.IsOnFire(hx - 1, hy - 1));
    game.SpawnHostileWithStatsForTest(hx, hy, 2, 1);

    CommandResult cmd = game.ApplyTurn("wait");

    for (const Entity& e : game.GetEntities())
    {
        ASSERT_FALSE(e.faction == Faction::Hostile && e.x == hx && e.y == hy);
    }

    EXPECT_NE(std::string::npos,
        cmd.description.find("Somewhere nearby, something snarls in burning agony."));
}