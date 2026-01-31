#include "Game.h"
#include "Command.h"
#include "PulseResult.h"

#include <sstream>

namespace NecroCore
{
	CommandResult Game::ApplyCommand(const std::string& command)
	{
		CommandResult parsed = ParseCommand(command);
		if (!parsed.success)
		{
			return parsed;
		}

		return ExecuteCommand(parsed);
	}
	CommandResult Game::ParseCommand(const std::string& command)
	{
		CommandResult result;

		std::istringstream iss(command);
		std::string verb;
		if (!(iss >> verb))
		{
			result.description = "Your words have no meaning.";
			result.success = false;
			return result;
		}

		if (verb == "pulse")
		{
			result.action = CommandAction::Pulse;

			int radius = 10;

			if (!(iss >> radius))
			{
				radius = 10;
			}

			result.args["radius"] = radius;
			result.description = "Pulse command parsed.";
			result.success = true;
		}
		else if (verb == "move")
		{
			result.action = CommandAction::Move;

			std::string direction;
			if (!(iss >> direction))
			{
				result.description = "Move where?";
				result.success = false;
				return result;
			}

			result.args["direction"] = direction;
			result.description = "Move command parsed.";
			result.success = true;
		}
		else
		{
			result.action = CommandAction::Unknown;
			result.description = "Unknown command: " + verb;
			result.success = false;
		}

		return result;
	}

	CommandResult Game::ExecuteCommand(const CommandResult& command)
	{
		CommandResult finalResult = command;

		switch (command.action)
		{
			case CommandAction::Pulse:
			{
				int radius = 10;

				auto it = command.args.find("radius");
				if (it != command.args.end() && std::holds_alternative<int>(it->second))
				{
					radius = std::get<int>(it->second);
				}
				PulseResult pulseResult = Pulse(radius);

				finalResult.payload = pulseResult;
				finalResult.description = "Your senses extend outward.";
				finalResult.success = true;
				break;
			}
			case CommandAction::Move:
			{
				std::string direction;

				auto it = command.args.find("direction");
				if (it == command.args.end() || !std::holds_alternative<std::string>(it->second))
				{
					finalResult.description = "You stumble in place.";
					finalResult.success = false;
					break;
				}

				direction = std::get<std::string>(it->second);

				int dx = 0;
				int dy = 0;

				if (direction == "north") { dy = -1; }
				else if (direction == "south") { dy = 1; }
				else if (direction == "west") { dx = -1; }
				else if (direction == "east") { dx = 1; }
				else
				{
					finalResult.description = "You try to move, but the direction confuses you.";
					finalResult.success = false;
					break;
				}

				MovePlayer(dx, dy);

				finalResult.description = "You move " + direction + ".";
				finalResult.success = true;
				break;
			}
			case CommandAction::Unknown:
			default:
				finalResult.description = "You watch as nothing happens.";
				finalResult.success = false;
				break;
		}

		return finalResult;
	}
}