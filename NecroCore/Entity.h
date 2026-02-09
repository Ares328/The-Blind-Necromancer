#pragma once

#include <cmath>
#include "Actor.h"

namespace NecroCore
{
	enum class Faction {
		Friendly,
		Hostile,
		Neutral
	};

	enum class EntityState {
		Idle,
		FollowPlayer,
		Guard,
		Attack
	};

	struct Entity : public Actor 
	{
		Faction faction;
		int aggroRange;

		EntityState aiState = EntityState::Idle;
		int guardX = 0;
		int guardY = 0;
	};
}