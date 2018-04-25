#include "Engine/Common/Common.h"
#include "Engine/Framework/System.h"
#include "Engine/Framework/GameComponents.h"
#include "Engine/Framework/FrameworkComponents.pb.h"
#include "UsagiTest/Input/ShipControlComponents.pb.h"
#include "UsagiTest/Input/ShipController.h"

namespace Systems
{
	class GetPlayerShipInput : public usg::System
	{
	public:
		struct Inputs
		{
			usg::Required<PlayerShipController> controller;
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
		}
	};
}

#include GENERATED_SYSTEM_CODE(UsagiTest/Input/InputSystems.cpp)
