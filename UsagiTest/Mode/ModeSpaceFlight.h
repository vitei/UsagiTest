/****************************************************************************
//	Filename: ModeSpaceFlight.h
//	Description: The meat and potatoes of the game
*****************************************************************************/
#ifndef __CLR_USAGITEST_MODE_SPACEFLIGHT_H__
#define __CLR_USAGITEST_MODE_SPACEFLIGHT_H__
#include "Engine/Common/Common.h"
#include "Engine/PostFX/PostFXSys.h"
#include "Engine/Debug/DebugCamera.h"
#include "Engine/Layout/Fonts/Text.h"
#include "Engine/Game/SystemsMode.h"

namespace usg
{
	class Debug3D;
	class GameView;
}

class ModeSpaceFlight : public usg::SystemsMode
{
	typedef usg::SystemsMode Inherited;
public:
	ModeSpaceFlight();
	virtual ~ModeSpaceFlight();

	void Init(usg::GFXDevice* pDevice) override;
	void CleanUp(usg::GFXDevice* pDevice) override;
	bool Update(float fElapsed) override;
	void PreDraw(usg::GFXDevice* pDevice, usg::GFXContext* pImmContext) override;
	void Draw(usg::Display* pDisplay, usg::IHeadMountedDisplay* pHMD, usg::GFXContext* pImmContext) override;
	void PostDraw(usg::GFXDevice* pDevice) override;

	void NotifyResize(usg::GFXDevice* pDevice, uint32 uDisplay, uint32 uWidth, uint32 uHeight) override;
	bool FinalTargetIsDisplay() const override { return true; };


private:

	void InitRoot();
	void InitScene();
	void InitGameView(usg::GFXDevice* pDevice);
	void InitPlayer();
	void InitText(usg::GFXDevice* pDevice);

	usg::DescriptorSet m_2DDescriptor;
	usg::ConstantSet	m_2DConstants;
	usg::Text			m_text;
	usg::DebugCamera	m_debugCam;
	usg::PostFXSys		m_postFXTV;
	usg::GameView*		m_pGameView;
};

#endif 
