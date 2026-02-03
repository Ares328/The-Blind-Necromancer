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

// Apply turn tests
TEST(CommandTests, ApplyTurnPlayerMoveNoHostiles)
{
	Game game("Ares");

	CommandResult command = game.ApplyTurn("move north");

	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("You move north."));
	EXPECT_EQ(std::string::npos, command.description.find("hostile"));
}

TEST(CommandTests, ApplyTurnPlayerMoveWithHostiles)
{
	Game game("Ares");
	game.SpawnHostileAt(6, 3);
	CommandResult command = game.ApplyTurn("move north");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("You move north."));
	EXPECT_NE(std::string::npos, command.description.find("A hostile shuffles closer from the south."));
}
TEST(CommandTests, ApplyTurnPlayerMoveHostileAdjacentAttacks)
{
	Game game("Ares");

	game.SpawnHostileAt(7, 3);

	CommandResult command = game.ApplyTurn("move south");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("You move south."));
	EXPECT_NE(std::string::npos, command.description.find("A hostile claws at you from the east."));
}
TEST(CommandTests, ApplyTurnPlayerPulseWithHostiles)
{
	Game game("Ares");
	game.SpawnHostileAt(7, 3);
	CommandResult command = game.ApplyTurn("pulse 5");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("Your senses extend outward."));
	EXPECT_NE(std::string::npos, command.description.find("A hostile shuffles closer from the east."));
}
TEST(CommandTests, ApplyTurnPlayerPulseNoHostiles)
{
	Game game("Ares");
	CommandResult command = game.ApplyTurn("pulse 5");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("Your senses extend outward."));
	EXPECT_NE(std::string::npos, command.description.find("The world waits in uneasy silence."));
}
TEST(CommandTests, ApplyTurnHostileFromNorth)
{
	Game game("Ares");

	game.SpawnHostileAt(6, 1);

	CommandResult command = game.ApplyTurn("move south");

	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("You move south."));
	EXPECT_NE(std::string::npos, command.description.find("A hostile shuffles closer from the north."));
}
TEST(CommandTests, ApplyTurnHostileFromSouth)
{
	Game game("Ares");
	game.SpawnHostileAt(6, 3);
	CommandResult command = game.ApplyTurn("move north");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("You move north."));
	EXPECT_NE(std::string::npos, command.description.find("A hostile shuffles closer from the south."));
}
TEST(CommandTests, ApplyTurnHostileFromEast)
{
	Game game("Ares");
	game.SpawnHostileAt(7, 2);
	CommandResult command = game.ApplyTurn("move west");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("You move west."));
	EXPECT_NE(std::string::npos, command.description.find("A hostile shuffles closer from the east."));
}
TEST(CommandTests, ApplyTurnHostileFromWest)
{
	Game game("Ares");
	game.SpawnHostileAt(5, 2);
	CommandResult command = game.ApplyTurn("move east");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("You move east."));
	EXPECT_NE(std::string::npos, command.description.find("A hostile shuffles closer from the west."));
}
TEST(CommandTests, ApplyTurnHostileAttackReducesPlayerHp)
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
TEST(CommandTests, ApplyTurnHostileCanKillPlayerAndSetGameOver)
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