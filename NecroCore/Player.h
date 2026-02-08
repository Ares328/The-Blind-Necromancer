#pragma once

namespace NecroCore
{
	struct Player
	{
		int x = 0;
		int y = 0;
		int hp = 10;
		int maxHp = 10;

		void MoveTo(int dx, int dy)
		{
			x = dx;
			y = dy;
		}

		void ApplyDamage(int damage)
		{
			hp -= damage;
			if (hp < 0) hp = 0;
		}

		bool IsAlive() const
		{
			return hp > 0;
		}
	};
}