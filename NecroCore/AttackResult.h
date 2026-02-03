#pragma once

namespace NecroCore
{
	struct AttackResult
	{
		bool hit = false;

		int targetEntityId = 0;

		int targetX = 0;
		int targetY = 0;

		int damageDealt = 0;

		bool targetDied = false;
	};
}