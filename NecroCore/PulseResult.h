#pragma once

namespace NecroCore
{
	struct PulseResult
	{
		int detectedHostileCount = 0;
		int detectedFriendlyCount = 0;
		int detectedTrapCount = 0;
		std::string description;
	};
}