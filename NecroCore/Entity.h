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

	enum class EntityStatus {
		Normal = 0,
		OnFire = 1 << 0,
	};

	inline EntityStatus operator|(EntityStatus a, EntityStatus b)
	{
		return static_cast<EntityStatus>(
			static_cast<unsigned char>(a) | static_cast<unsigned char>(b));
	}

	inline EntityStatus& operator|=(EntityStatus& a, EntityStatus b)
	{
		a = a | b;
		return a;
	}

	inline bool HasStatus(EntityStatus value, EntityStatus flag)
	{
		return (static_cast<unsigned char>(value) &
			static_cast<unsigned char>(flag)) != 0;
	}

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

		EntityStatus status = EntityStatus::Normal;

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