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
		int x;
		int y;
	};
}