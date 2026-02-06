#include "Pathfinding.h"
#include <queue>
#include <limits>

namespace NecroCore
{
	std::vector<std::pair<int, int>> Pathfinder::BfsReachable(
		const Map& map,
		int startX,
		int startY,
		int maxDist)
	{
		struct Node { int x, y, dist; };

		std::vector<std::pair<int, int>> reachable;

		const int side = maxDist * 2 + 1;

		bool visited[21][21] = {};

		auto inLocal = [&](int relX, int relY)
			{
				return relX >= -maxDist && relX <= maxDist &&
					relY >= -maxDist && relY <= maxDist;
			};
		auto idx = [&](int rel) { return rel + maxDist; };

		std::queue<Node> q;
		q.push({ startX, startY, 0 });
		visited[idx(0)][idx(0)] = true;

		while (!q.empty())
		{
			Node cur = q.front();
			q.pop();

			reachable.emplace_back(cur.x, cur.y);
			if (cur.dist >= maxDist)
				continue;

			for (const auto& d : Map::dirs)
			{
				const int nx = cur.x + d.dx;
				const int ny = cur.y + d.dy;
				const int relX = nx - startX;
				const int relY = ny - startY;

				if (!inLocal(relX, relY))
					continue;

				if (visited[idx(relX)][idx(relY)])
					continue;

				if (!map.IsWalkable(nx, ny))
					continue;

				visited[idx(relX)][idx(relY)] = true;
				q.push({ nx, ny, cur.dist + 1 });
			}
		}

		return reachable;
	}

	bool Pathfinder::FindFirstStepTowards(
		const Map& map,
		int fromX,
		int fromY,
		int toX,
		int toY,
		int& outDx,
		int& outDy)
	{
		if (fromX == toX && fromY == toY)
		{
			outDx = 0;
			outDy = 0;
			return false;
		}

		struct Node { int x, y; };

		const int width = map.GetWidth();
		const int height = map.GetHeight();

		auto inBounds = [&](int x, int y)
			{
				return x >= 0 && x < width && y >= 0 && y < height;
			};

		if (!inBounds(fromX, fromY))
			return false;
		if (!map.IsWalkable(fromX, fromY))
			return false;

		std::vector<std::vector<bool>> visited(
			height, std::vector<bool>(width, false));
		std::vector<std::vector<std::pair<int, int>>> parent(
			height, std::vector<std::pair<int, int>>(width, { -1, -1 }));

		std::queue<Node> q;
		q.push({ fromX, fromY });
		visited[fromY][fromX] = true;

		bool found = false;

		while (!q.empty() && !found)
		{
			Node cur = q.front();
			q.pop();

			for (const auto& d : Map::dirs)
			{
				const int nx = cur.x + d.dx;
				const int ny = cur.y + d.dy;

				if (!inBounds(nx, ny))
					continue;
				if (visited[ny][nx])
					continue;
				if (!map.IsWalkable(nx, ny))
					continue;

				visited[ny][nx] = true;
				parent[ny][nx] = { cur.x, cur.y };

				if (nx == toX && ny == toY)
				{
					found = true;
					break;
				}

				q.push({ nx, ny });
			}
		}

		if (!found)
			return false;

		int cx = toX;
		int cy = toY;

		while (true)
		{
			auto p = parent[cy][cx];
			if (p.first == -1 && p.second == -1)
				return false;

			if (p.first == fromX && p.second == fromY)
			{
				outDx = cx - fromX;
				outDy = cy - fromY;
				return true;
			}

			cx = p.first;
			cy = p.second;
		}
	}
}