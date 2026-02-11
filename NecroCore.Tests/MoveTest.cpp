#include <gtest/gtest.h>
#include "Command.h"
#include "Game.h"

using namespace NecroCore;

TEST(MoveTest, MoveCommandReturnsSuccess)
{
	Game game("Ares");
	auto command = game.ApplyCommand("move north");
	EXPECT_TRUE(command.success);
	EXPECT_EQ(command.action, CommandAction::Move);
}
TEST(MoveTest, MoveCommandReturnsDescription)
{
	Game game("Ares");
	auto command = game.ApplyCommand("move east");
	EXPECT_EQ(command.description, "You move east.");
}
TEST(MoveTest, MoveCommandReturnsCorrectArgs)
{
	Game game("Ares");
	auto command = game.ApplyCommand("move south");
	auto it = command.args.find("direction");
	ASSERT_NE(it, command.args.end());
	EXPECT_TRUE(std::holds_alternative<std::string>(it->second));
	EXPECT_EQ(std::get<std::string>(it->second), "south");
}
TEST(MoveTest, MoveCommandReturnsFalseOnInvalidDirection)
{
	Game game("Ares");
	auto command = game.ApplyCommand("move upward");
	EXPECT_FALSE(command.success);
	EXPECT_EQ(command.action, CommandAction::Move);
}
TEST(MoveTest, MoveCommandChangesPlayerPosition)
{
	Game game("Ares");
	Map map = game.GetMap();
	const Player& player = game.GetPlayer();
	EXPECT_EQ(player.x, map.spawnX);
	EXPECT_EQ(player.y, map.spawnY);
	game.ApplyCommand("move north");
	EXPECT_EQ(player.x, map.spawnX);
	EXPECT_EQ(player.y, map.spawnY - 1);
	game.ApplyCommand("move east");
	EXPECT_EQ(player.x, map.spawnX + 1);
	EXPECT_EQ(player.y, map.spawnY - 1);
	game.ApplyCommand("move south");
	EXPECT_EQ(player.x, map.spawnX + 1);
	EXPECT_EQ(player.y, map.spawnY);
	game.ApplyCommand("move west");
	EXPECT_EQ(player.x, map.spawnX);
	EXPECT_EQ(player.y, map.spawnY);
}

TEST(MoveTest, MoveCommandReturnsSameAsMovePlayer)
{
	Game funcGame("funcGame");
	auto direct = funcGame.MovePlayer(0, -1);

	Game commGame("commGame");
	auto command = commGame.ApplyCommand("move north");

	MoveResult cmdResult = std::get<MoveResult>(command.payload);
	EXPECT_EQ(direct.oldX, cmdResult.oldX);
	EXPECT_EQ(direct.oldY, cmdResult.oldY);
	EXPECT_EQ(direct.newX, cmdResult.newX);
	EXPECT_EQ(direct.newY, cmdResult.newY);
}


TEST(MoveTest, PlayermovementBlockedAtWall)
{
	Game game("Ares");
	Map map = game.GetMap();
	const Player& player = game.GetPlayer();
	EXPECT_EQ(player.x, map.spawnX);
	EXPECT_EQ(player.y, map.spawnY);
	game.ApplyCommand("move north");
	game.ApplyCommand("move north");
	game.ApplyCommand("move north");
	game.ApplyCommand("move north");
	game.ApplyCommand("move north");
	game.ApplyCommand("move north");
	EXPECT_EQ(player.x, map.spawnX);
	EXPECT_EQ(player.y, map.spawnY - 2);
}

TEST(MoveTest, MoveCommandCorrectTillWall)
{
	Game game("Ares");
	Map map = game.GetMap();
	const Player& player = game.GetPlayer();
	EXPECT_EQ(player.x, map.spawnX);
	EXPECT_EQ(player.y, map.spawnY);
	auto command1 = game.ApplyCommand("move north");
	EXPECT_TRUE(command1.success);
	EXPECT_EQ(player.x, map.spawnX);
	EXPECT_EQ(player.y, map.spawnY - 1);
	auto command2 = game.ApplyCommand("move north");
	EXPECT_TRUE(command2.success);
	EXPECT_EQ(player.x, map.spawnX);
	EXPECT_EQ(player.y, map.spawnY - 2);
	auto command3 = game.ApplyCommand("move north");
	EXPECT_FALSE(command3.success);
	EXPECT_EQ(player.x, map.spawnX);
	EXPECT_EQ(player.y, map.spawnY - 2);
	auto command4 = game.ApplyCommand("move south");
	EXPECT_TRUE(command4.success);
	EXPECT_EQ(player.x, map.spawnX);
	EXPECT_EQ(player.y, map.spawnY - 1);
}

TEST(MoveTest, MoveCommandWorksAtActiveFireplace)
{
	Game game("Ares", "fire_place_test");

	CommandResult command = game.ApplyCommand("move north");
	std::cout << command.description << "\n";
	EXPECT_TRUE(command.success);
}