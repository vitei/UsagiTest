#include "Engine/Common/Common.h"
#include "Engine/Core/Utility.h"
#include "Engine/Graphics/Device/GFXDevice.h"
#include "Engine/Game/GameUtil.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Core/Modules/ModuleManager.h"
#include "Engine/Layout/StringTable.h"
#include "Engine/Graphics/Device/GFXContext.h"
#include "Mode/ModeSpaceFlight.h"
#include "UsagiTest.h"


static UsagiTest* g_spGame = nullptr;

namespace usg
{
	usg::GameInterface* CreateGame()
	{
		ASSERT(!g_spGame);
		g_spGame = vnew(usg::ALLOC_OBJECT) UsagiTest();
		return g_spGame;
	}

}

	bool UsagiTestLoadFunc(uint32 uMode, usg::Mode** ppLoadMode)
	{
		switch (uMode)
		{
		case MODE_TITLE:
			ASSERT(false);	// No such mode yet
			break;
		case MODE_SPACEFLIGHT:
			*ppLoadMode = vnew(usg::ALLOC_MODE) ModeSpaceFlight;
			break;
		default:
			FATAL_RELEASE(false, "[InitThread::Run] Invalid mode!\n");
			return false;
		}

		return true;
	}

UsagiTest::UsagiTest() :
	Inherited()
{
}

UsagiTest::~UsagiTest()
{

}


usg::ModeLoadFunc UsagiTest::GetLoadFunc() const
{
	return UsagiTestLoadFunc;
}


void UsagiTest::Init(usg::GFXDevice* pDevice)
{
	Inherited::Init(pDevice);
//	usg::ModuleManager::Inst()->LoadModule("Oculus.dll");
	HookupModules(pDevice);

	m_eMode = MODE_SPACEFLIGHT;
	m_ePrevMode = MODE_NONE;
	
	usg::Audio::Inst()->LoadSoundArchive("VPB/Audio/UsagiTest.vpb");
	usg::StringTable::Create()->Init("UsagiTestStrings", usg::Region_EUROPE, usg::Language_ENGLISH);

	StartNextMode(pDevice);

	m_bIsRunning = true;
}


uint32 UsagiTest::GetNextMode() const
{
	return (uint32)m_eMode;
}

void UsagiTest::ModeFinished()
{
	SetNextMode(MODE_SPACEFLIGHT);
}

void UsagiTest::SetNextMode(ModeType eMode)
{
	m_eMode = eMode;
}



