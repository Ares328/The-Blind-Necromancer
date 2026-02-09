#include <gtest/gtest.h>
#include "Command.h"
#include "Game.h"

using namespace NecroCore;

TEST(CommandTests, AttackCommandParsesAndReturnsSuccessOnHit)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();


	game.SpawnHostileAt(player.x, player.y - 1);

	CommandResult command = game.ApplyCommand("attack north");

	EXPECT_TRUE(command.success);
	EXPECT_EQ(command.action, CommandAction::Attack);
	auto it = command.args.find("direction");
	ASSERT_NE(it, command.args.end());
	EXPECT_TRUE(std::holds_alternative<std::string>(it->second));
	EXPECT_EQ(std::get<std::string>(it->second), "north");
	EXPECT_NE(std::string::npos, command.description.find("You deal a blow and hear a grunt north."));
}

TEST(CommandTests, AttackCommandMissesWhenNoHostileInDirection)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();

	CommandResult command = game.ApplyCommand("attack north");

	EXPECT_FALSE(command.success);
	EXPECT_EQ(command.action, CommandAction::Attack);
	EXPECT_NE(std::string::npos, command.description.find("You strike north but hit nothing."));
}

TEST(CommandTests, ApplyTurnAttackCanKillHostile)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();

	game.SpawnHostileWithStatsForTest(player.x, player.y - 1, 1, 1, "Skeleton");

	CommandResult command = game.ApplyTurn("attack north");

	EXPECT_TRUE(command.success);
	EXPECT_EQ(command.action, CommandAction::Attack);
	EXPECT_NE(std::string::npos, command.description.find("You strike north and your foe crumbles into dust."));
}
TEST(CommandTests, ApplyTurnAttackMisses)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();
	game.SpawnHostileAt(player.x + 1, player.y);
	CommandResult command = game.ApplyTurn("attack north");
	EXPECT_FALSE(command.success);
	EXPECT_EQ(command.action, CommandAction::Attack);
	EXPECT_NE(std::string::npos, command.description.find("You strike north but hit nothing."));
}
TEST(CommandTests, ApplyTurnAttackNoDirection)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();
	CommandResult command = game.ApplyTurn("attack");
	EXPECT_FALSE(command.success);
	EXPECT_EQ(command.action, CommandAction::Attack);
	EXPECT_NE(std::string::npos, command.description.find("You lash out blindly at the darkness."));
}
TEST(CommandTests, ApplyTurnAttackInvalidDirection)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();
	CommandResult command = game.ApplyTurn("attack upward");
	EXPECT_FALSE(command.success);
	EXPECT_EQ(command.action, CommandAction::Attack);
	EXPECT_NE(std::string::npos, command.description.find("You swing upward, but the notion of that direction escapes this realm."));
}
TEST(CommandTests, ApplyTurnAttackHostileAttacksBack)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();
	int initialHp = player.hp;
	game.SpawnHostileAt(player.x, player.y - 1);
	CommandResult command = game.ApplyTurn("attack north");
	const Player& playerAfter = game.GetPlayer();
	EXPECT_TRUE(command.success);
	EXPECT_EQ(playerAfter.hp, initialHp - 1);
	EXPECT_NE(std::string::npos, command.description.find("You deal a blow and hear a grunt north."));
	EXPECT_NE(std::string::npos, command.description.find("A hostile claws at you from the north."));
}