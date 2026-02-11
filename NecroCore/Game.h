#pragma once

#include <string>
#include <vector>
#include <functional>
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
		Game(const std::string& playerName, const std::string& mapName);
		Game(const std::string& playerName);

		const std::string& GetPlayerName() const;

		std::string GetCurrentDescription() const;

		PulseResult Pulse() const;
		PulseResult Pulse(int radius) const;

		CastResult CastSpell(const std::string& element, const std::string& direction);

		CommandResult ApplyCommand(const std::string& command);
		CommandResult ParseCommand(const std::string& command);
		CommandResult ExecuteCommand(const CommandResult& command);
		CommandResult ApplyTurn(const std::string& command);

		void SpawnHostile();
		void SpawnHostileAt(int x, int y);
		void SpawnHostileWithStatsForTest(int x, int y, int hp, int attackDamage, std::string name);
		void SpawnFriendly();
		void SpawnFriendlyAt(int x, int y);
		void SpawnFriendlyWithStatsForTest(int x, int y, int hp, int attackDamage, std::string name);

		void SpawnTrapAt(int x, int y, StatusEffect trapType);
		void SpawnDoorAt(int x, int y);
		void SpawnFireplaceAt(int x, int y, bool isOnFire);

		MoveResult MovePlayer(int dx, int dy);
		MoveResult MoveEntity(Entity& entity, int dx, int dy);

		SummonResult SummonFriendlyNextToPlayer();

		const std::vector<Entity>& GetEntities() const { return m_Entities; }
		std::vector<Entity>& GetEntities() { return m_Entities; }

		const Player& GetPlayer() const { return m_Player; }
		Player& GetPlayer() { return m_Player; }

		Entity* GetEntityById(int id);
		Entity* GetEntityByName(const std::string& name);

		const Map& GetMap() const { return m_Map; }
		Map& GetMap() { return m_Map; }

		bool IsTileFree(int x, int y) const;

		std::string HandleTrapOnActor(Actor& actor);

	private:
		std::string m_PlayerName;
		Player m_Player;
		std::vector<Entity> m_Entities;
		int m_NextEntityId = 1;
		

		Map m_Map;

		void InitializeMap(const std::string& mapName);
	};
};