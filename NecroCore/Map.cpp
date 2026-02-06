#include "Map.h"

namespace NecroCore
{
	TileType Map::GetTile(int x, int y) const
	{
		if (x < 0 || y < 0 || x >= m_Width || y >= m_Height)
		{
			return TileType::Empty;
		}

		return m_Tiles[static_cast<std::size_t>(y) * m_Width + x];
	}

	bool Map::IsWalkable(int x, int y) const
	{
		TileType tile = GetTile(x, y);
		switch (tile)
		{
		case TileType::Floor:
			return true;
		case TileType::Door:
			return true;
		case TileType::Wall:
			return false;
		case TileType::Empty:
			return false;
		default:
			return false;
		}
	}

	bool Map::IsDoor(int x, int y) const
	{
		TileType tile = GetTile(x, y);
		return tile == TileType::Door;
	}

	void Map::LoadFromAscii(const std::vector<std::string>& lines)
	{
		m_Height = static_cast<int>(lines.size());
		m_Width = m_Height > 0 ? static_cast<int>(lines[0].size()) : 0;
		m_Tiles.clear();
		m_Tiles.reserve(static_cast<std::size_t>(m_Width) * m_Height);

		for (int y = 0; y < m_Height; ++y)
		{
			const std::string& row = lines[static_cast<std::size_t>(y)];
			for (int x = 0; x < m_Width; ++x)
			{
				char c = (x < static_cast<int>(row.size())) ? row[static_cast<std::size_t>(x)] : ' ';
				TileType t = TileType::Empty;

				switch (c)
				{
				case '#': t = TileType::Wall;  break;
				case '.': t = TileType::Floor; break;
				case '+': t = TileType::Door;  break;
				case ' ':
				default:
					t = TileType::Empty;
					break;
				}

				m_Tiles.push_back(t);
			}
		}
	}

	void Map::convertTile(int x, int y, TileType newType)
	{
		if (x < 0 || y < 0 || x >= m_Width || y >= m_Height)
		{
			return;
		}
		m_Tiles[static_cast<std::size_t>(y) * m_Width + x] = newType;
	}

	const char* Map::DirectionNameFromDelta(int dx, int dy)
	{
		if (dx > 0) dx = 1;
		else if (dx < 0) dx = -1;
		if (dy > 0) dy = 1;
		else if (dy < 0) dy = -1;

		for (const auto& dir : dirs)
		{
			if (dir.dx == dx && dir.dy == dy)
			{
				return dir.name;
			}
		}
		return nullptr;
	}

	const char* Map::DirectionNameFromPoints(int fromX, int fromY, int toX, int toY)
	{
		const int dx = toX - fromX;
		const int dy = toY - fromY;
		return DirectionNameFromDelta(dx, dy);
	}
}