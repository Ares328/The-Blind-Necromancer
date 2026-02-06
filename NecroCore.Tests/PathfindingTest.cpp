#include <gtest/gtest.h>
#include "Game.h"
#include "Pathfinding.h"

using namespace NecroCore;

// Map for pathfinding tests:
    //  -- o is player start --
	//		 "#######",
	//		 "#o....#",
	//		 "#.###.#",
	//		 "#.#.#.#",
	//		 "#.###.#",
	//		 "#.....#",
	//		 "#######",


TEST(PathfindingTest, BfsReachableRespectsWalls)
{
	Game game("Ares", "path_test");
	const Map& map = game.GetMap();

	const int startX = map.spawnX;
	const int startY = map.spawnY;

	const int radius = 3;
	auto reachable = Pathfinder::BfsReachable(map, startX, startY, radius);

	auto contains = [&](int x, int y)
		{
			for (const auto& p : reachable)
			{
				if (p.first == x && p.second == y) return true;
			}
			return false;
		};

	EXPECT_TRUE(contains(startX + 1, startY));
	EXPECT_TRUE(contains(startX, startY + 1));

	EXPECT_FALSE(contains(startX + 1, startY + 1));
	EXPECT_FALSE(contains(startX - 1, startY - 1));
	EXPECT_FALSE(contains(startX, startY - 1));
	EXPECT_FALSE(contains(startX - 1, startY));
}

TEST(PathfindingTest, FindFirstStepTowardsAvoidsWalls)
{
	Game game("Ares", "path_test");
	const Map& map = game.GetMap();

	const int startX = map.spawnX;
	const int startY = map.spawnY;
	const int targetX = 5;
	const int targetY = 5;
	game.SpawnHostileAt(targetX, targetY);


	int dx = 0;
	int dy = 0;
	const bool found = Pathfinder::FindFirstStepTowards(
		map, startX, startY, targetX, targetY, dx, dy);

	EXPECT_TRUE(found);

	EXPECT_TRUE(
		(dx == 1 && dy == 0) ||
		(dx == 0 && dy == 1));

	EXPECT_TRUE(map.IsWalkable(startX + dx, startY + dy));
}

TEST(PathfindingTest, FindFirstStepTowardsReturnsFalseWhenBlocked)
{
	Game game("Ares", "path_test");
	const Map& map = game.GetMap();

	const int startX = map.spawnX;
	const int startY = map.spawnY;

	const int blockedX = 3;
	const int blockedY = 3;

	int dx = 0;
	int dy = 0;
	const bool found = Pathfinder::FindFirstStepTowards(
		map, startX, startY, blockedX, blockedY, dx, dy);

	EXPECT_FALSE(found);
}

TEST(PathfindingTest, BfsReachableFromMiddleSegment)
{
	Game game("Ares", "path_test");
	const Map& map = game.GetMap();

	const int startX = 3;
	const int startY = 1;

	const int radius = 2;
	auto reachable = Pathfinder::BfsReachable(map, startX, startY, radius);

	auto contains = [&](int x, int y)
		{
			for (const auto& p : reachable)
			{
				if (p.first == x && p.second == y) return true;
			}
			return false;
		};


	EXPECT_TRUE(contains(startX + 1, startY));
	EXPECT_TRUE(contains(startX - 1, startY));

	EXPECT_FALSE(contains(startX, startY + 1));
}

TEST(PathfindingTest, FirstStepTowardsPlayerLikeSummonWould)
{
	Game game("Ares", "path_test");
	const Map& map = game.GetMap();

	const int playerX = map.spawnX;
	const int playerY = map.spawnY;

	const int summonStartX = playerX + 4;
	const int summonStartY = playerY + 4;
	game.SpawnFriendlyAt(summonStartX, summonStartY);


	int dx = 0;
	int dy = 0;
	const bool found = Pathfinder::FindFirstStepTowards(
		map, summonStartX, summonStartY, playerX, playerY, dx, dy);

	EXPECT_TRUE(found);

	const int nx = summonStartX + dx;
	const int ny = summonStartY + dy;

	EXPECT_TRUE(map.IsWalkable(nx, ny));

	const int oldDist = std::abs(summonStartX - playerX) + std::abs(summonStartY - playerY);
	const int newDist = std::abs(nx - playerX) + std::abs(ny - playerY);
	EXPECT_LT(newDist, oldDist);
}

TEST(PathfindingTest, HostileTracksMovingPlayer)
{
	Game game("Ares", "path_test");
	const Map& map = game.GetMap();

	int playerX = map.spawnX;
	int playerY = map.spawnY;

	int hostileX = playerX + 4;
	int hostileY = playerY + 4;

	for (int turn = 0; turn < 3; ++turn)
	{
		playerX += 1;

		int dx = 0;
		int dy = 0;
		const bool found = Pathfinder::FindFirstStepTowards(
			map, hostileX, hostileY, playerX, playerY, dx, dy);

		EXPECT_TRUE(found);

		const int nx = hostileX + dx;
		const int ny = hostileY + dy;

		EXPECT_TRUE(map.IsWalkable(nx, ny));

		const int oldDist = std::abs(hostileX - playerX) + std::abs(hostileY - playerY);
		const int newDist = std::abs(nx - playerX) + std::abs(ny - playerY);
		EXPECT_LT(newDist, oldDist);

		hostileX = nx;
		hostileY = ny;
	}
}

TEST(PathfindingTest, FindFirstStepTowardsBoxedInPlayerReturnsFalse)
{
	Game game("Ares", "path_test");
	Map& map = const_cast<Map&>(game.GetMap());

	map.convertTile(2, 3, TileType::Wall);
	map.convertTile(4, 3, TileType::Wall);
	map.convertTile(3, 2, TileType::Wall);
	map.convertTile(3, 4, TileType::Wall);

	const int startX = map.spawnX;
	const int startY = map.spawnY;
	const int targetX = 3;
	const int targetY = 3;

	int dx = 0, dy = 0;
	const bool found = Pathfinder::FindFirstStepTowards(
		map, startX, startY, targetX, targetY, dx, dy);

	EXPECT_FALSE(found);
}