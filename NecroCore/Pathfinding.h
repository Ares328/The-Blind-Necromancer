#pragma once

#include <vector>
#include <utility>
#include "Map.h"

namespace NecroCore
{
	class Pathfinder
	{
	public:
		static std::vector<std::pair<int, int>> BfsReachable(
			const Map& map,
			int startX,
			int startY,
			int maxDist);
		
		static bool FindFirstStepTowards(
			const Map& map,
			int fromX,
			int fromY,
			int toX,
			int toY,
			int& outDx,
			int& outDy);
	};
}