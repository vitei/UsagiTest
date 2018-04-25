#pragma once
#include "Engine/Common/Common.h"

class ShipController;

namespace usg
{

	template<>
	struct RuntimeData<PlayerShipController>
	{
		ShipController* pController;
	};

}