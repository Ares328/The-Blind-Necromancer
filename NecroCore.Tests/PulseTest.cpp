#include <gtest/gtest.h>
#include "Command.h"
#include "Game.h"

using namespace NecroCore;

TEST(PulseTest, PulseCommandReturnsSuccess)
{
	Game game("Ares");
	auto command = game.ApplyCommand("pulse");
	EXPECT_TRUE(command.success);
	EXPECT_EQ(command.action, CommandAction::Pulse);
}
TEST(PulseTest, PulseCommandReturnsDescription)
{
	Game game("Ares");
	auto command = game.ApplyCommand("pulse");
	EXPECT_EQ(command.description, "Your senses extend outward.");
}
TEST(PulseTest, PulseCommandReturnsCorrectArgs)
{
	Game game("Ares");
	auto command = game.ApplyCommand("pulse 15");
	auto it = command.args.find("radius");
	ASSERT_NE(it, command.args.end());
	EXPECT_TRUE(std::holds_alternative<int>(it->second));
	EXPECT_EQ(std::get<int>(it->second), 15);
}
TEST(PulseTest, PulseCommandReturnsFalse)
{
	Game game("Ares");
	auto command = game.ApplyCommand("pules 10");
	EXPECT_FALSE(command.success);
	EXPECT_EQ(command.action, CommandAction::Unknown);
}
TEST(PulseTest, PulseCommandReturnsSameAsPulse)
{
	Game game("Ares");

	game.SpawnHostileAt(1, 0);

	auto direct = game.Pulse(5);

	auto command = game.ApplyCommand("pulse 5");

	EXPECT_EQ(direct.detectedHostileCount, std::get<PulseResult>(command.payload).detectedHostileCount);
	EXPECT_EQ(direct.detectedFriendlyCount, std::get<PulseResult>(command.payload).detectedFriendlyCount);
}