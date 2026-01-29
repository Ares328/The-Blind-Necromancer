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
	std::string& Game::GetCurrentDescription() const
	{
		static std::string description = "You find yourself in a realm of darkness, where the cold whisper of forgotten souls echoes around you.";
		return description;
	}
}