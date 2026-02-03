#pragma once

#include <string>
#include <vector>
#include "Entity.h"
#include "Command.h"
#include "Player.h"
#include "PulseResult.h"
#include "MoveResult.h"
#include "SummonResult.h"
#include "Map.h"

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
		CommandResult ApplyTurn(const std::string& command);

		void SpawnHostile();
		void SpawnHostileAt(int x, int y);
		void SpawnHostileWithStatsForTest(int x, int y, int hp, int attackDamage);
		void SpawnFriendly();
		void SpawnFriendlyAt(int x, int y);

		const Player& GetPlayer() const;
		MoveResult MovePlayer(int dx, int dy);

		SummonResult SummonFriendlyInFrontPlayer();

		const Map& GetMap() const { return m_Map; }

	private:
		std::string m_PlayerName;
		Player m_Player;
		std::vector<Entity> m_Entities;
		int m_NextEntityId = 1;
		

		Map m_Map;

		void InitializeDefaultMap();

		void ProcessHostileTurn(CommandResult& result);
	};
};