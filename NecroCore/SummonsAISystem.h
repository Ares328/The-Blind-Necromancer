#pragma once

#include "Entity.h"
#include <ostream>
#include <functional>

namespace NecroCore
{
	class Game;
	struct CommandResult;

	class SummonsAISystem
	{
	public:
		void ProcessSummonedTurn(Game& game, CommandResult& result);

	private:
		bool HandleSummonedAttackAI(Game& game,
			Entity& entity,
			std::ostringstream& oss,
			const std::function<void()>& appendSeparator);
	};
}