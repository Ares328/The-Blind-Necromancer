#pragma once

#include <vector>
#include <string>

namespace NecroCore
{
	enum class TileType
	{
		Empty,
		Floor,
		Wall,
		Door,
	};

	class Map
	{
	public:
		Map() = default;

		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }

		TileType GetTile(int x, int y) const;

		bool IsWalkable(int x, int y) const;

		void LoadFromAscii(const std::vector<std::string>& lines);

		int spawnX = 0;
		int spawnY = 0;

	private:
		int m_Width = 0;
		int m_Height = 0;
		std::vector<TileType> m_Tiles;
	};
}