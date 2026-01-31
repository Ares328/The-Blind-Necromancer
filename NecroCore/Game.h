#pragma once

#include <string>
#include <vector>
#include "Entity.h"
#include "Command.h"
#include "PulseResult.h"

namespace NecroCore
{
	class Game
	{
	public:
		Game(const std::string& playerName);

		const std::string& GetPlayerName() const;

		std::string GetCurrentDescription() const;

		PulseResult Pulse() const;
		PulseResult Pulse(int radius) const;

		CommandResult ApplyCommand(const std::string& command);
		CommandResult ParseCommand(const std::string& command);
		CommandResult ExecuteCommand(const CommandResult& command);

		void SpawnHostile();
		void SpawnHostileAt(int x, int y);
		void SpawnFriendly();
		void SpawnFriendlyAt(int x, int y);

	private:
		std::string m_PlayerName;
		std::vector<Entity> m_Entities;
		int m_NextEntityId = 1;
	};
};