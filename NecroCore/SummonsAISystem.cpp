#include "SummonsAISystem.h"
#include "Game.h"
#include "Entity.h"
#include "Pathfinding.h"

#include <sstream>
#include <limits>
#include <cmath>
#include <iostream>

namespace NecroCore
{
	void SummonsAISystem::ProcessSummonedTurn(Game& game, CommandResult& result)
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

		auto& m_Entities = game.GetEntities();
		Player& m_Player = game.GetPlayer();
		Map& m_Map = game.GetMap();

		for (Entity& entity : m_Entities)
		{
			if (entity.faction != Faction::Friendly)
			{
				continue;
			}

			switch (entity.aiState)
			{
			case EntityState::Idle:
			{
				break;
			}
			case EntityState::FollowPlayer:
			{
				int dx = 0;
				int dy = 0;
				const bool found = Pathfinder::FindFirstStepTowards(
					m_Map,
					entity.x, entity.y,
					m_Player.x, m_Player.y,
					dx, dy);

				if (!found)
				{
					break;
				}
				const int targetX = entity.x + dx;
				const int targetY = entity.y + dy;

				if (m_Map.IsWalkable(targetX, targetY) && game.IsTileFree(targetX, targetY))
				{
					entity.x = targetX;
					entity.y = targetY;
					appendSeparator();
					oss << "Your summoned ally moves closer to you.";
				}
				break;
			}
			case EntityState::Guard:
			{
				if (entity.x != entity.guardX || entity.y != entity.guardY)
				{
					int dx = 0;
					int dy = 0;
					const bool found = Pathfinder::FindFirstStepTowards(
						m_Map,
						entity.x, entity.y,
						entity.guardX, entity.guardY,
						dx, dy);

					if (!found)
						break;

					const int targetX = entity.x + dx;
					const int targetY = entity.y + dy;

					if (m_Map.IsWalkable(targetX, targetY) && game.IsTileFree(targetX, targetY))
					{
						entity.x = targetX;
						entity.y = targetY;
						appendSeparator();
						oss << "Your summoned ally returns to its post.";
					}
				}
				else
				{
					HandleSummonedAttackAI(game, entity, oss, appendSeparator);
				}
				break;
			}
			case EntityState::Attack:
			{
				HandleSummonedAttackAI(game, entity, oss, appendSeparator);
				break;
			}
			default:
				break;
			}
		}
		std::cout << "[ProcessSummonedTurn] Final description: " << oss.str() << "\n";

		m_Entities.erase(
			std::remove_if(
				m_Entities.begin(),
				m_Entities.end(),
				[](const Entity& e)
				{
					return e.faction == Faction::Hostile && e.hp <= 0;
				}),
			m_Entities.end()
		);

		result.description = oss.str();
	}
	bool SummonsAISystem::HandleSummonedAttackAI(Game& game,
		Entity& entity,
		std::ostringstream& oss,
		const std::function<void()>& appendSeparator)
	{
		auto& m_Entities = game.GetEntities();
		Player& m_Player = game.GetPlayer();
		Map& m_Map = game.GetMap();

		Entity* closestHostile = nullptr;
		int bestDistance = std::numeric_limits<int>::max();

		for (Entity& other : m_Entities)
		{
			if (other.faction != Faction::Hostile || other.hp <= 0)
				continue;

			const int dist = std::abs(other.x - entity.x) + std::abs(other.y - entity.y);
			if (dist < bestDistance)
			{
				bestDistance = dist;
				closestHostile = &other;
			}
		}

		if (!closestHostile || bestDistance > entity.aggroRange)
		{
			appendSeparator();
			oss << "Your summoned ally has no targets.";
			return false;
		}

		if (Entity::IsAdjacent(entity.x, entity.y, closestHostile->x, closestHostile->y))
		{
			closestHostile->ApplyDamage(entity.attackDamage);

			if (!closestHostile->IsAlive())
			{
				appendSeparator();
				oss << "Your summoned ally's foe crumbles into dust.";
			}
			else
			{
				appendSeparator();
				oss << "Your summoned ally strikes at a foe.";
			}
			return true;
		}

		int dx = 0;
		int dy = 0;
		const bool foundStep = Pathfinder::FindFirstStepTowards(
			m_Map,
			entity.x, entity.y,
			closestHostile->x, closestHostile->y,
			dx, dy);

		if (!foundStep)
			return false;

		const int targetX = entity.x + dx;
		const int targetY = entity.y + dy;

		if (m_Map.IsWalkable(targetX, targetY) && game.IsTileFree(targetX, targetY))
		{
			entity.x = targetX;
			entity.y = targetY;
			appendSeparator();
			oss << "Your summoned ally stalks a distant foe.";
			std::cout << "[ProcessSummonedTurn] Summoned ally " << entity.id << " moves to ("
				<< entity.x << "," << entity.y << ") while targeting hostile "
				<< closestHostile->id << " at (" << closestHostile->x << "," << closestHostile->y << ")\n";
			return true;
		}

		return false;
	}

}