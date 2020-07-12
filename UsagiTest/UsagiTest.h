#pragma once
#ifndef USAGITEST_USAGITEST_H
#define USAGITEST_USAGITEST_H
#include "Engine/Common/Common.h"
#include "Engine/Core/Timer/Timer.h"
#include "Engine/Debug/DebugStats.h"
#include "Engine/Core/Timer/ProfilingTimer.h"
#include "Engine/Game/ModeTransition.h"
#include "Engine/Game/SimpleGameBase.h"
#include "Engine/Game/InitThread.h"

enum ModeType
{
	MODE_TITLE = 0,
	MODE_SPACEFLIGHT,
	MODE_NONE
};

class InitThread;

class UsagiTest : public usg::SimpleGameBase
{
	typedef usg::SimpleGameBase Inherited;
public:
	explicit UsagiTest();
	virtual ~UsagiTest();

	virtual void Init(usg::GFXDevice* pDevice, usg::ResourceMgr* pResMgr) override;

private:
	void SetNextMode(ModeType eMode);
	virtual uint32 GetNextMode() const override;
	virtual void ModeFinished() override;
	virtual usg::ModeLoadFunc GetLoadFunc() const override;

	ModeType			m_eMode;
	ModeType			m_ePrevMode;
	
};

#endif // USAGITEST_USAGITEST_H