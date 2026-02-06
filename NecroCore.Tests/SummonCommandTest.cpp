#include <gtest/gtest.h>
#include "Command.h"
#include "Game.h"

using namespace NecroCore;

TEST(SummonCommandTest, SummonedEntityMovesTowardHostile)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();
	game.SpawnHostileAt(player.x + 3, player.y + 1);
	game.SpawnFriendlyAt(player.x + 1, player.y);
	auto command = game.ApplyTurn("command all attack");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("Your summoned ally stalks a distant foe."));
	EXPECT_NE(std::string::npos, command.description.find("A hostile lashes out at your summoned ally."));
}
TEST(SummonCommandTest, SummonedEntityAttacksHostile)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();
	game.SpawnHostileWithStatsForTest(player.x + 2, player.y, 2, 1);
	game.SpawnFriendlyAt(player.x + 1, player.y);
	auto command = game.ApplyTurn("command all attack");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("Your summoned ally strikes at a foe."));
}
TEST(SummonCommandTest, SummonedEntityAttacksHostileAndGetsAttacked)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();
	game.SpawnHostileWithStatsForTest(player.x + 2, player.y, 2, 1);
	game.SpawnFriendlyAt(player.x + 1, player.y);
	auto command = game.ApplyTurn("command all attack");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("Your summoned ally strikes at a foe."));
	EXPECT_NE(std::string::npos, command.description.find("A hostile lashes out at your summoned ally."));
}
TEST(SummonCommandTest, SummonedEntityKillsHostile)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();
	game.SpawnHostileWithStatsForTest(player.x + 2, player.y, 1, 1);
	game.SpawnFriendlyAt(player.x + 1, player.y);
	auto command = game.ApplyTurn("command all attack");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("Your summoned ally's foe crumbles into dust."));
}
TEST(SummonCommandTest, SummonedEntityDoesNotAttackWhenNoHostiles)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();
	game.SpawnFriendlyAt(player.x + 1, player.y);
	auto command = game.ApplyTurn("command all attack");
	EXPECT_TRUE(command.success);
	EXPECT_EQ(std::string::npos, command.description.find("Your summoned ally strikes at a foe."));
}
TEST(SummonCommandTest, SummonedEntityFollowsPlayer)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();
	game.SpawnFriendlyAt(player.x + 2, player.y);
	auto command = game.ApplyTurn("command all follow");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("Your summoned ally moves closer to you."));
}
TEST(SummonCommandTest, SummonedEntityGuardsPosition)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();
	game.SpawnFriendlyAt(player.x + 1, player.y);
	auto command1 = game.ApplyTurn("command all guard");
	EXPECT_TRUE(command1.success);
	// Move player away
	game.MovePlayer(5, 0);
	auto command2 = game.ApplyTurn("pulse");
	EXPECT_TRUE(command2.success);
	EXPECT_EQ(std::string::npos, command2.description.find("Your summoned ally moves closer to you."));
}
TEST(SummonCommandTest, SummonCommandReturnsFalseOnMissingArgs)
{
	Game game("Ares");
	auto command = game.ApplyCommand("command all");
	EXPECT_FALSE(command.success);
	EXPECT_EQ(command.action, CommandAction::SummonCommand);
}
TEST(SummonCommandTest, SummonCommandReturnsFalseOnUnknownTarget)
{
	Game game("Ares");
	auto command = game.ApplyCommand("command minions attack");
	EXPECT_FALSE(command.success);
	EXPECT_EQ(command.action, CommandAction::SummonCommand);
}
TEST(SummonCommandTest, SummonCommandReturnsFalseOnUnknownOrder)
{
	Game game("Ares");
	auto command = game.ApplyCommand("command all defend");
	EXPECT_FALSE(command.success);
	EXPECT_EQ(command.action, CommandAction::SummonCommand);
}
TEST(SummonCommandTest, SummonCommandReturnsCorrectArgs)
{
	Game game("Ares");
	game.SpawnFriendlyWithStatsForTest(game.GetPlayer().x + 1, game.GetPlayer().y, 1, 1);
	auto command = game.ApplyCommand("command all attack");
	auto itTarget = command.args.find("target");
	ASSERT_NE(itTarget, command.args.end());
	EXPECT_TRUE(std::holds_alternative<std::string>(itTarget->second));
	EXPECT_EQ(std::get<std::string>(itTarget->second), "all");
	auto itOrder = command.args.find("order");
	ASSERT_NE(itOrder, command.args.end());
	EXPECT_TRUE(std::holds_alternative<std::string>(itOrder->second));
	EXPECT_EQ(std::get<std::string>(itOrder->second), "attack");
}
TEST(SummonCommandTest, SummonAttacksWhenSpawned)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();
	game.SpawnHostileWithStatsForTest(player.x + 1, player.y + 1, 2, 1);
	auto summonCommand = game.ApplyCommand("summon skeleton");
	EXPECT_TRUE(summonCommand.success);
	auto turnCommand = game.ApplyTurn("command all attack");
	EXPECT_TRUE(turnCommand.success);
	EXPECT_NE(std::string::npos, turnCommand.description.find("Your summoned ally strikes at a foe."));
}
TEST(SummonCommandTest, SummonCommandNoSummonsResult)
{
	Game game("Ares");
	auto turnCommand = game.ApplyTurn("command all attack");
	EXPECT_FALSE(turnCommand.success);
	EXPECT_NE(std::string::npos, turnCommand.description.find("You have no minions to command."));
}