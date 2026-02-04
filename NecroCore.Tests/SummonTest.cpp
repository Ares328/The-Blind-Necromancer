#include <gtest/gtest.h>
#include "Command.h"
#include "Game.h"

using namespace NecroCore;

TEST(SummonTest, SummonCommandReturnsSuccess)
{
	Game game("Ares");
	auto command = game.ApplyCommand("summon skeleton");

	EXPECT_TRUE(command.success);
	EXPECT_EQ(command.action, CommandAction::Summon);
}

TEST(SummonTest, SummonCommandReturnsDescription)
{
	Game game("Ares");
	auto command = game.ApplyCommand("summon skeleton");

	EXPECT_EQ(command.description, "You summon a loyal servant from the shadows in front of you.");
}

TEST(SummonTest, SummonCommandReturnsFalseOnMissingCreature)
{
	Game game("Ares");
	auto command = game.ApplyCommand("summon");
	EXPECT_FALSE(command.success);
	EXPECT_EQ(command.action, CommandAction::Summon);
}

TEST(SummonTest, SummonCommandReturnsFalseOnUnknownCreature)
{
	Game game("Ares");
	auto command = game.ApplyCommand("summon dragon");
	EXPECT_FALSE(command.success);
	EXPECT_EQ(command.action, CommandAction::Summon);
}

TEST(SummonTest, SummonCommandReturnsFalseOnMisspelledCommand)
{
	Game game("Ares");
	auto command = game.ApplyCommand("summonn skeleton");
	EXPECT_FALSE(command.success);
	EXPECT_EQ(command.action, CommandAction::Unknown);
}

TEST(SummonTest, SummonCommandReturnsFalseOnNoCreature)
{
	Game game("Ares");
	auto command = game.ApplyCommand("summon ");
	EXPECT_FALSE(command.success);
	EXPECT_EQ(command.action, CommandAction::Summon);
}

TEST(SummonTest, SummonCommandReturnsCorrectArgs)
{
	Game game("Ares");
	auto command = game.ApplyCommand("summon skeleton");

	auto it = command.args.find("creature");
	ASSERT_NE(it, command.args.end());
	EXPECT_TRUE(std::holds_alternative<std::string>(it->second));
	EXPECT_EQ(std::get<std::string>(it->second), "skeleton");
}

TEST(SummonTest, SummonCommandPopulatesPayload)
{
	Game game("Ares");
	auto command = game.ApplyCommand("summon skeleton");

	ASSERT_TRUE(std::holds_alternative<SummonResult>(command.payload));
	SummonResult res = std::get<SummonResult>(command.payload);

	const Player& player = game.GetPlayer();
	EXPECT_EQ(res.summonedEntity.x, player.x);
	EXPECT_EQ(res.summonedEntity.y, player.y + 1);
	EXPECT_GT(res.summonedEntity.id, 0);
}
TEST(SummonTest, SummonedEntityHasCorrectFaction)
{
	Game game("Ares");
	auto command = game.ApplyCommand("summon skeleton");
	ASSERT_TRUE(std::holds_alternative<SummonResult>(command.payload));
	SummonResult res = std::get<SummonResult>(command.payload);
	EXPECT_EQ(res.summonedEntity.faction, Faction::Friendly);
}