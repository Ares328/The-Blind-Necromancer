#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include "PulseResult.h"
#include "MoveResult.h"
#include "SummonResult.h"
#include "AttackResult.h"
#include "CastResult.h"
#include "SummonCommandResult.h"

namespace NecroCore
{

	enum class CommandAction {
		Unknown,
		Pulse,
		Move,
		Summon,
		Attack,
		SummonCommand,
		Cast,
		Help
	};

	using ArgValue = std::variant<int, float, std::string, bool>;
	using ArgMap = std::unordered_map<std::string, ArgValue>;

	using CommandPayload = std::variant<std::monostate, PulseResult, MoveResult, SummonResult, AttackResult, SummonCommandResult, CastResult>;

	struct CommandResult
	{
		CommandAction action = CommandAction::Unknown;
		std::string description;

		ArgMap args;

		CommandPayload payload;

		bool success = false;
		bool gameOver = false;
	};
}