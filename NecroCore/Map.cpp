#include "Map.h"
#include "Status.h"

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

	StatusEffect Map::GetTileState(int x, int y) const
	{
		if (x < 0 || y < 0 || x >= m_Width || y >= m_Height)
		{
			return StatusEffect::Normal;
		}
		return m_TileStates[static_cast<std::size_t>(y) * m_Width + x];
	}
	void Map::SetTileState(int x, int y, StatusEffect newState)
	{
		if (x < 0 || y < 0 || x >= m_Width || y >= m_Height)
		{
			return;
		}
		m_TileStates[static_cast<std::size_t>(y) * m_Width + x] = newState;
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
		case TileType::Trap:
			return true;
		case TileType::Fireplace:
			return true;
		default:
			return false;
		}
	}

	void Map::LoadFromAscii(const std::vector<std::string>& lines)
	{
		m_Height = static_cast<int>(lines.size());
		m_Width = m_Height > 0 ? static_cast<int>(lines[0].size()) : 0;

		m_Tiles.clear();
		m_Tiles.reserve(static_cast<std::size_t>(m_Width) * m_Height);

		m_TileStates.clear();
		m_TileStates.resize(static_cast<std::size_t>(m_Width) * m_Height, StatusEffect::Normal);

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
				case 'o': t = TileType::Floor; break;
				case 'f': t = TileType::Fireplace; break;
				case 't': t = TileType::Trap; break;
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

	bool Map::DirectionFromString(const std::string& dir, int& outDx, int& outDy)
	{
		for (const auto& d : Map::dirs)
		{
			if (dir == d.name)
			{
				outDx = d.dx;
				outDy = d.dy;
				return true;
			}
		}
		return false;
	}

	bool Map::DirectionExists(const std::string& dir) const
	{
		for (const auto& d : Map::dirs)
		{
			if (dir == d.name)
			{
				return true;
			}
		}
		return false;
	}
}