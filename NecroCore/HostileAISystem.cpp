#include "HostileAISystem.h"
#include "Game.h"
#include "Entity.h"
#include "Pathfinding.h"

#include <sstream>
#include <limits>
#include <cmath>
#include <iostream>

namespace NecroCore
{
	void HostileAISystem::ProcessHostileTurn(Game& game, CommandResult& result)
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

		auto& m_Entities = const_cast<std::vector<Entity>&>(game.GetEntities());
		const Player& m_Player = game.GetPlayer();
		const Map& m_Map = game.GetMap();

		bool anyHostileActed = false;
		bool playerDiedThisTurn = false;
		std::cout << "[ProcessHostileTurn] Processing " << m_Entities.size() << " entities.\n";
		for (Entity& entity : m_Entities)
		{
			if (entity.faction != Faction::Hostile)
			{
				continue;
			}

			switch (entity.aiState)
			{
			case EntityState::Attack:
			{
				HandleHostileAttackAI(game, entity, oss, anyHostileActed, playerDiedThisTurn, appendSeparator);
				break;
			}
			case EntityState::FollowPlayer:
			case EntityState::Idle:
			case EntityState::Guard:
			default:
				// Not used for hostiles yet
				break;
			}
		}

		// TODO - Random chance for ambient events if no hostiles acted
		if (!anyHostileActed && !playerDiedThisTurn)
		{
			//	appendSeparator();
			//	oss << "The world waits in uneasy silence.";
		}

		m_Entities.erase(
			std::remove_if(
				m_Entities.begin(),
				m_Entities.end(),
				[](const Entity& e)
				{
					return e.faction == Faction::Friendly && e.hp <= 0;
				}),
			m_Entities.end()
		);

		// TODO - Random death message variations
		if (playerDiedThisTurn)
		{
			appendSeparator();
			oss << "You collapse as the last of your strength drains away.";
			result.gameOver = true;
		}

		std::cout << "[ProcessHostileTurn] Final description: " << oss.str() << "\n";

		result.description = oss.str();
	}
	bool HostileAISystem::HandleHostileAttackAI(Game& game, Entity& entity, std::ostringstream& oss, bool& anyHostileActed, bool& playerDiedThisTurn, const std::function<void()>& appendSeparator)
	{
		const Map& m_Map = game.GetMap();
		Player& m_Player = const_cast<Player&>(game.GetPlayer());
		std::vector<Entity>& m_Entities = const_cast<std::vector<Entity>&>(game.GetEntities());


		const int distanceToPlayer = std::abs(entity.x - m_Player.x) + std::abs(entity.y - m_Player.y);
		const int distanceToClosestSummoned = [&]() {
			int bestDistance = std::numeric_limits<int>::max();
			for (const Entity& other : m_Entities)
			{
				if (other.faction != Faction::Friendly || other.hp <= 0)
					continue;
				const int dist = std::abs(other.x - entity.x) + std::abs(other.y - entity.y);
				if (dist < bestDistance)
				{
					bestDistance = dist;
				}
			}

			return bestDistance;
			}();
		int targetX = m_Player.x;
		int targetY = m_Player.y;
		Entity* targetEntity = nullptr;

		if (distanceToPlayer > entity.aggroRange && distanceToClosestSummoned > entity.aggroRange)
		{
			return false;
		}

		if (distanceToClosestSummoned < distanceToPlayer)
		{
			int bestDistance = std::numeric_limits<int>::max();
			for (Entity& other : m_Entities)
			{
				if (other.faction != Faction::Friendly || other.hp <= 0)
					continue;
				const int dist = std::abs(other.x - entity.x) + std::abs(other.y - entity.y);
				if (dist < bestDistance)
				{
					bestDistance = dist;
					targetEntity = &other;
				}
			}

			if (targetEntity)
			{
				targetX = targetEntity->x;
				targetY = targetEntity->y;
			}
		}

		const bool adjacentToPlayer = Entity::IsAdjacent(entity.x, entity.y, m_Player.x, m_Player.y);
		const bool adjacentToSummoned = targetEntity &&
			Entity::IsAdjacent(entity.x, entity.y, targetEntity->x, targetEntity->y);

		if (adjacentToSummoned && targetEntity)
		{
			anyHostileActed = true;

			targetEntity->hp -= entity.attackDamage;
			if (targetEntity->hp <= 0) {
				targetEntity->hp = 0;
				appendSeparator();
				oss << "A hostile slays your summoned ally.";
			}
			else {
				appendSeparator();
				oss << "A hostile lashes out at your summoned ally.";
			}
			return true;
		}
		else if (adjacentToPlayer && !playerDiedThisTurn)
		{
			anyHostileActed = true;

			if (m_Player.hp > 0)
			{
				m_Player.hp -= entity.attackDamage;
				if (m_Player.hp < 0) m_Player.hp = 0;
				if (m_Player.hp == 0) playerDiedThisTurn = true;
			}

			std::string direction;

			const char* dirName = Map::DirectionNameFromPoints(m_Player.x, m_Player.y, entity.x, entity.y);
			if (dirName)
			{
				direction = dirName;
			}

			appendSeparator();
			oss << "A hostile claws at you from the " << direction << ".";
			return true;
		}
		int dx = 0, dy = 0;
		const bool foundStep = Pathfinder::FindFirstStepTowards(
			m_Map,
			entity.x, entity.y,
			targetX, targetY,
			dx, dy);

		if (!foundStep)
			return false;
		const int targetMoveX = entity.x + dx;
		const int targetMoveY = entity.y + dy;

		if (m_Map.IsWalkable(targetMoveX, targetMoveY) && game.IsTileFree(targetMoveX, targetMoveY) && !playerDiedThisTurn)
		{
			appendSeparator();
			oss << "A hostile shuffles closer from the ";
			std::string direction;
			const char* dirName = Map::DirectionNameFromPoints(m_Player.x, m_Player.y, entity.x, entity.y);
			if (dirName)
			{
				oss << dirName;
			}
			oss << ".";

			entity.x = targetMoveX;
			entity.y = targetMoveY;
			anyHostileActed = true;
			return true;
		}
		return false;
	}
}