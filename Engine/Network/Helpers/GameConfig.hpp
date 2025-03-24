#pragma once

#include "GameAdapter.hpp"

namespace Engine
{
	static const uint8_t DEFAULT_PRIVATE_KEY[yojimbo::KeyBytes] = { 0 };

	class GameNetworkType
	{
	public:
		virtual ~GameNetworkType() = default;

		virtual void CleanUp() = 0;
	};
}