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
		result.detectedHostileCount = m_HostileCount;
		result.detectedFriendlyCount = m_FriendlyCount;
		return result;
	}
	void Game::SpawnHostile()
	{
		m_HostileCount += 1;
	}
	void Game::SpawnFriendly()
	{
		m_FriendlyCount += 1;
	}
}