#include "Game.h"
#include "PulseResult.h"
#include "MoveResult.h"
#include "SummonResult.h"
#include "CastResult.h"
#include "Pathfinding.h"
#include "HostileAISystem.h"
#include "SummonsAISystem.h"
#include "EnvironmentSystem.h"
#include "Spell.h"

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cmath>

inline static std::string StepsPhrase(int distance)
{
	if (distance <= 1)
		return "right next to you";
	if (distance == 2)
		return "1 step away";
	return std::to_string(distance) + " steps away";
}

inline static int Distance(int x1, int y1, int x2, int y2)
{
	return std::max(std::abs(x1 - x2), std::abs(y1 - y2));
}

namespace NecroCore
{
	Game::Game(const std::string& playerName, const std::string& mapName)
		: m_PlayerName(playerName)
	{
		m_Player.name = playerName;
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

		MoveResult result{};
		result.oldX = oldX;
		result.oldY = oldY;

		if (!walkable || !free)
		{
			result.newX = oldX;
			result.newY = oldY;
			result.success = false;
			return result;
		}

		m_Player.MoveTo(newX, newY);

		result.newX = newX;
		result.newY = newY;

		return result;
	}

	MoveResult Game::MoveEntity(Entity& entity, int dx, int dy)
	{
		int oldX = entity.x;
		int oldY = entity.y;

		int newX = oldX + dx;
		int newY = oldY + dy;

		std::cout << "[MoveEntity] from (" << oldX << "," << oldY
			<< ") to (" << newX << "," << newY << ")\n";

		bool walkable = m_Map.IsWalkable(newX, newY);
		bool free = IsTileFree(newX, newY);
		std::cout << "[MoveEntity] IsWalkable(" << newX << "," << newY
			<< ") = " << (walkable ? "true" : "false")
			<< ", IsTileFree = " << (free ? "true" : "false") << "\n";

		MoveResult result{};
		result.oldX = oldX;
		result.oldY = oldY;

		if (!walkable || !free)
		{
			result.newX = oldX;
			result.newY = oldY;
			result.success = false;
			return result;
		}

		entity.MoveTo(newX, newY);

		result.newX = newX;
		result.newY = newY;

		return result;
	}

	Entity* Game::GetEntityById(int id)
	{
		for (Entity& entity : m_Entities)
		{
			if (entity.id == id)
				return &entity;
		}
		return nullptr;
	}

	Entity* Game::GetEntityByName(const std::string& name)
	{
		for (Entity& entity : m_Entities)
		{
			if (entity.name == name)
				return &entity;
		}
		return nullptr;
	}

	std::string Game::GetCurrentDescription() const
	{
		return "You stand at the edge of a realm of darkness, "
			"where the cold whisper of forgotten souls brushes past your ear.";
	}
	PulseResult Game::Pulse() const
	{
		const int defaultRadius = 3;
		return Pulse(defaultRadius);
	}
	PulseResult Game::Pulse(int radius) const
	{
		PulseResult result;
		result.description = "";

		const int playerX = m_Player.x;
		const int playerY = m_Player.y;

		std::vector<std::pair<int, int>> reachable = Pathfinder::BfsReachable(m_Map, playerX, playerY, radius);

		auto isReachable = [&](int x, int y) -> bool
			{
				for (const auto& pos : reachable)
				{
					if (pos.first == x && pos.second == y)
						return true;
				}
				return false;
			};

		bool firstDoor = true;
		for (const auto& pos : reachable)
		{
			const int tx = pos.first;
			const int ty = pos.second;

			if (!m_Map.IsDoor(tx, ty))
				continue;

			const char* dirName = Map::DirectionNameFromPoints(playerX, playerY, tx, ty);
			if (!dirName)
				continue;

			if (firstDoor)
			{
				result.description += " ";
				firstDoor = false;
			}
			else
			{
				result.description += " ";
			}

			const int dist = Distance(playerX, playerY, tx, ty);

			result.description += "\nThere is a door ";
			result.description += StepsPhrase(dist);
			result.description += " to the ";
			result.description += dirName;
			result.description += ".";
		}

		for (const auto& pos : reachable)
		{
			const int tx = pos.first;
			const int ty = pos.second;

			if (!m_Map.IsFireplace(tx, ty))
				continue;

			const char* dirName = Map::DirectionNameFromPoints(playerX, playerY, tx, ty);
			if (!dirName)
				continue;

			if (!HasStatus(m_Map.GetTileState(tx, ty), StatusEffect::OnFire))
				continue;

			const int dist = Distance(playerX, playerY, tx, ty);

			result.description += "\nYou feel gentle warmth and hear a soft crackling ";
			result.description += StepsPhrase(dist);
			result.description += " to the ";
			result.description += dirName;
			result.description += ".";
		}


		// Describe entities within reach
		for (const Entity& entity : m_Entities)
		{
			if (!entity.IsAlive())
				continue;
			if (!isReachable(entity.x, entity.y))
				continue;

			const char* dirName = Map::DirectionNameFromPoints(playerX, playerY, entity.x, entity.y);
			if (!dirName)
				continue;

			result.description += "\nYou sense ";

			if (entity.faction == Faction::Hostile)
			{
				result.description += "a hostile presence ";
				result.detectedHostileCount += 1;
			}
			else if (entity.faction == Faction::Friendly)
			{
				result.description += "a friendly presence ";
				result.detectedFriendlyCount += 1;
			}
			else
			{
				result.description += "a presence ";
			}

			const int dist = Distance(playerX, playerY, entity.x, entity.y);

			result.description += StepsPhrase(dist);
			result.description += " to the ";
			result.description += dirName;
			result.description += ".";
		}


		for (const auto& pos : reachable)
		{
			const int tx = pos.first;
			const int ty = pos.second;

			if (!m_Map.IsTrap(tx, ty))
				continue;

			const char* dirName = Map::DirectionNameFromPoints(playerX, playerY, tx, ty);
			if (!dirName)
				continue;

			StatusEffect trapEffect = m_Map.GetTileState(tx, ty);

			result.description += "\nYou sense ";

			if (HasStatus(trapEffect, StatusEffect::OnFire))
			{
				result.description += "a fire trap ";
				result.detectedTrapCount += 1;
			}
			else if (HasStatus(trapEffect, StatusEffect::Poisoned))
			{
				result.description += "a poison trap ";
				result.detectedTrapCount += 1;
			}
			else
			{
				result.description += "a trap ";
				result.detectedTrapCount += 1;
			}

			const int dist = Distance(playerX, playerY, tx, ty);

			result.description += StepsPhrase(dist);
			result.description += " to the ";
			result.description += dirName;
			result.description += ".";
		}
		return result;
	}
	CastResult Game::CastSpell(const std::string& element, const std::string& direction)
	{
		CastResult result{};

		int dx = 0;
		int dy = 0;
		int tx = 0;
		int ty = 0;

		if (direction == "self")
		{
			tx = m_Player.x;
			ty = m_Player.y;
		}
		else
		{
			if (!m_Map.DirectionFromString(direction, dx, dy))
			{
				return result;
			}

			tx = m_Player.x + dx;
			ty = m_Player.y + dy;
		}
		std::cout << "[CastSpell] Casting " << element << " towards " << direction
			<< " (target tile: " << tx << "," << ty << ")\n";
		const ISpell* spell = FindSpellByName(element);
		if (spell == nullptr)
		{
			result.description = "You cast a spell of " + element + " towards " + direction + ", but nothing happens.";
			return result;
		}
		return spell->Cast(*this, tx, ty, direction);
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
		hostileEntity.name = "Skeleton";
		hostileEntity.attackDamage = 1;
		m_Entities.push_back(hostileEntity);
	}
	void Game::SpawnHostileWithStatsForTest(int x, int y, int hp, int attackDamage, std::string name)
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
		hostileEntity.name = name;
		hostileEntity.attackDamage = attackDamage;
		hostileEntity.aiState = EntityState::Attack;
		m_Entities.push_back(hostileEntity);
	}
	void Game::SpawnFriendlyWithStatsForTest(int x, int y, int hp, int attackDamage, std::string name)
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
		friendlyEntity.name = name;
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
		friendlyEntity.name = "friendly undead";
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
				friendlyEntity.name = "skeleton#" + std::to_string(friendlyEntity.id);

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
		else if (mapName == "fire_place_test")
		{
			map = {
				"#######",
				"#.....#",
				"#..f..#",
				"#.....#",
				"#.....#",
				"#.....#",
				"#######",
			};
			spawnX = 3;
			spawnY = 3;
		}
		else if (mapName == "map1")
		{
			map = {
				"             #####   ",
				"            #.....#  ",
				"#####      #...o...# ",
				"#...#     #.........#",
				"#...#######.........#",
				"#.o....t..+.........#",
				"#...#######.........#   #############",
				"#...#     #.........#   #......+....#",
				"#...#######....o....#   #......######",
				"######..............#   #...........#",
				"     #....t.........#   ######t######",
				"#######+#####+#######        #.#",
				"#........o#.........#        #.#",
				"#......t..#...t.....#        #.#",
				"#+###.....#+####+####        #.#",
				"#...#.....#....#....##########.#",
				"#...f.....#....#....f..........#",
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
				} else if(row[static_cast<std::size_t>(x)] == 't')
				{
					StatusEffect randomEffect = (std::rand() % 2 == 0) ? StatusEffect::OnFire : StatusEffect::Poisoned;
					SpawnTrapAt(x, y, randomEffect);
				}
				else if (row[static_cast<std::size_t>(x)] == 'f')
				{
					SpawnFireplaceAt(x, y, true);
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
	void Game::SpawnDoorAt(int x, int y)
	{
		m_Map.convertTile(x, y, TileType::Door);
	}
	void Game::SpawnTrapAt(int x, int y, StatusEffect trapType)
	{
		m_Map.convertTile(x, y, TileType::Trap);
		m_Map.SetTileState(x, y, trapType);
	}
	void Game::SpawnFireplaceAt(int x, int y, bool isOnFire)
	{
		m_Map.convertTile(x, y, TileType::Fireplace);
		if (isOnFire)
		{
			m_Map.SetTileState(x, y, StatusEffect::OnFire);
		}
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
	static std::string TrapMessageForActor(const Actor& actor, StatusEffect effect)
	{
		switch (effect)
		{
		case StatusEffect::OnFire:
			return "A hidden fire trap erupts beneath " + actor.name + "!";
		case StatusEffect::Poisoned:
			return "A hidden poison trap is sprung beneath " + actor.name + "!";
		default:
			return "A hidden trap is triggered beneath " + actor.name + "!";
		}
	}

	std::string Game::HandleTrapOnActor(Actor& actor)
	{
		const int x = actor.x;
		const int y = actor.y;

		if (!m_Map.IsTrap(x, y))
			return {};

		StatusEffect trapEffect = m_Map.GetTileState(x, y);

		if (trapEffect != StatusEffect::Normal)
		{
			actor.AddStatus(trapEffect);
		}

		m_Map.convertTile(x, y, TileType::Floor);
		m_Map.SetTileState(x, y, StatusEffect::Normal);

		return TrapMessageForActor(actor, trapEffect);
	}
};