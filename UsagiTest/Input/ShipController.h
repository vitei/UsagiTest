/****************************************************************************
//	Filename: ShipController.h
//	Description: Manages input for the ships
*****************************************************************************/
#ifndef _USAGITEST_SHIP_CONTROLLER_H_
#define _USAGITEST_SHIP_CONTROLLER_H_

#include "Engine/Common/Common.h"
#include "Engine/HID/Controller.h"
#include "UsagiTest/Input/ShipControlComponents.pb.h"

using namespace usg;

class ShipController : public usg::Controller
{
	typedef Controller Inherited;

public:
	ShipController( uint32 uPadId =0 );
	virtual ~ShipController();

	void	Init();

	virtual void Update(float timeDelta );
	void GetInput(ShipControlInput& input) const;


private:
	void ClearInputMappings();

	MappingOutput		m_buttons[SHIP_BOOL_COUNT];
	MappingOutput		m_axes[SHIP_AXIS_COUNT];


};

#endif
