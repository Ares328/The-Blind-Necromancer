#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include "PulseResult.h"

namespace NecroCore
{

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
}