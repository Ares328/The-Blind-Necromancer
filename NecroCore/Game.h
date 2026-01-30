#pragma once

#include <string>
#include <vector>
#include "Entity.h"

namespace NecroCore
{
	class Game
	{
	public:
		Game(const std::string& playerName);

		const std::string& GetPlayerName() const;

		std::string GetCurrentDescription() const;

		struct PulseResult
		{
			int detectedHostileCount = 0;
			int detectedFriendlyCount = 0;
		};

		PulseResult Pulse() const;

		void SpawnHostile();

		void SpawnFriendly();

	private:
		std::string m_PlayerName;

		std::vector<Entity> m_Entities;

		int m_NextEntityId = 1;
	};
};