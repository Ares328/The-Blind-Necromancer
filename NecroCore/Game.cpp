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
		m_Entities.push_back(friendlyEntity);
	}
	void Game::SpawnHostile()
	{
		// defaults for testing
		SpawnHostileAt(m_Player.x, m_Player.y);
	}
	void Game::SpawnFriendly()
	{
		// defaults for testing
		SpawnFriendlyAt(m_Player.x, m_Player.y);
	}
	SummonResult Game::SummonFriendlyInFrontPlayer()
	{
		SpawnFriendlyAt(m_Player.x, m_Player.y + 1);

		const Entity& e = m_Entities.back();

		SummonResult result;
		result.entityId = e.id;
		result.x = e.x;
		result.y = e.y;

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

		ProcessHostileTurn(playerResult);

		return playerResult;
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
		std::cout << "[ProcessHostileTurn] Processing " << m_Entities.size() << " entities.\n";
		for (Entity& entity : m_Entities)
		{
			if (entity.faction != Faction::Hostile)
			{
				continue;
			}

			int dx = 0;
			int dy = 0;

			if (entity.x < m_Player.x) dx = 1;
			else if (entity.x > m_Player.x) dx = -1;

			if (entity.y < m_Player.y) dy = 1;
			else if (entity.y > m_Player.y) dy = -1;

			const bool adjacent =
				(std::abs(entity.x - m_Player.x) + std::abs(entity.y - m_Player.y)) == 1;

			if (adjacent)
			{
				anyHostileActed = true;
				std::string direction;
				if (entity.x < m_Player.x) direction = "west";
				else if (entity.x > m_Player.x) direction = "east";
				else if (entity.y < m_Player.y) direction = "north";
				else if (entity.y > m_Player.y) direction = "south";
				appendSeparator();
				oss << "A hostile claws at you from the " << direction << ".";
				continue;
			}

			const int targetX = entity.x + dx;
			const int targetY = entity.y + dy;

			if (m_Map.IsWalkable(targetX, targetY))
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
			else
			{
				// Blocked; flavor text optional
				// anyHostileActed = true;
				// oss << "A hostile bumps into an unseen obstacle.";
			}
		}

		// TODO - Random chance for ambient events if no hostiles acted
		if (!anyHostileActed)
		{
			appendSeparator();
			oss << "The world waits in uneasy silence.";
		}
		std::cout << "[ProcessHostileTurn] Final description: " << oss.str() << "\n";

		result.description = oss.str();
	}
}