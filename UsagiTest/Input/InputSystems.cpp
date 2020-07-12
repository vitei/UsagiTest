#include "Engine/Common/Common.h"
#include "Engine/Framework/System.h"
#include "Engine/Framework/EventManager.h"
#include "Engine/Framework/GameComponents.h"
#include "Engine/Framework/FrameworkComponents.pb.h"
#include "UsagiTest/Input/ShipControlComponents.pb.h"
#include "Engine/Debug/DebugEvents.pb.h"
#include "UsagiTest/Input/ShipController.h"

namespace Systems
{
	class GetPlayerShipInput : public usg::System
	{
	public:
		struct Inputs
		{
			usg::Required<EventManagerHandle, FromSelfOrParents> eventManager;
			usg::Required<PlayerShipController> controller;
			usg::Required<usg::SimulationActive, FromSelfOrParents> simActive;
		};

		struct Outputs
		{
			usg::Required<PlayerShipController> controller;
			usg::Required<ShipControlInput> shipInput;
		};


		DECLARE_SYSTEM(usg::SYSTEM_PRE_EARLY)	// Get the input as early as we can

		static void Run(const Inputs& inputs, Outputs& outputs, float fDelta)
		{
			outputs.controller.GetRuntimeData().pController->Update(fDelta);
			outputs.controller.GetRuntimeData().pController->GetInput(outputs.shipInput.Modify());

			if (outputs.controller.GetRuntimeData().pController->ToggleDebugCam())
			{
				usg::RequestDebugCameraState evt;
				evt.bEnable = inputs.simActive->bActive;
				inputs.eventManager->handle->RegisterEvent(evt);
			}
		}
	};
}

#include GENERATED_SYSTEM_CODE(UsagiTest/Input/InputSystems.cpp)
