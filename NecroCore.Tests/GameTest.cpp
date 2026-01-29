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
