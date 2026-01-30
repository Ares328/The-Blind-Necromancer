#include "Game.h"

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
		PulseResult result;

		for (const Entity& entity : m_Entities)
		{
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
	void Game::SpawnHostile()
	{
		Entity hostileEntity;
		hostileEntity.id = m_NextEntityId++;
		hostileEntity.faction = Faction::Hostile;
		m_Entities.push_back(hostileEntity);
	}
	void Game::SpawnFriendly()
	{
		Entity friendlyEntity;
		friendlyEntity.id = m_NextEntityId++;
		friendlyEntity.faction = Faction::Friendly;
		m_Entities.push_back(friendlyEntity);
	}
}