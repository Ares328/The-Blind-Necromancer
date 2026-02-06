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

TEST(PulseTest, PulseCommandDoorToTheNorth)
{
	Game game("Ares");
	Map map = game.GetMap();
	game.SpawnDoorAt(map.spawnX, map.spawnY - 1);
	CommandResult command = game.ApplyCommand("pulse 5");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("There is a door to the north."));
}
TEST(PulseTest, PulseCommandDoorToTheSouth)
{
	Game game("Ares");
	Map map = game.GetMap();
	game.SpawnDoorAt(map.spawnX, map.spawnY + 1);
	CommandResult command = game.ApplyCommand("pulse 5");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("There is a door to the south."));
}
TEST(PulseTest, PulseCommandDoorToTheNorthEast)
{
	Game game("Ares");
	Map map = game.GetMap();
	game.SpawnDoorAt(map.spawnX + 1, map.spawnY - 1);
	CommandResult command = game.ApplyCommand("pulse 5");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("There is a door to the north-east."));
}
TEST(PulseTest, PulseCommandDoorToTheSouthEast)
{
	Game game("Ares");
	Map map = game.GetMap();
	game.SpawnDoorAt(map.spawnX + 1, map.spawnY + 1);
	CommandResult command = game.ApplyCommand("pulse 5");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("There is a door to the south-east."));
}
TEST(PulseTest, PulseCommandDoorToTheEast)
{
	Game game("Ares");
	Map map = game.GetMap();
	game.SpawnDoorAt(map.spawnX + 1, map.spawnY);
	CommandResult command = game.ApplyCommand("pulse 5");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("There is a door to the east."));
}
TEST(PulseTest, PulseCommandDoorToTheWest)
{
	Game game("Ares");
	Map map = game.GetMap();
	game.SpawnDoorAt(map.spawnX - 1, map.spawnY);
	CommandResult command = game.ApplyCommand("pulse 5");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("There is a door to the west."));
}
TEST(PulseTest, PulseCommandDoorToTheSouthWest)
{
	Game game("Ares");
	Map map = game.GetMap();
	game.SpawnDoorAt(map.spawnX - 1, map.spawnY + 1);
	CommandResult command = game.ApplyCommand("pulse 5");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("There is a door to the south-west."));
}
TEST(PulseTest, PulseCommandDoorToTheNorthWest)
{
	Game game("Ares");
	Map map = game.GetMap();
	game.SpawnDoorAt(map.spawnX - 1, map.spawnY - 1);
	CommandResult command = game.ApplyCommand("pulse 5");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("There is a door to the north-west."));
}