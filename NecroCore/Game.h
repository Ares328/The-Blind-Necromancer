#pragma once

#include <string>
#include <vector>
#include "Entity.h"
#include <variant>
#include <unordered_map>

namespace NecroCore
{
	class Game
	{
	public:
		Game(const std::string& playerName);

		const std::string& GetPlayerName() const;

		std::string GetCurrentDescription() const;

		struct PulseResult
		{
			int detectedHostileCount = 0;
			int detectedFriendlyCount = 0;
		};

		enum class CommandAction {
			Unknown,
			Pulse,
		};

		using ArgValue = std::variant<int, float, std::string, bool>;
		using ArgMap = std::unordered_map<std::string, ArgValue>;
		using CommandPayload = std::variant<std::monostate, PulseResult>;

		struct CommandResult
		{
			CommandAction action = CommandAction::Unknown;
			std::string description;

			ArgMap args;

			CommandPayload payload;

			bool success = false;
		};

		PulseResult Pulse() const;

		PulseResult Pulse(int radius) const;

		CommandResult ApplyCommand(const std::string& command);

		CommandResult ParseCommand(const std::string& command);

		CommandResult ExecuteCommand(const CommandResult& command);

		void SpawnHostile();

		void SpawnHostileAt(int x, int y);

		void SpawnFriendly();

		void SpawnFriendlyAt(int x, int y);

	private:
		std::string m_PlayerName;

		std::vector<Entity> m_Entities;

		int m_NextEntityId = 1;
	};
};