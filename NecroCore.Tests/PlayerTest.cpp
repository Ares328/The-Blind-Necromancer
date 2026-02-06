#include <gtest/gtest.h>
#include "Game.h"

#include <iostream>

using namespace NecroCore;

TEST(PlayerTest, PlayerStartsAtOrigin)
{
	Game game("Ares");
	Map map = game.GetMap();
	const Player& player = game.GetPlayer();

	EXPECT_EQ(player.x, map.spawnX);
	EXPECT_EQ(player.y, map.spawnY);
}
TEST(PlayerTest, PlayerCanMove)
{
	Game game("Ares");
	Map map = game.GetMap();
	const Player& player = game.GetPlayer();
	EXPECT_EQ(player.x, map.spawnX);
	EXPECT_EQ(player.y, map.spawnY);
	game.MovePlayer(1, -1);
	EXPECT_EQ(player.x, map.spawnX + 1);
	EXPECT_EQ(player.y, map.spawnY - 1);
}
TEST(PlayerTest, PlayerCanMoveMultipleTimes)
{
	Game game("Ares");
	Map map = game.GetMap();
	const Player& player = game.GetPlayer();
	EXPECT_EQ(player.x, map.spawnX);
	EXPECT_EQ(player.y, map.spawnY);
	game.MovePlayer(-1, 1);
	EXPECT_EQ(player.x, map.spawnX - 1);
	EXPECT_EQ(player.y, map.spawnY + 1);
	game.MovePlayer(0, -2);
	EXPECT_EQ(player.x, map.spawnX - 1);
	EXPECT_EQ(player.y, map.spawnY - 1);
}
TEST(PlayerTest, NameStillTheSameAfterMovement)
{
	Game game("Ares");
	EXPECT_EQ(game.GetPlayerName(), "Ares");
	game.MovePlayer(10, 10);
	EXPECT_EQ(game.GetPlayerName(), "Ares");
}
TEST(PlayerTest, MultiplePlayersHaveIndependentPositions)
{
	Game game1("Ares");
	Game game2("Zephyr");
	Map map1 = game1.GetMap();
	Map map2 = game2.GetMap();
	const Player& player1 = game1.GetPlayer();
	const Player& player2 = game2.GetPlayer();
	EXPECT_EQ(player1.x, map1.spawnX);
	EXPECT_EQ(player1.y, map1.spawnY);
	EXPECT_EQ(player2.x, map2.spawnX);
	EXPECT_EQ(player2.y, map2.spawnY);
	game1.MovePlayer(2, 1);
	game2.MovePlayer(-2, -1);
	EXPECT_EQ(player1.x, map1.spawnX + 2);
	EXPECT_EQ(player1.y, map1.spawnY + 1);
	EXPECT_EQ(player2.x, map2.spawnX - 2);
	EXPECT_EQ(player2.y, map2.spawnY - 1);
}
TEST(PlayerTest, PlayerCanReturnToOrigin)
{
	Game game("Ares");
	Map map = game.GetMap();
	const Player& player = game.GetPlayer();
	EXPECT_EQ(player.x, map.spawnX);
	EXPECT_EQ(player.y, map.spawnY);
	game.MovePlayer(3, -1);
	EXPECT_EQ(player.x, map.spawnX + 3);
	EXPECT_EQ(player.y, map.spawnY - 1);
	game.MovePlayer(-3, 1);
	EXPECT_EQ(player.x, map.spawnX);
	EXPECT_EQ(player.y, map.spawnY);
}
TEST(PlayerTest, PlayerMovementDoesNotAffectOtherGameStates)
{
	Game game("Ares");
	game.SpawnHostileAt(6, 3);
	const Player& player = game.GetPlayer();

	auto pulseBeforeMove = game.Pulse(2);
	EXPECT_EQ(pulseBeforeMove.detectedHostileCount, 1);
	EXPECT_EQ(pulseBeforeMove.detectedFriendlyCount, 0);
	game.MovePlayer(-5, -1);
	auto pulseAfterMove = game.Pulse(2);
	EXPECT_EQ(pulseAfterMove.detectedHostileCount, 0);
	EXPECT_EQ(pulseAfterMove.detectedFriendlyCount, 0);
}

TEST(PlayerTest, PlayerMovementBlockedAtAir)
{
	Game game("Ares");
	Map map = game.GetMap();
	const Player& player = game.GetPlayer();
	EXPECT_EQ(player.x, map.spawnX);
	EXPECT_EQ(player.y, map.spawnY);
	auto moveResult = game.MovePlayer(0, -20);
	EXPECT_EQ(player.x, map.spawnX);
	EXPECT_EQ(player.y, map.spawnY);
	EXPECT_EQ(moveResult.oldX, map.spawnX);
	EXPECT_EQ(moveResult.oldY, map.spawnY);
	EXPECT_EQ(moveResult.newX, map.spawnX);
	EXPECT_EQ(moveResult.newY, map.spawnY);
}
TEST(PlayerTest, PlayerMovementBlockedAtWall)
{
	Game game("Ares");
	Map map = game.GetMap();
	const Player& player = game.GetPlayer();
	EXPECT_EQ(player.x, map.spawnX);
	EXPECT_EQ(player.y, map.spawnY);
	auto moveResult = game.MovePlayer(0, 0);
	EXPECT_EQ(player.x, map.spawnX);
	EXPECT_EQ(player.y, map.spawnY);
	EXPECT_EQ(moveResult.oldX, map.spawnX);
	EXPECT_EQ(moveResult.oldY, map.spawnY);
	EXPECT_EQ(moveResult.newX, map.spawnX);
	EXPECT_EQ(moveResult.newY, map.spawnY);
}