#include <gtest/gtest.h>
#include "Game.h"

using namespace NecroCore;

TEST(GameTests, StoresPlayerName)
{
	Game game("Ares");
	EXPECT_EQ(game.GetPlayerName(), "Ares");
}

TEST(GameTests, ReturnsInitialDescription)
{
	Game game("Ares");
	std::string description = game.GetCurrentDescription();
	EXPECT_NE(description.find("realm of darkness"), std::string::npos);
	EXPECT_NE(description.find("cold whisper of forgotten souls"), std::string::npos);
}

TEST(GameTests, InitialPulseDetectsNoEntities)
{
	Game game("Ares");

	auto result = game.Pulse();

	EXPECT_EQ(result.detectedHostileCount, 0);
	EXPECT_EQ(result.detectedFriendlyCount, 0);
}

TEST(GameTests, PulseDetectsHostileAfterSpawning)
{
	Game game("Ares");
	game.SpawnHostile();
	auto result = game.Pulse();
	EXPECT_EQ(result.detectedHostileCount, 1);
	EXPECT_EQ(result.detectedFriendlyCount, 0);
}

TEST(GameTests, PulseDetectsNoHostilesPulseBeforeSpawn)
{
	Game game("Ares");
	auto result = game.Pulse();
	game.SpawnHostile();
	EXPECT_EQ(result.detectedHostileCount, 0);
	EXPECT_EQ(result.detectedFriendlyCount, 0);
}

TEST(GameTests, PulseDetectsMultipleHostiles)
{
	Game game("Ares");
	game.SpawnHostile();
	game.SpawnHostile();
	auto result = game.Pulse();
	EXPECT_EQ(result.detectedHostileCount, 2);
	EXPECT_EQ(result.detectedFriendlyCount, 0);
}

TEST(GameTests, PulseDetectsBothHostilesAndFriendlies)
{
	Game game("Ares");
	game.SpawnHostile();
	game.SpawnHostile();
	game.SpawnFriendly();

	auto result = game.Pulse();

	EXPECT_EQ(result.detectedHostileCount, 2);
	EXPECT_EQ(result.detectedFriendlyCount, 1);
}

TEST(GameTests, PulseDetectsFriendlyAfterSpawning)
{
	Game game("Ares");
	game.SpawnFriendly();
	auto result = game.Pulse();
	EXPECT_EQ(result.detectedHostileCount, 0);
	EXPECT_EQ(result.detectedFriendlyCount, 1);
}

TEST(GameTests, PulseDetectsNoFriendlyPulseBeforeSpawn)
{
	Game game("Ares");
	auto result = game.Pulse();
	game.SpawnFriendly();
	EXPECT_EQ(result.detectedHostileCount, 0);
	EXPECT_EQ(result.detectedFriendlyCount, 0);
}

TEST(GameTests, PulseDetectsMultipleFriendlies)
{
	Game game("Ares");
	game.SpawnFriendly();
	game.SpawnFriendly();
	auto result = game.Pulse();
	EXPECT_EQ(result.detectedHostileCount, 0);
	EXPECT_EQ(result.detectedFriendlyCount, 2);
}