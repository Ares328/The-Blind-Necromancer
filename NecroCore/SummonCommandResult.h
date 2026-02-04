#pragma once
#include <string>
#include "SummonResult.h"

namespace NecroCore
{
	struct SummonCommandResult
	{
		bool success = false;

		std::string creature;

		std::string description;

		SummonResult summonResult;
	};
}