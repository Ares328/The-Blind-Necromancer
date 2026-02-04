#pragma once

namespace NecroCore
{
	enum class Faction {
		Player,
		Friendly,
		Hostile
	};

	enum class EntityState {
		Idle,
		FollowPlayer,
		AttackPlayer,
		Guard,
		AttackHostiles
	};

	struct Entity {
		Faction faction;
		int id;
		int x;
		int y;
		int attackDamage;
		int aggroRange;
		int hp;
		int maxHp;

		EntityState aiState = EntityState::Idle;
		int guardX = 0;
		int guardY = 0;

		static bool IsAdjacent(int x1, int y1, int x2, int y2)
		{
			return (std::abs(x1 - x2) + std::abs(y1 - y2)) == 1;
		}

		static void ComputeStepTowards(int fromX, int fromY, int toX, int toY, int& outDx, int& outDy)
		{
			outDx = 0;
			outDy = 0;

			if (fromX < toX)      outDx = 1;
			else if (fromX > toX) outDx = -1;

			if (fromY < toY)      outDy = 1;
			else if (fromY > toY) outDy = -1;
		}

		bool IsAdjacentTo(int otherX, int otherY) const
		{
			return IsAdjacent(x, y, otherX, otherY);
		}
	};
}