#include <gtest/gtest.h>
#include "Command.h"
#include "Game.h"

using namespace NecroCore;

//	Pulse Command Tests
TEST(CommandTests, PulseCommandReturnsSuccess)
{
	Game game("Ares");
	auto command = game.ApplyCommand("pulse");
	EXPECT_TRUE(command.success);
	EXPECT_EQ(command.action, CommandAction::Pulse);
}
TEST(CommandTests, PulseCommandReturnsDescription)
{
	Game game("Ares");
	auto command = game.ApplyCommand("pulse");
	EXPECT_EQ(command.description, "Your senses extend outward.");
}
TEST(CommandTests, PulseCommandReturnsCorrectArgs)
{
	Game game("Ares");
	auto command = game.ApplyCommand("pulse 15");
	auto it = command.args.find("radius");
	ASSERT_NE(it, command.args.end());
	EXPECT_TRUE(std::holds_alternative<int>(it->second));
	EXPECT_EQ(std::get<int>(it->second), 15);
}
TEST(CommandTests, PulseCommandReturnsFalse)
{
	Game game("Ares");
	auto command = game.ApplyCommand("pules 10");
	EXPECT_FALSE(command.success);
	EXPECT_EQ(command.action, CommandAction::Unknown);
}
TEST(CommandTests, PulseCommandReturnsSameAsPulse)
{
	Game game("Ares");

	game.SpawnHostileAt(1, 0);

	auto direct = game.Pulse(5);

	auto command = game.ApplyCommand("pulse 5");

	EXPECT_EQ(direct.detectedHostileCount, std::get<PulseResult>(command.payload).detectedHostileCount);
	EXPECT_EQ(direct.detectedFriendlyCount, std::get<PulseResult>(command.payload).detectedFriendlyCount);
}

// Move Command Tests

TEST(CommandTests, MoveCommandReturnsSuccess)
{
	Game game("Ares");
	auto command = game.ApplyCommand("move north");
	EXPECT_TRUE(command.success);
	EXPECT_EQ(command.action, CommandAction::Move);
}
TEST(CommandTests, MoveCommandReturnsDescription)
{
	Game game("Ares");
	auto command = game.ApplyCommand("move east");
	EXPECT_EQ(command.description, "You move east.");
}
TEST(CommandTests, MoveCommandReturnsCorrectArgs)
{
	Game game("Ares");
	auto command = game.ApplyCommand("move south");
	auto it = command.args.find("direction");
	ASSERT_NE(it, command.args.end());
	EXPECT_TRUE(std::holds_alternative<std::string>(it->second));
	EXPECT_EQ(std::get<std::string>(it->second), "south");
}
TEST(CommandTests, MoveCommandReturnsFalseOnInvalidDirection)
{
	Game game("Ares");
	auto command = game.ApplyCommand("move upward");
	EXPECT_FALSE(command.success);
	EXPECT_EQ(command.action, CommandAction::Move);
}
TEST(CommandTests, MoveCommandChangesPlayerPosition)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();
	EXPECT_EQ(player.x, 0);
	EXPECT_EQ(player.y, 0);
	game.ApplyCommand("move north");
	EXPECT_EQ(player.x, 0);
	EXPECT_EQ(player.y, -1);
	game.ApplyCommand("move east");
	EXPECT_EQ(player.x, 1);
	EXPECT_EQ(player.y, -1);
	game.ApplyCommand("move south");
	EXPECT_EQ(player.x, 1);
	EXPECT_EQ(player.y, 0);
	game.ApplyCommand("move west");
	EXPECT_EQ(player.x, 0);
	EXPECT_EQ(player.y, 0);
}