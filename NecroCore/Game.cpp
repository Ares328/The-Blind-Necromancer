#include "Game.h"
#include "PulseResult.h"
#include "MoveResult.h"
#include "SummonResult.h"
#include "Pathfinding.h"
#include "HostileAISystem.h"
#include "SummonsAISystem.h"
#include "EnvironmentSystem.h"

#include <iostream>
#include <sstream>
#include <cstdlib>

namespace NecroCore 
{
	Game::Game(const std::string& playerName, const std::string& mapName)
		: m_PlayerName(playerName)
	{
		InitializeMap(mapName);
	}
	Game::Game(const std::string& playerName)
		: Game(playerName, "test_box")
	{
	}
	const std::string& Game::GetPlayerName() const
	{
		return m_PlayerName;
	}
	const Player& Game::GetPlayer() const
	{
		return m_Player;
	}

	MoveResult Game::MovePlayer(int dx, int dy)
	{
		int oldX = m_Player.x;
		int oldY = m_Player.y;

		int newX = oldX + dx;
		int newY = oldY + dy;

		std::cout << "[MovePlayer] from (" << oldX << "," << oldY
			<< ") to (" << newX << "," << newY << ")\n";

		bool walkable = m_Map.IsWalkable(newX, newY);
		bool free = IsTileFree(newX, newY);
		std::cout << "[MovePlayer] IsWalkable(" << newX << "," << newY
			<< ") = " << (walkable ? "true" : "false")
			<< ", IsTileFree = " << (free ? "true" : "false") << "\n";

		if (!walkable || !free)
		{
			MoveResult result;
			result.oldX = oldX;
			result.oldY = oldY;
			result.newX = oldX;
			result.newY = oldY;
			return result;
		}

		m_Player.x = newX;
		m_Player.y = newY;

		MoveResult result;
		result.oldX = oldX;
		result.oldY = oldY;
		result.newX = newX;
		result.newY = newY;

		return result;
	}
	std::string Game::GetCurrentDescription() const
	{
		return "You stand at the edge of a realm of darkness, "
			"where the cold whisper of forgotten souls brushes past your ear.";
	}
	PulseResult Game::Pulse() const
	{
		const int defaultRadius = 10;
		return Pulse(defaultRadius);
	}
	PulseResult Game::Pulse(int radius) const
	{
		PulseResult result;

		const int playerX = m_Player.x;
		const int playerY = m_Player.y;

		for (const Entity& entity : m_Entities)
		{
			int dx = entity.x - playerX;
			int dy = entity.y - playerY;
			int distance = std::abs(dx) + std::abs(dy); // Using Manhattan distance for simplicity
			
			if (distance > radius)
			{
				continue;
			}

			if (entity.faction == Faction::Hostile)
			{
				result.detectedHostileCount++;
			}
			else if (entity.faction == Faction::Friendly)
			{
				result.detectedFriendlyCount++;
			}
		}
		return result;
	}
	void Game::SpawnHostileAt(int x, int y)
	{
		if (!m_Map.IsWalkable(x, y)) return;

		Entity hostileEntity;
		hostileEntity.id = m_NextEntityId++;
		hostileEntity.faction = Faction::Hostile;
		hostileEntity.x = x;
		hostileEntity.y = y;
		hostileEntity.aggroRange = 5;
		hostileEntity.aiState = EntityState::Attack;
		hostileEntity.hp = 10;
		hostileEntity.maxHp = 10;
		hostileEntity.attackDamage = 1;
		m_Entities.push_back(hostileEntity);
	}
	void Game::SpawnHostileWithStatsForTest(int x, int y, int hp, int attackDamage)
	{
		if (!m_Map.IsWalkable(x, y)) return;
		Entity hostileEntity;
		hostileEntity.id = m_NextEntityId++;
		hostileEntity.faction = Faction::Hostile;
		hostileEntity.x = x;
		hostileEntity.y = y;
		hostileEntity.aggroRange = 5;
		hostileEntity.hp = hp;
		hostileEntity.maxHp = hp;
		hostileEntity.attackDamage = attackDamage;
		hostileEntity.aiState = EntityState::Attack;
		m_Entities.push_back(hostileEntity);
	}
	void Game::SpawnFriendlyWithStatsForTest(int x, int y, int hp, int attackDamage)
	{
		if (!m_Map.IsWalkable(x, y)) return;
		Entity friendlyEntity;
		friendlyEntity.id = m_NextEntityId++;
		friendlyEntity.faction = Faction::Friendly;
		friendlyEntity.x = x;
		friendlyEntity.y = y;
		friendlyEntity.aggroRange = 5;
		friendlyEntity.hp = hp;
		friendlyEntity.maxHp = hp;
		friendlyEntity.attackDamage = attackDamage;
		friendlyEntity.aiState = EntityState::FollowPlayer;
		m_Entities.push_back(friendlyEntity);
	}
	void Game::SpawnFriendlyAt(int x, int y)
	{
		if (!m_Map.IsWalkable(x, y)) return;

		Entity friendlyEntity;
		friendlyEntity.id = m_NextEntityId++;
		friendlyEntity.faction = Faction::Friendly;
		friendlyEntity.x = x;
		friendlyEntity.y = y;
		friendlyEntity.aggroRange = 5;
		friendlyEntity.aiState = EntityState::FollowPlayer;
		friendlyEntity.hp = 5;
		friendlyEntity.maxHp = 5;
		friendlyEntity.attackDamage = 1;
		m_Entities.push_back(friendlyEntity);
	}
	void Game::SpawnHostile()
	{
		SpawnHostileAt(m_Player.x, m_Player.y);
	}
	void Game::SpawnFriendly()
	{
		SpawnFriendlyAt(m_Player.x, m_Player.y);
	}
	SummonResult Game::SummonFriendlyNextToPlayer()
	{
		SummonResult result{};

		const int px = m_Player.x;
		const int py = m_Player.y;

		constexpr int frontIndex = 0;

		auto trySummonAt = [&](int sx, int sy) -> bool
		{
			if (!m_Map.IsWalkable(sx, sy))
				return false;
			if (!IsTileFree(sx, sy))
				return false;

			Entity friendlyEntity;
			friendlyEntity.id = m_NextEntityId++;
			friendlyEntity.faction = Faction::Friendly;
			friendlyEntity.x = sx;
			friendlyEntity.y = sy;
			friendlyEntity.aggroRange = 5;
			friendlyEntity.aiState = EntityState::FollowPlayer;
			friendlyEntity.hp = 5;
			friendlyEntity.maxHp = 5;
			friendlyEntity.attackDamage = 1;

			m_Entities.push_back(friendlyEntity);
			result.summonedEntity = friendlyEntity;
			result.summonedDirection = Map::DirectionNameFromPoints(px, py, sx, sy);
			return true;
		};

		const auto& d = Map::dirs[frontIndex];
		const int sx = px + d.dx;
		const int sy = py + d.dy;
		if (trySummonAt(sx, sy))
			return result;

		for (std::size_t i = 0; i < std::size(Map::dirs); ++i)
		{
			if (static_cast<int>(i) == frontIndex)
				continue;

			const auto& d = Map::dirs[i];
			const int sx = px + d.dx;
			const int sy = py + d.dy;

			if (trySummonAt(sx, sy))
				return result;
		}

		result.summonedEntity.id = 0;
		return result;
	}
	void Game::InitializeMap(const std::string& mapName)
	{
		std::vector<std::string> map;
		int spawnX = 0;
		int spawnY = 0;

		if (mapName == "test_box")
		{
			map = {
				"###############",
				"#.............#",
				"#.............#",
				"#.............#",
				"#.............#",
				"#.............#",
				"###############",
			};
			spawnX = 7;
			spawnY = 3;
		}
		else if (mapName == "path_test")
		{

			map = {
				"#######",
				"#.....#",
				"#.###.#",
				"#.#.#.#",
				"#.###.#",
				"#.....#",
				"#######",
			};

			spawnX = 1;
			spawnY = 1;
		}
		else if (mapName == "map1")
		{


			map = {
				"             #####   ",
				"            #.....#  ",
				"#####      #...o...# ",
				"#...#     #.........#",
				"#...#######.........#",
				"#.o.......+.........#",
				"#...#######.........#   #############",
				"#...#     #.........#   #......+....#",
				"#...#######....o....#   #......######",
				"######..............#   #...........#",
				"     #..............#   ######.######",
				"#######+#####+#######        #.#",
				"#........o#.........#        #.#",
				"#.........#.........#        #.#",
				"#+###.....#+####+####        #.#",
				"#...#.....#....#....##########.#",
				"#...#.....#....#....+..........#",
				"################################",
			};

			spawnX = 3;
			spawnY = 15;
		}
		else
		{
			std::cerr << "[InitializeMap] Unknown map: " << mapName << "\n";
			std::exit(EXIT_FAILURE);
		}

		m_Map.LoadFromAscii(map);
		m_Map.spawnX = spawnX;
		m_Map.spawnY = spawnY;
		m_Player.x = spawnX;
		m_Player.y = spawnY;

		for (int y = 0; y < static_cast<int>(map.size()); ++y)
		{
			const std::string& row = map[static_cast<std::size_t>(y)];
			for (int x = 0; x < static_cast<int>(row.size()); ++x)
			{
				if (row[static_cast<std::size_t>(x)] == 'o')
				{
					SpawnHostileAt(x, y);
				}
			}
		}
	}
	bool Game::IsTileFree(int x, int y) const
	{
		if (m_Player.x == x && m_Player.y == y && m_Player.hp > 0)
			return false;

		for (const Entity& e : m_Entities)
		{
			if (e.hp <= 0) continue;
			if (e.x == x && e.y == y)
				return false;
		}
		return true;
	}
	std::string Game::DescribeNearbyDoors(int radius) const
	{
		std::ostringstream oss;
		bool first = true;

		for (const auto& d : m_Map.dirs)
		{
			const int tx = m_Player.x + d.dx;
			const int ty = m_Player.y + d.dy;
			if (m_Map.IsDoor(tx, ty))
			{
				if (!first)
				{
					oss << " ";
				}
				first = false;
				std::cout << "There is a door to the " << d.name << ".\n";
				oss << "There is a door to the " << d.name << ".";
			}
		}

		return oss.str();
	}
	void Game::SpawnDoorAt(int x, int y)
	{
		m_Map.convertTile(x, y, TileType::Door);
	}
	CommandResult Game::ApplyTurn(const std::string& command)
	{
		CommandResult playerResult = ApplyCommand(command);

		if (!playerResult.success)
		{
			return playerResult;
		}
		static SummonsAISystem summonsSystem;
		static HostileAISystem hostileSystem;
		static EnvironmentSystem envSystem;

		summonsSystem.ProcessSummonedTurn(*this, playerResult);
		hostileSystem.ProcessHostileTurn(*this, playerResult);
		envSystem.ApplyTurn(*this, playerResult);

		return playerResult;
	}
}