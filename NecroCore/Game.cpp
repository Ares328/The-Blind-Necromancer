#include "Game.h"
#include "PulseResult.h"
#include "MoveResult.h"
#include "SummonResult.h"

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
		std::cout << "[MovePlayer] IsWalkable(" << newX << "," << newY
			<< ") = " << (walkable ? "true" : "false") << "\n";

		if (!walkable)
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
	SummonResult Game::SummonFriendlyInFrontPlayer()
	{
		SpawnFriendlyAt(m_Player.x, m_Player.y + 1);

		const Entity& e = m_Entities.back();

		SummonResult result;
		result.summonedEntity = e;

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
		else if (mapName == "map1")
		{


			map = {
				"             #####   ",
				"            #.....#  ",
				"#####      #.......# ",
				"#...#     #.........#",
				"#...#######.........#",
				"#.........+.........#",
				"#...#######.........#   #############",
				"#...#     #.........#   #......+....#",
				"#...#######.........#   #......######",
				"######..............#   #...........#",
				"     #..............#   ######.######",
				"#######+#####+#######        #.#",
				"#.........#.........#        #.#",
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
		ProcessSummonedTurn(playerResult);
		ProcessHostileTurn(playerResult);

		return playerResult;
	}
	void Game::ProcessSummonedTurn(CommandResult& result)
	{
		std::ostringstream oss;
		oss << result.description;

		bool firstAppend = true;
		auto appendSeparator = [&]()
		{
			if (firstAppend)
			{
				oss << "\n";
				firstAppend = false;
			}
			else
			{
				oss << "\n";
			}
		};
		for (Entity& entity : m_Entities)
		{
			if (entity.faction != Faction::Friendly)
			{
				continue;
			}

			switch (entity.aiState)
			{
				case EntityState::Idle:
				{
					break;
				}
				case EntityState::FollowPlayer:
				{
					int dx = 0;
					int dy = 0;
					Entity::ComputeStepTowards(entity.x, entity.y, m_Player.x, m_Player.y, dx, dy);
					const int targetX = entity.x + dx;
					const int targetY = entity.y + dy;

					if (m_Map.IsWalkable(targetX, targetY))
					{
						entity.x = targetX;
						entity.y = targetY;
						appendSeparator();
						oss << "Your summoned ally moves closer to you.";
					}
					break;
				}
				case EntityState::Guard:
				{
					if (entity.x != entity.guardX || entity.y != entity.guardY)
					{
						int dx = 0;
						int dy = 0;
						Entity::ComputeStepTowards(entity.x, entity.y, entity.guardX, entity.guardY, dx, dy);
						const int targetX = entity.x + dx;
						const int targetY = entity.y + dy;

						if (m_Map.IsWalkable(targetX, targetY))
						{
							entity.x = targetX;
							entity.y = targetY;
							appendSeparator();
							oss << "Your summoned ally returns to its post.";
						}
					}
					break;
				}
				case EntityState::Attack:
				{
					Entity* closestHostile = nullptr;
					int bestDistance = std::numeric_limits<int>::max();

					for (Entity& other : m_Entities)
					{
						if (other.faction != Faction::Hostile || other.hp <= 0)
							continue;

						const int dist = std::abs(other.x - entity.x) + std::abs(other.y - entity.y);
						if (dist < bestDistance)
						{
							bestDistance = dist;
							closestHostile = &other;
						}
					}

					if (!closestHostile)
						break;

					if (Entity::IsAdjacent(entity.x, entity.y, closestHostile->x, closestHostile->y))
					{
						closestHostile->hp -= entity.attackDamage;

						if (closestHostile->hp <= 0)
						{
							closestHostile->hp = 0;
							appendSeparator();
							oss << "Your summoned ally's foe crumbles into dust.";
						}
						else {
							appendSeparator();
							oss << "Your summoned ally strikes at a foe.";
						}
					}
					else
					{
						int dx = 0;
						int dy = 0;
						Entity::ComputeStepTowards(entity.x, entity.y, closestHostile->x, closestHostile->y, dx, dy);
						const int targetX = entity.x + dx;
						const int targetY = entity.y + dy;

						if (m_Map.IsWalkable(targetX, targetY))
						{
							entity.x = targetX;
							entity.y = targetY;
							appendSeparator();
							oss << "Your summoned ally stalks a distant foe.";
						}
					}
					break;
				}
				default:
					break;
			}
		}
		std::cout << "[ProcessSummonedTurn] Final description: " << oss.str() << "\n";

		m_Entities.erase(
			std::remove_if(
				m_Entities.begin(),
				m_Entities.end(),
				[](const Entity& e)
				{
					return e.faction == Faction::Hostile && e.hp <= 0;
				}),
			m_Entities.end()
		);

		result.description = oss.str();
	}
	void Game::ProcessHostileTurn(CommandResult& result)
	{
		std::ostringstream oss;
		oss << result.description;

		bool firstAppend = true;
		auto appendSeparator = [&]()
		{
			if (firstAppend)
			{
				oss << "\n";
				firstAppend = false;
			}
			else
			{
				oss << "\n";
			}
		};

		bool anyHostileActed = false;
		bool playerDiedThisTurn = false;
		std::cout << "[ProcessHostileTurn] Processing " << m_Entities.size() << " entities.\n";
		for (Entity& entity : m_Entities)
		{
			if (entity.faction != Faction::Hostile)
			{
				continue;
			}

			switch (entity.aiState)
			{
				case EntityState::Attack:
				{
					const int distanceToPlayer = std::abs(entity.x - m_Player.x) + std::abs(entity.y - m_Player.y);
					const int distanceToClosestSummoned = [&]() {
						int bestDistance = std::numeric_limits<int>::max();
						for (const Entity& other : m_Entities)
						{
							if (other.faction != Faction::Friendly || other.hp <= 0)
								continue;
							const int dist = std::abs(other.x - entity.x) + std::abs(other.y - entity.y);
							if (dist < bestDistance)
							{
								bestDistance = dist;
							}
						}
						return bestDistance;
						}();
					int targetX = m_Player.x;
					int targetY = m_Player.y;
					Entity* targetEntity = nullptr;

					if (distanceToClosestSummoned < distanceToPlayer)
					{
						int bestDistance = std::numeric_limits<int>::max();
						for (Entity& other : m_Entities)
						{
							if (other.faction != Faction::Friendly || other.hp <= 0)
								continue;
							const int dist = std::abs(other.x - entity.x) + std::abs(other.y - entity.y);
							if (dist < bestDistance)
							{
								bestDistance = dist;
								targetEntity = &other;
							}
						}

						if (targetEntity)
						{
							targetX = targetEntity->x;
							targetY = targetEntity->y;
						}
					}

					const bool adjacentToPlayer = Entity::IsAdjacent(entity.x, entity.y, m_Player.x, m_Player.y);
					const bool adjacentToSummoned = targetEntity &&
						Entity::IsAdjacent(entity.x, entity.y, targetEntity->x, targetEntity->y);

					if (adjacentToSummoned && targetEntity)
					{
						anyHostileActed = true;

						targetEntity->hp -= entity.attackDamage;
						if (targetEntity->hp <= 0) {
							targetEntity->hp = 0;
							appendSeparator();
							oss << "A hostile slays your summoned ally.";
						}
						else {
							appendSeparator();
							oss << "A hostile lashes out at your summoned ally.";
						}
						break;
					}
					else if (adjacentToPlayer && !playerDiedThisTurn)
					{
						anyHostileActed = true;

						if (m_Player.hp > 0)
						{
							m_Player.hp -= entity.attackDamage;
							if (m_Player.hp < 0) m_Player.hp = 0;
							if (m_Player.hp == 0) playerDiedThisTurn = true;
						}

						std::string direction;

						const char* dirName = Map::DirectionNameFromPoints(m_Player.x, m_Player.y, entity.x, entity.y);
						if (dirName)
						{
							direction = dirName;
						}

						appendSeparator();
						oss << "A hostile claws at you from the " << direction << ".";
						break;
					}
					int dx = 0, dy = 0;
					Entity::ComputeStepTowards(entity.x, entity.y, targetX, targetY, dx, dy);
					const int targetMoveX = entity.x + dx;
					const int targetMoveY = entity.y + dy;

					if (m_Map.IsWalkable(targetMoveX, targetMoveY) && !playerDiedThisTurn)
					{
						appendSeparator();
						oss << "A hostile shuffles closer from the ";
						std::string direction;
						const char* dirName = Map::DirectionNameFromPoints(m_Player.x, m_Player.y, entity.x, entity.y);
						if (dirName)
						{
							oss << dirName;
						}
						oss << ".";

						entity.x = targetMoveX;
						entity.y = targetMoveY;
						anyHostileActed = true;
					}
					break;
				}
				case EntityState::FollowPlayer:
				case EntityState::Idle:
				case EntityState::Guard:
			default:
				// Not used for hostiles yet
				break;
			}
		}

		// TODO - Random chance for ambient events if no hostiles acted
		if (!anyHostileActed && !playerDiedThisTurn)
		{
			//	appendSeparator();
			//	oss << "The world waits in uneasy silence.";
		}

		m_Entities.erase(
			std::remove_if(
				m_Entities.begin(),
				m_Entities.end(),
				[](const Entity& e)
				{
					return e.faction == Faction::Friendly && e.hp <= 0;
				}),
			m_Entities.end()
		);

		// TODO - Random death message variations
		if (playerDiedThisTurn)
		{
			appendSeparator();
			oss << "You collapse as the last of your strength drains away.";
			result.gameOver = true;
		}

		std::cout << "[ProcessHostileTurn] Final description: " << oss.str() << "\n";

		result.description = oss.str();
	}
}