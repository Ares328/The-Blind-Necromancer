#include "Game.h"
#include "PulseResult.h"

#include <sstream>
#include <cstdlib>

namespace NecroCore 
{
	Game::Game(const std::string& playerName)
		: m_PlayerName(playerName)
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

	void Game::MovePlayer(int dx, int dy)
	{
		m_Player.x += dx;
		m_Player.y += dy;
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

		const int playerX = 0;
		const int playerY = 0;

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
		Entity hostileEntity;
		hostileEntity.id = m_NextEntityId++;
		hostileEntity.faction = Faction::Hostile;
		hostileEntity.x = x;
		hostileEntity.y = y;
		m_Entities.push_back(hostileEntity);
	}
	void Game::SpawnFriendlyAt(int x, int y)
	{
		Entity friendlyEntity;
		friendlyEntity.id = m_NextEntityId++;
		friendlyEntity.faction = Faction::Friendly;
		friendlyEntity.x = x;
		friendlyEntity.y = y;
		m_Entities.push_back(friendlyEntity);
	}
	void Game::SpawnHostile()
	{
		SpawnHostileAt(0, 0);
	}
	void Game::SpawnFriendly()
	{
		SpawnFriendlyAt(0, 0);
	}
}