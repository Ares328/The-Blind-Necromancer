#include <gtest/gtest.h>
#include "Game.h"

#include <iostream>

using namespace NecroCore;

TEST(PlayerTest, PlayerStartsAtOrigin)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();

	EXPECT_EQ(player.x, 6);
	EXPECT_EQ(player.y, 2);
}
TEST(PlayerTest, PlayerCanMove)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();
	EXPECT_EQ(player.x, 6);
	EXPECT_EQ(player.y, 2);
	game.MovePlayer(1, -1);
	EXPECT_EQ(player.x, 7);
	EXPECT_EQ(player.y, 1);
}
TEST(PlayerTest, PlayerCanMoveMultipleTimes)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();
	EXPECT_EQ(player.x, 6);
	EXPECT_EQ(player.y, 2);
	game.MovePlayer(-1, 1);
	EXPECT_EQ(player.x, 5);
	EXPECT_EQ(player.y, 3);
	game.MovePlayer(0, -2);
	EXPECT_EQ(player.x, 5);
	EXPECT_EQ(player.y, 1);
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
	const Player& player1 = game1.GetPlayer();
	const Player& player2 = game2.GetPlayer();
	EXPECT_EQ(player1.x, 6);
	EXPECT_EQ(player1.y, 2);
	EXPECT_EQ(player2.x, 6);
	EXPECT_EQ(player2.y, 2);
	game1.MovePlayer(2, 1);
	game2.MovePlayer(-2, -1);
	EXPECT_EQ(player1.x, 8);
	EXPECT_EQ(player1.y, 3);
	EXPECT_EQ(player2.x, 4);
	EXPECT_EQ(player2.y, 1);
}
TEST(PlayerTest, PlayerPositionUnaffectedByPulse)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();
	EXPECT_EQ(player.x, 6);
	EXPECT_EQ(player.y, 2);
	game.Pulse();
	EXPECT_EQ(player.x, 6);
	EXPECT_EQ(player.y, 2);
	game.MovePlayer(1, 1);
	EXPECT_EQ(player.x, 7);
	EXPECT_EQ(player.y, 3);
	game.Pulse(5);
	EXPECT_EQ(player.x, 7);
	EXPECT_EQ(player.y, 3);
}
TEST(PlayerTest, PlayerPositionAffectsSeeingEnemies)
{
	Game game("Ares");
	game.SpawnHostileAt(1, 1);
	auto pulseAtOrigin = game.Pulse(5);
	EXPECT_EQ(pulseAtOrigin.detectedHostileCount, 0);
	EXPECT_EQ(pulseAtOrigin.detectedFriendlyCount, 0);
}
TEST(PlayerTest, PlayerCanReturnToOrigin)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();
	EXPECT_EQ(player.x, 6);
	EXPECT_EQ(player.y, 2);
	game.MovePlayer(3, -1);
	EXPECT_EQ(player.x, 9);
	EXPECT_EQ(player.y, 1);
	game.MovePlayer(-3, 1);
	EXPECT_EQ(player.x, 6);
	EXPECT_EQ(player.y, 2);
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
	const Player& player = game.GetPlayer();
	EXPECT_EQ(player.x, 6);
	EXPECT_EQ(player.y, 2);
	auto moveResult = game.MovePlayer(0, -20);
	EXPECT_EQ(player.x, 6);
	EXPECT_EQ(player.y, 2);
	EXPECT_EQ(moveResult.oldX, 6);
	EXPECT_EQ(moveResult.oldY, 2);
	EXPECT_EQ(moveResult.newX, 6);
	EXPECT_EQ(moveResult.newY, 2);
}
TEST(PlayerTest, PlayerMovementBlockedAtWall)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();
	EXPECT_EQ(player.x, 6);
	EXPECT_EQ(player.y, 2);
	auto moveResult = game.MovePlayer(0, 0);
	EXPECT_EQ(player.x, 6);
	EXPECT_EQ(player.y, 2);
	EXPECT_EQ(moveResult.oldX, 6);
	EXPECT_EQ(moveResult.oldY, 2);
	EXPECT_EQ(moveResult.newX, 6);
	EXPECT_EQ(moveResult.newY, 2);
}