#include "Engine/Common/Common.h"
#include "ShipController.h"
#include "ShipControlComponents.h"

namespace usg
{

	template<>
	void OnDeactivate<PlayerShipController>(Component<PlayerShipController>& p, ComponentLoadHandles& handles)
	{
		vdelete p.GetRuntimeData().pController;
	}

	template<>
	void OnActivate<PlayerShipController>(Component<PlayerShipController>& p)
	{
		p.GetRuntimeData().pController = vnew(usg::ALLOC_OBJECT) ShipController();
		p.GetRuntimeData().pController->Init();
	}
}