#pragma once

#include "Entity.h"

#include <ostream>
#include <functional>

namespace NecroCore
{
	class Game;
	struct CommandResult;

	class HostileAISystem
	{
	public:
		void ProcessHostileTurn(Game& game, CommandResult& result);

	private:
		bool HandleHostileAttackAI(Game& game,
			Entity& entity,
			std::ostringstream& oss,
			bool& anyHostileActed,
			bool& playerDiedThisTurn,
			const std::function<void()>& appendSeparator);
	};
}