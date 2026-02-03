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
	EXPECT_EQ(player.x, 6);
	EXPECT_EQ(player.y, 2);
	game.ApplyCommand("move north");
	EXPECT_EQ(player.x, 6);
	EXPECT_EQ(player.y, 1);
	game.ApplyCommand("move east");
	EXPECT_EQ(player.x, 7);
	EXPECT_EQ(player.y, 1);
	game.ApplyCommand("move south");
	EXPECT_EQ(player.x, 7);
	EXPECT_EQ(player.y, 2);
	game.ApplyCommand("move west");
	EXPECT_EQ(player.x, 6);
	EXPECT_EQ(player.y, 2);
}

TEST(CommandTests, MoveCommandReturnsSameAsMovePlayer)
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


TEST(CommandTests, PlayermovementBlockedAtWall)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();
	EXPECT_EQ(player.x, 6);
	EXPECT_EQ(player.y, 2);
	game.ApplyCommand("move north");
	game.ApplyCommand("move north");
	game.ApplyCommand("move north");
	game.ApplyCommand("move north");
	game.ApplyCommand("move north");
	game.ApplyCommand("move north");
	EXPECT_EQ(player.x, 6);
	EXPECT_EQ(player.y, 1);
}

TEST(CommandTests, MoveCommandCorrectTillWall)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();
	EXPECT_EQ(player.x, 6);
	EXPECT_EQ(player.y, 2);
	auto command1 = game.ApplyCommand("move north");
	EXPECT_TRUE(command1.success);
	EXPECT_EQ(player.x, 6);
	EXPECT_EQ(player.y, 1);
	auto command2 = game.ApplyCommand("move north");
	EXPECT_FALSE(command2.success);
	EXPECT_EQ(player.x, 6);
	EXPECT_EQ(player.y, 1);
	auto command3 = game.ApplyCommand("move south");
	EXPECT_TRUE(command3.success);
	EXPECT_EQ(player.x, 6);
	EXPECT_EQ(player.y, 2);
}

// Summon Command Tests

TEST(CommandTests, SummonCommandReturnsSuccess)
{
	Game game("Ares");
	auto command = game.ApplyCommand("summon skeleton");

	EXPECT_TRUE(command.success);
	EXPECT_EQ(command.action, CommandAction::Summon);
}

TEST(CommandTests, SummonCommandReturnsDescription)
{
	Game game("Ares");
	auto command = game.ApplyCommand("summon skeleton");

	EXPECT_EQ(command.description, "You summon a loyal servant from the shadows in front of you.");
}

TEST(CommandTests, SummonCommandReturnsFalseOnMissingCreature)
{
	Game game("Ares");
	auto command = game.ApplyCommand("summon");
	EXPECT_FALSE(command.success);
	EXPECT_EQ(command.action, CommandAction::Summon);
}

TEST(CommandTests, SummonCommandReturnsFalseOnUnknownCreature)
{
	Game game("Ares");
	auto command = game.ApplyCommand("summon dragon");
	EXPECT_FALSE(command.success);
	EXPECT_EQ(command.action, CommandAction::Summon);
}

TEST(CommandTests, SummonCommandReturnsFalseOnMisspelledCommand)
{
	Game game("Ares");
	auto command = game.ApplyCommand("summonn skeleton");
	EXPECT_FALSE(command.success);
	EXPECT_EQ(command.action, CommandAction::Unknown);
}

TEST(CommandTests, SummonCommandReturnsFalseOnNoCreature)
{
	Game game("Ares");
	auto command = game.ApplyCommand("summon ");
	EXPECT_FALSE(command.success);
	EXPECT_EQ(command.action, CommandAction::Summon);
}

TEST(CommandTests, SummonCommandReturnsCorrectArgs)
{
	Game game("Ares");
	auto command = game.ApplyCommand("summon skeleton");

	auto it = command.args.find("creature");
	ASSERT_NE(it, command.args.end());
	EXPECT_TRUE(std::holds_alternative<std::string>(it->second));
	EXPECT_EQ(std::get<std::string>(it->second), "skeleton");
}

TEST(CommandTests, SummonCommandPopulatesPayload)
{
	Game game("Ares");
	auto command = game.ApplyCommand("summon skeleton");

	ASSERT_TRUE(std::holds_alternative<SummonResult>(command.payload));
	SummonResult res = std::get<SummonResult>(command.payload);

	// Summoned in front of player position
	const Player& player = game.GetPlayer();
	EXPECT_EQ(res.x, player.x);
	EXPECT_EQ(res.y, player.y + 1);
	EXPECT_GT(res.entityId, 0);
}