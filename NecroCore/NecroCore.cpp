#include "Game.h"
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
	std::string Game::GetCurrentDescription() const
	{
		return "You stand at the edge of a realm of darkness, "
			"where the cold whisper of forgotten souls brushes past your ear.";
	}
	Game::PulseResult Game::Pulse() const
	{
		const int defaultRadius = 10;
		return Pulse(defaultRadius);
	}
	Game::PulseResult Game::Pulse(int radius) const
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
	Game::CommandResult Game::ApplyCommand(const std::string& command)
	{
		CommandResult parsed = ParseCommand(command);
		if (!parsed.success)
		{
			return parsed;
		}

		return ExecuteCommand(parsed);
	}
	Game::CommandResult Game::ParseCommand(const std::string& command)
	{
		CommandResult result;

		std::istringstream iss(command);
		std::string verb;
		if (!(iss >> verb))
		{
			result.description = "Your words have no meaning.";
			result.success = false;
			return result;
		}

		if (verb == "pulse")
		{
			result.action = CommandAction::Pulse;

			int radius = 10;

			if (!(iss >> radius))
			{
				radius = 10;
			}

			result.args["radius"] = radius;
			result.description = "Pulse command parsed.";
			result.success = true;
		}
		else
		{
			result.action = CommandAction::Unknown;
			result.description = "Unknown command: " + verb;
			result.success = false;
		}

		return result;
	}

	Game::CommandResult Game::ExecuteCommand(const CommandResult& command)
	{
		CommandResult finalResult = command;

		switch (command.action)
		{
		case CommandAction::Pulse:
		{
			int radius = 10;

			auto it = command.args.find("radius");
			if (it != command.args.end() && std::holds_alternative<int>(it->second))
			{
				radius = std::get<int>(it->second);
			}
			PulseResult pulseResult = Pulse(radius);

			finalResult.payload = pulseResult;
			finalResult.description = "Your senses extend outward.";
			finalResult.success = true;
			break;
		}
		case CommandAction::Unknown:
		default:
			finalResult.description = "You watch as nothing happens.";
			finalResult.success = false;
			break;
		}

		return finalResult;
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