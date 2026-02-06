#include <gtest/gtest.h>
#include "Command.h"
#include "Game.h"

using namespace NecroCore;

TEST(TurnTest, ApplyTurnPlayerMoveNoHostiles)
{
	Game game("Ares");

	CommandResult command = game.ApplyTurn("move north");

	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("You move north."));
	EXPECT_EQ(std::string::npos, command.description.find("hostile"));
}

TEST(TurnTest, ApplyTurnPlayerMoveWithHostiles)
{
	Game game("Ares");
	Map map = game.GetMap();
	game.SpawnHostileAt(map.spawnX, map.spawnY + 1);
	CommandResult command = game.ApplyTurn("move north");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("You move north."));
	EXPECT_NE(std::string::npos, command.description.find("A hostile shuffles closer from the south."));
}
TEST(TurnTest, ApplyTurnPlayerMoveHostileAdjacentAttacks)
{
	Game game("Ares");
	Map map = game.GetMap();
	game.SpawnHostileAt(map.spawnX + 1, map.spawnY + 1);
	CommandResult command = game.ApplyTurn("move south");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("You move south."));
	EXPECT_NE(std::string::npos, command.description.find("A hostile claws at you from the east."));
}
TEST(TurnTest, ApplyTurnPlayerPulseWithHostiles)
{
	Game game("Ares");
	Map map = game.GetMap();
	game.SpawnHostileAt(map.spawnX + 3, map.spawnY);
	CommandResult command = game.ApplyTurn("pulse 5");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("Your senses extend outward."));
	EXPECT_NE(std::string::npos, command.description.find("A hostile shuffles closer from the east."));
}
TEST(TurnTest, ApplyTurnPlayerPulseNoHostiles)
{
	Game game("Ares");
	CommandResult command = game.ApplyTurn("pulse 5");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("Your senses extend outward."));
	//	EXPECT_NE(std::string::npos, command.description.find("The world waits in uneasy silence."));
}
TEST(TurnTest, ApplyTurnHostileFromNorth)
{
	Game game("Ares");
	Map map = game.GetMap();

	game.SpawnHostileAt(map.spawnX, map.spawnY - 1);

	CommandResult command = game.ApplyTurn("move south");

	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("You move south."));
	EXPECT_NE(std::string::npos, command.description.find("A hostile shuffles closer from the north."));
}
TEST(TurnTest, ApplyTurnHostileFromSouth)
{
	Game game("Ares");
	Map map = game.GetMap();
	game.SpawnHostileAt(map.spawnX, map.spawnY + 1);
	CommandResult command = game.ApplyTurn("move north");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("You move north."));
	EXPECT_NE(std::string::npos, command.description.find("A hostile shuffles closer from the south."));
}
TEST(TurnTest, ApplyTurnHostileFromEast)
{
	Game game("Ares");
	Map map = game.GetMap();
	game.SpawnHostileAt(map.spawnX + 1, map.spawnY);
	CommandResult command = game.ApplyTurn("move west");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("You move west."));
	EXPECT_NE(std::string::npos, command.description.find("A hostile shuffles closer from the east."));
}
TEST(TurnTest, ApplyTurnHostileFromWest)
{
	Game game("Ares");
	Map map = game.GetMap();
	game.SpawnHostileAt(map.spawnX - 1, map.spawnY);
	CommandResult command = game.ApplyTurn("move east");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("You move east."));
	EXPECT_NE(std::string::npos, command.description.find("A hostile shuffles closer from the west."));
}
TEST(TurnTest, ApplyTurnHostileAttackReducesPlayerHp)
{
	Game game("Ares");

	const Player& playerBefore = game.GetPlayer();
	int initialHp = playerBefore.hp;

	game.SpawnHostileAt(playerBefore.x + 1, playerBefore.y - 1);

	CommandResult command = game.ApplyTurn("move north");

	const Player& playerAfter = game.GetPlayer();

	EXPECT_TRUE(command.success);
	EXPECT_EQ(playerAfter.hp, initialHp - 1);
	EXPECT_NE(std::string::npos, command.description.find("A hostile claws at you from the east."));
	EXPECT_FALSE(command.gameOver);
}
TEST(TurnTest, ApplyTurnHostileCanKillPlayerAndSetGameOver)
{
	Game game("Ares");

	Player& mutablePlayer = const_cast<Player&>(game.GetPlayer());
	mutablePlayer.hp = 1;
	mutablePlayer.maxHp = 10;

	game.SpawnHostileAt(mutablePlayer.x + 1, mutablePlayer.y - 1);

	CommandResult command = game.ApplyTurn("move north");

	const Player& playerAfter = game.GetPlayer();

	EXPECT_EQ(playerAfter.hp, 0);
	EXPECT_TRUE(command.gameOver);
	EXPECT_NE(std::string::npos, command.description.find("You collapse"));
}

TEST(TurnTest, ApplyTurnHostileFromNorthEast)
{
	Game game("Ares");
	Map map = game.GetMap();
	game.SpawnHostileAt(map.spawnX + 1, map.spawnY - 1);
	CommandResult command = game.ApplyTurn("move south");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("You move south."));
	EXPECT_NE(std::string::npos, command.description.find("A hostile shuffles closer from the north-east."));
}
TEST(TurnTest, ApplyTurnHostileFromNorthWest)
{
	Game game("Ares");
	Map map = game.GetMap();
	game.SpawnHostileAt(map.spawnX - 1, map.spawnY - 1);
	CommandResult command = game.ApplyTurn("move south");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("You move south."));
	EXPECT_NE(std::string::npos, command.description.find("A hostile shuffles closer from the north-west."));
}
TEST(TurnTest, ApplyTurnHostileFromSouthEast)
{
	Game game("Ares");
	Map map = game.GetMap();
	game.SpawnHostileAt(map.spawnX + 1, map.spawnY + 1);
	CommandResult command = game.ApplyTurn("move north");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("You move north."));
	EXPECT_NE(std::string::npos, command.description.find("A hostile shuffles closer from the south-east."));
}
TEST(TurnTest, ApplyTurnHostileFromSouthWest)
{
	Game game("Ares");
	Map map = game.GetMap();
	game.SpawnHostileAt(map.spawnX - 1, map.spawnY + 1);
	CommandResult command = game.ApplyTurn("move north");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("You move north."));
	EXPECT_NE(std::string::npos, command.description.find("A hostile shuffles closer from the south-west."));
}