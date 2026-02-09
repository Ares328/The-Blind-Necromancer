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

	EXPECT_EQ(command.description, "You summon a loyal servant from the shadows north of you.");
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
	EXPECT_EQ(res.summonedEntity.y, player.y - 1);
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
TEST(SummonTest, SummonedEntitySpawnsNorthOfYou)
{
	Game game("Ares");
	auto command = game.ApplyCommand("summon skeleton");
	ASSERT_TRUE(std::holds_alternative<SummonResult>(command.payload));
	SummonResult res = std::get<SummonResult>(command.payload);
	EXPECT_NE(std::string::npos, command.description.find("You summon a loyal servant from the shadows north of you."));
}
TEST(SummonTest, SummonedEntitySpawnsEastOfYouIfNorthBlocked)
{
	Game game("Ares");
	Map& map = const_cast<Map&>(game.GetMap());
	const Player& player = game.GetPlayer();
	map.convertTile(player.x, player.y - 1, TileType::Wall);
	map.convertTile(player.x + 1, player.y - 1, TileType::Wall);
	map.convertTile(player.x - 1, player.y - 1, TileType::Wall);
	auto command = game.ApplyCommand("summon skeleton");
	ASSERT_TRUE(std::holds_alternative<SummonResult>(command.payload));
	SummonResult res = std::get<SummonResult>(command.payload);
	EXPECT_NE(std::string::npos, command.description.find("You summon a loyal servant from the shadows east of you."));
}
TEST(SummonTest, SummonedEntitySpawnsEastOfYouIfNorthBlockedByHostile)
{
	Game game("Ares");
	Map& map = const_cast<Map&>(game.GetMap());
	const Player& player = game.GetPlayer();
	game.SpawnHostileAt(player.x, player.y - 1);
	map.convertTile(player.x + 1, player.y - 1, TileType::Wall);
	map.convertTile(player.x - 1, player.y - 1, TileType::Wall);
	auto command = game.ApplyCommand("summon skeleton");
	ASSERT_TRUE(std::holds_alternative<SummonResult>(command.payload));
	SummonResult res = std::get<SummonResult>(command.payload);
	EXPECT_NE(std::string::npos, command.description.find("You summon a loyal servant from the shadows east of you."));
}
TEST(SummonTest, SummonedEntitySpawnsNorthEastOfYouIfNorthIsBlockedBySummon)
{
	Game game("Ares");
	Map& map = const_cast<Map&>(game.GetMap());
	const Player& player = game.GetPlayer();
	game.SpawnFriendlyAt(player.x, player.y - 1);
	map.convertTile(player.x - 1, player.y - 1, TileType::Wall);
	auto command = game.ApplyCommand("summon skeleton");
	ASSERT_TRUE(std::holds_alternative<SummonResult>(command.payload));
	SummonResult res = std::get<SummonResult>(command.payload);
	EXPECT_NE(std::string::npos, command.description.find("You summon a loyal servant from the shadows north-east of you."));
}
TEST(SummonTest, SummonedEntitySpawnFailOnAvailableTile)
{
	Game game("Ares");
	Map& map = const_cast<Map&>(game.GetMap());
	const Player& player = game.GetPlayer();
	map.convertTile(player.x + 1, player.y, TileType::Wall);
	map.convertTile(player.x + 1, player.y + 1, TileType::Wall);
	map.convertTile(player.x, player.y + 1, TileType::Wall);
	map.convertTile(player.x - 1, player.y + 1, TileType::Wall);
	map.convertTile(player.x - 1, player.y, TileType::Wall);
	map.convertTile(player.x - 1, player.y - 1, TileType::Wall);
	map.convertTile(player.x, player.y - 1, TileType::Wall);
	map.convertTile(player.x + 1, player.y - 1, TileType::Wall);
	auto command = game.ApplyCommand("summon skeleton");
	EXPECT_FALSE(command.success);
	EXPECT_EQ(command.description, "Your power strains, no spirit can take form here.");
}
TEST(SummonTest, SummonedEntityDiesAfterAttack)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();
	game.SpawnHostileWithStatsForTest(player.x + 2, player.y + 1, 2, 1, "Skeleton");
	game.SpawnFriendlyWithStatsForTest(player.x + 1, player.y + 1, 1, 1, "friendly undead");
	auto command = game.ApplyTurn("command all guard");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("A hostile slays your summoned ally."));
}

TEST(SummonTest, SummonedEntityWontAttackBeyondAggroRange)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();
	game.SpawnHostileWithStatsForTest(1, 1, 2, 1, "Skeleton");
	game.SpawnFriendlyWithStatsForTest(5, 9, 1, 1, "friendly undead");
	auto command = game.ApplyTurn("command all attack");
	EXPECT_FALSE(command.success);
	EXPECT_EQ(std::string::npos, command.description.find("Your summoned ally has no targets."));
}
TEST(SummonTest, SummonedEntityWillAttackWithinAggroRange)
{
	Game game("Ares");
	const Player& player = game.GetPlayer();
	game.SpawnHostileWithStatsForTest(player.x + 2, player.y + 1, 2, 1, "Skeleton");
	game.SpawnFriendlyWithStatsForTest(player.x + 1, player.y + 1, 1, 1, "friendly undead");
	auto command = game.ApplyTurn("command all attack");
	EXPECT_TRUE(command.success);
	EXPECT_NE(std::string::npos, command.description.find("Your summoned ally strikes at a foe."));
}