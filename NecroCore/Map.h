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

	enum class TileState : unsigned char
	{
		Normal = 0,
		OnFire = 1 << 0,
	};

	inline TileState operator|(TileState a, TileState b)
	{
		return static_cast<TileState>(
			static_cast<unsigned char>(a) | static_cast<unsigned char>(b));
	}

	inline TileState& operator|=(TileState& a, TileState b)
	{
		a = a | b;
		return a;
	}

	inline bool HasState(TileState value, TileState flag)
	{
		return (static_cast<unsigned char>(value) &
			static_cast<unsigned char>(flag)) != 0;
	}

	class Map
	{
	public:
		Map() = default;

		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }

		TileType GetTile(int x, int y) const;
		TileState GetTileState(int x, int y) const;

		bool IsWalkable(int x, int y) const;
		bool IsDoor(int x, int y) const;

		void LoadFromAscii(const std::vector<std::string>& lines);

		void convertTile(int x, int y, TileType newType);

		static const char* DirectionNameFromDelta(int dx, int dy);
		static const char* DirectionNameFromPoints(int fromX, int fromY, int toX, int toY);

		int spawnX = 0;
		int spawnY = 0;

		struct Dir { int dx; int dy; const char* name; };
		static constexpr Dir dirs[] = {
			{  0,  -1, "north" },
			{  1, -1, "north-east" },
			{ -1, -1, "north-west" },
			{ 1,  0, "east"  },
			{  -1,  0, "west"  },
			{  1,  1, "south-east" },
			{ -1,  1, "south-west" },
			{  0, 1, "south" },
		};

		void SetOnFire(int x, int y, bool onFire);
		bool IsOnFire(int x, int y) const;

	private:
		int m_Width = 0;
		int m_Height = 0;
		std::vector<TileType> m_Tiles;
		std::vector<TileState> m_TileStates;
	};
}