#pragma once

namespace NecroCore
{
	enum class Faction {
		Player,
		Friendly,
		Hostile
	};

	struct Entity {
		Faction faction;
		int id;
	};
}