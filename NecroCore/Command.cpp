#include "Game.h"
#include "Command.h"
#include "PulseResult.h"
#include "MoveResult.h"
#include "SummonResult.h"
#include "AttackResult.h"
#include "SummonCommandResult.h"

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
		else if (verb == "summon")
		{
			result.action = CommandAction::Summon;

			std::string creature;
			if (!(iss >> creature))
			{
				result.description = "Summon what?";
				result.success = false;
				return result;
			}

			result.args["creature"] = creature;
			result.description = "Summon command parsed.";
			result.success = true;
		}
		else if (verb == "attack")
		{
			result.action = CommandAction::Attack;

			std::string direction;
			if (!(iss >> direction))
			{
				result.description = "You lash out blindly at the darkness.";
				result.success = false;
				return result;
			}

			result.args["direction"] = direction;
			result.description = "Attack command parsed.";
			result.success = true;
		}
		else if (verb == "command")
		{
			result.action = CommandAction::SummonCommand;

			std::string target;
			std::string order;
			if (!(iss >> target >> order))
			{
				result.description = "Your will is unclear; your minions hesitate.";
				result.success = false;
				return result;
			}

			result.args["target"] = target; // "all" | "skeleton#1"
			result.args["order"] = order;  // "follow" | "guard" | "attack"
			result.description = "Command parsed.";
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

				MoveResult moveResult = MovePlayer(dx, dy);
				if (moveResult.newX == moveResult.oldX && moveResult.newY == moveResult.oldY)
				{
					finalResult.description = "You bump into an obstacle and cannot move " + direction + ".";
					finalResult.success = false;
					break;
				}
				finalResult.payload = moveResult;
				finalResult.description = "You move " + direction + ".";
				finalResult.success = true;
				break;
			}
			case CommandAction::Summon:
			{
				auto it = command.args.find("creature");
				if (it == command.args.end() || !std::holds_alternative<std::string>(it->second))
				{
					finalResult.description = "You mutter nonsense; nothing answers.";
					finalResult.success = false;
					break;
				}

				const std::string& creature = std::get<std::string>(it->second);

				if (creature != "skeleton")
				{
					finalResult.description = "Your invocation falters: such a creature does not heed your call.";
					finalResult.success = false;
					break;
				}

				SummonResult summonResult = SummonFriendlyInFrontPlayer();

				finalResult.payload = summonResult;
				finalResult.description = "You summon a loyal servant from the shadows in front of you.";
				finalResult.success = true;
				break;
			}
			case CommandAction::Attack:
			{
				std::string direction;

				auto it = command.args.find("direction");
				if (it == command.args.end() || !std::holds_alternative<std::string>(it->second))
				{
					finalResult.description = "You lash out blindly at the darkness.";
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
					finalResult.description = "You swing " + direction + ", but the notion of that direction escapes this realm.";
					finalResult.success = false;
					break;
				}

				const int targetX = m_Player.x + dx;
				const int targetY = m_Player.y + dy;

				bool hit = false;

				for (auto entityIteration = m_Entities.begin(); entityIteration != m_Entities.end();)
				{
					Entity& entity = *entityIteration;
					if (entity.faction == Faction::Hostile &&
						entity.x == targetX && entity.y == targetY)
					{
						hit = true;
						const int kPlayerDamage = 1;
						entity.hp -= kPlayerDamage;

						if (entity.hp <= 0)
						{
							entityIteration = m_Entities.erase(entityIteration);
							finalResult.description = "You strike " + direction + " and your foe crumbles into dust.";
						}
						else
						{
							++entityIteration;
							finalResult.description = "You deal a blow and hear a grunt " + direction + ".";
						}
						break;
					}
					else
					{
						++entityIteration;
					}
				}

				if (!hit)
				{
					finalResult.description = "You strike " + direction + " but hit nothing.";
					finalResult.success = false;
				}
				else
				{
					finalResult.success = true;
				}

				break;
			}
			case CommandAction::SummonCommand:
			{
				auto itTarget = command.args.find("target");
				auto itOrder = command.args.find("order");
				if (itTarget == command.args.end() || itOrder == command.args.end() ||
					!std::holds_alternative<std::string>(itTarget->second) ||
					!std::holds_alternative<std::string>(itOrder->second))
				{
					finalResult.description = "Your will is fractured; no clear order is given.";
					finalResult.success = false;
					break;
				}

				const std::string& target = std::get<std::string>(itTarget->second);
				const std::string& order = std::get<std::string>(itOrder->second);

				if (target != "all")
				{
					finalResult.description = "Only the collective heed your call for now.";
					finalResult.success = false;
					break;
				}

				EntityState newState;
				if (order == "follow")
				{
					newState = EntityState::FollowPlayer;
				}
				else if (order == "guard")
				{
					newState = EntityState::Guard;
				}
				else if (order == "attack")
				{
					newState = EntityState::Attack;
				}
				else
				{
					finalResult.description = "Your command lacks focus; your minions do not understand.";
					finalResult.success = false;
					break;
				}

				for (Entity& e : m_Entities)
				{
					if (e.faction != Faction::Friendly)
						continue;

					e.aiState = newState;
					if (newState == EntityState::Guard)
					{
						e.guardX = e.x;
						e.guardY = e.y;
					}
				}

				finalResult.description = "Your will ripples through the ranks of your summoned allies.";
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