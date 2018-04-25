#include "Engine/Common/Common.h"
#include "Engine/Framework/SystemCoordinator.h"
#include "ShipController.h"


using namespace usg;

ShipController::ShipController(uint32 uPadId)
	: Controller(uPadId)
{
	MemSet(m_buttons, 0, sizeof(usg::Controller::MappingOutput) * SHIP_BOOL_COUNT);
	MemSet(m_axes, 0, sizeof(usg::Controller::MappingOutput) * SHIP_AXIS_COUNT);

}

ShipController::~ShipController()
{
}

void ShipController::Init()
{
	ResetDetails();

	Inherited::CreateAxisMapping(GAMEPAD_AXIS_LEFT_X, AXIS_TYPE_ABSOLUTE, m_axes[SHIP_AXIS_YAW]);
	Inherited::CreateAxisMapping(GAMEPAD_AXIS_LEFT_Y, AXIS_TYPE_ABSOLUTE, m_axes[SHIP_AXIS_PITCH]);
	Inherited::CreateAxisMapping(GAMEPAD_AXIS_RIGHT_X, AXIS_TYPE_ABSOLUTE, m_axes[SHIP_AXIS_ROLL]);
	Inherited::CreateButtonMapping(GAMEPAD_BUTTON_R, m_axes[SHIP_AXIS_THROTTLE], usg::BUTTON_STATE_HELD);
	Inherited::CreateButtonMapping(GAMEPAD_BUTTON_ZR, m_buttons[SHIP_BOOL_FIRE]);
}

void ShipController::Update(float timeDelta)
{
	ClearInputMappings();
	Inherited::Update(timeDelta);

}


void ShipController::ClearInputMappings()
{
	for (int i = 0; i < SHIP_BOOL_COUNT; i++)
	{
		m_buttons[i].Clear();
	}
	for (int i = 0; i < SHIP_AXIS_COUNT; i++)
	{
		m_axes[i].Clear();
	}
}


void ShipController::GetInput(ShipControlInput& input) const
{
	for (uint32 i = 0; i < SHIP_AXIS_COUNT; i++)
	{
		input.inputAxes[i] = m_axes[i].GetFloat();
	}

	for (uint32 i = 0; i < SHIP_BOOL_COUNT; i++)
	{
		input.inputBools[i] = m_buttons[i].GetBool();
	}
}