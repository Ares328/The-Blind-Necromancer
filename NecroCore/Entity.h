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
		Guard,
		Attack
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
			const int dx = std::abs(x1 - x2);
			const int dy = std::abs(y1 - y2);

			return (dx <= 1 && dy <= 1) && !(dx == 0 && dy == 0);
		}

		bool IsAdjacentTo(int otherX, int otherY) const
		{
			return IsAdjacent(x, y, otherX, otherY);
		}
	};
}