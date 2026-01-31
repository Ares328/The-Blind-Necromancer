#include <gtest/gtest.h>
#include "Game.h"

using namespace NecroCore;

TEST(PlayerTests, PlayerStartsAtOrigin)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();

	EXPECT_EQ(player.x, 0);
	EXPECT_EQ(player.y, 0);
}
TEST(PlayerTests, PlayerCanMove)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();
	EXPECT_EQ(player.x, 0);
	EXPECT_EQ(player.y, 0);
	game.MovePlayer(5, -3);
	EXPECT_EQ(player.x, 5);
	EXPECT_EQ(player.y, -3);
}
TEST(PlayerTests, PlayerCanMoveMultipleTimes)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();
	EXPECT_EQ(player.x, 0);
	EXPECT_EQ(player.y, 0);
	game.MovePlayer(2, 2);
	EXPECT_EQ(player.x, 2);
	EXPECT_EQ(player.y, 2);
	game.MovePlayer(-1, 4);
	EXPECT_EQ(player.x, 1);
	EXPECT_EQ(player.y, 6);
	game.MovePlayer(0, -6);
	EXPECT_EQ(player.x, 1);
	EXPECT_EQ(player.y, 0);
}
TEST(PlayerTests, NameStillTheSameAfterMovement)
{
	Game game("Ares");
	EXPECT_EQ(game.GetPlayerName(), "Ares");
	game.MovePlayer(10, 10);
	EXPECT_EQ(game.GetPlayerName(), "Ares");
}
TEST(PlayerTests, MultiplePlayersHaveIndependentPositions)
{
	Game game1("Ares");
	Game game2("Zephyr");
	const Player& player1 = game1.GetPlayer();
	const Player& player2 = game2.GetPlayer();
	EXPECT_EQ(player1.x, 0);
	EXPECT_EQ(player1.y, 0);
	EXPECT_EQ(player2.x, 0);
	EXPECT_EQ(player2.y, 0);
	game1.MovePlayer(3, 4);
	game2.MovePlayer(-2, -2);
	EXPECT_EQ(player1.x, 3);
	EXPECT_EQ(player1.y, 4);
	EXPECT_EQ(player2.x, -2);
	EXPECT_EQ(player2.y, -2);
}
TEST(PlayerTests, PlayerPositionUnaffectedByPulse)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();
	EXPECT_EQ(player.x, 0);
	EXPECT_EQ(player.y, 0);
	game.Pulse();
	EXPECT_EQ(player.x, 0);
	EXPECT_EQ(player.y, 0);
	game.MovePlayer(1, 1);
	EXPECT_EQ(player.x, 1);
	EXPECT_EQ(player.y, 1);
	game.Pulse(5);
	EXPECT_EQ(player.x, 1);
	EXPECT_EQ(player.y, 1);
}
TEST(PlayerTests, PlayerPositionAffectsSeeingEnemies)
{
	Game game("Ares");
	game.SpawnHostileAt(2, 2);
	game.SpawnFriendlyAt(8, 8);
	auto pulseRadius5 = game.Pulse(5);
	EXPECT_EQ(pulseRadius5.detectedHostileCount, 1);
	EXPECT_EQ(pulseRadius5.detectedFriendlyCount, 0);
	game.MovePlayer(5, 5);
	auto pulseRadius5AfterMove = game.Pulse(6);
	EXPECT_EQ(pulseRadius5AfterMove.detectedHostileCount, 1);
	EXPECT_EQ(pulseRadius5AfterMove.detectedFriendlyCount, 1);
}
TEST(PlayerTests, PlayerCanReturnToOrigin)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();
	EXPECT_EQ(player.x, 0);
	EXPECT_EQ(player.y, 0);
	game.MovePlayer(10, -10);
	EXPECT_EQ(player.x, 10);
	EXPECT_EQ(player.y, -10);
	game.MovePlayer(-10, 10);
	EXPECT_EQ(player.x, 0);
	EXPECT_EQ(player.y, 0);
}
TEST(PlayerTests, PlayerMovementDoesNotAffectOtherGameStates)
{
	Game game("Ares");
	game.SpawnHostileAt(1, 1);
	auto pulseBeforeMove = game.Pulse(5);
	EXPECT_EQ(pulseBeforeMove.detectedHostileCount, 1);
	EXPECT_EQ(pulseBeforeMove.detectedFriendlyCount, 0);
	game.MovePlayer(100, 100);
	auto pulseAfterMove = game.Pulse(5);
	EXPECT_EQ(pulseAfterMove.detectedHostileCount, 0);
	EXPECT_EQ(pulseAfterMove.detectedFriendlyCount, 0);
}