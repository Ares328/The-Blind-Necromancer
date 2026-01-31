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

TEST(GameTests, PulseOnlyCountsHostilesAlliesWithinRadius)
{
	Game game("Ares");
	game.SpawnHostileAt(1, 0);
	game.SpawnFriendlyAt(15, 0);
	auto closePulse5 = game.Pulse(5);
	EXPECT_EQ(closePulse5.detectedHostileCount, 1);
	EXPECT_EQ(closePulse5.detectedFriendlyCount, 0);

	auto farPulse15 = game.Pulse(15);
	EXPECT_EQ(farPulse15.detectedHostileCount, 1);
	EXPECT_EQ(farPulse15.detectedFriendlyCount, 1);
}

TEST(GameTests, PulseOnlyCountsWithinRadius)
{
	Game game("Ares");
	game.SpawnHostileAt(3, 4);
	game.SpawnFriendlyAt(6, 8);
	auto pulseRadius10 = game.Pulse(10);
	EXPECT_EQ(pulseRadius10.detectedHostileCount, 1);
	EXPECT_EQ(pulseRadius10.detectedFriendlyCount, 0);
	auto pulseRadius15 = game.Pulse(15);
	EXPECT_EQ(pulseRadius15.detectedHostileCount, 1);
	EXPECT_EQ(pulseRadius15.detectedFriendlyCount, 1);
}