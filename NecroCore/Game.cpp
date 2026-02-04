#include "Game.h"
#include "PulseResult.h"
#include "MoveResult.h"
#include "SummonResult.h"

#include <iostream>
#include <sstream>
#include <cstdlib>

namespace NecroCore 
{
	Game::Game(const std::string& playerName)
		: m_PlayerName(playerName)
	{
		InitializeDefaultMap();
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
		hostileEntity.aiState = EntityState::AttackPlayer;
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
		hostileEntity.aiState = EntityState::AttackPlayer;
		m_Entities.push_back(hostileEntity);
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
	void Game::InitializeDefaultMap()
	{
		// Player starts at (6,2)
		std::vector<std::string> lines = {
			"##########",
			"#.........#",
			"#.........#",
			"#.........#",
			"##########",
		};

		m_Map.LoadFromAscii(lines);

		m_Player.x = 6;
		m_Player.y = 2;
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
				case EntityState::AttackHostiles:
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
						appendSeparator();
						oss << "Your summoned ally strikes at a foe.";

						if (closestHostile->hp <= 0)
						{
							closestHostile->hp = 0;
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
				case EntityState::AttackPlayer:
				{
					break;
				}
				default:
					break;
			}
		}
		std::cout << "[ProcessSummonedTurn] Final description: " << oss.str() << "\n";

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
				case EntityState::AttackPlayer:
				case EntityState::FollowPlayer:
				{
					const bool adjacent = Entity::IsAdjacent(entity.x, entity.y, m_Player.x, m_Player.y);

					if (adjacent && !playerDiedThisTurn)
					{
						anyHostileActed = true;

						if (m_Player.hp > 0)
						{
							m_Player.hp -= entity.attackDamage;
							if (m_Player.hp < 0) m_Player.hp = 0;
							if (m_Player.hp == 0) playerDiedThisTurn = true;
						}

						std::string direction;
						if (entity.x < m_Player.x) direction = "west";
						else if (entity.x > m_Player.x) direction = "east";
						else if (entity.y < m_Player.y) direction = "north";
						else if (entity.y > m_Player.y) direction = "south";

						appendSeparator();
						oss << "A hostile claws at you from the " << direction << ".";
						break;
					}

					int dx = 0, dy = 0;
					Entity::ComputeStepTowards(entity.x, entity.y, m_Player.x, m_Player.y, dx, dy);
					const int targetX = entity.x + dx;
					const int targetY = entity.y + dy;

					if (m_Map.IsWalkable(targetX, targetY) && !playerDiedThisTurn)
					{
						entity.x = targetX;
						entity.y = targetY;
						anyHostileActed = true;

						appendSeparator();
						oss << "A hostile shuffles closer from the ";
						if (dx < 0)      oss << "east";
						else if (dx > 0) oss << "west";
						else if (dy < 0) oss << "south";
						else if (dy > 0) oss << "north";
						oss << ".";
					}
					break;
				}

				case EntityState::Idle:
				case EntityState::Guard:
				case EntityState::AttackHostiles:
			default:
				// Not used for hostiles yet
				break;
			}
		}

		// TODO - Random chance for ambient events if no hostiles acted
		if (!anyHostileActed && !playerDiedThisTurn)
		{
			appendSeparator();
			oss << "The world waits in uneasy silence.";
		}

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