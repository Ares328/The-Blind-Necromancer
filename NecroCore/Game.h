#pragma once

#include <string>

namespace NecroCore
{
	class Game
	{
	public:
		Game(const std::string& playerName);

		const std::string& GetPlayerName() const;

		std::string& GetCurrentDescription() const;

	private:
		std::string m_PlayerName;
	};
};