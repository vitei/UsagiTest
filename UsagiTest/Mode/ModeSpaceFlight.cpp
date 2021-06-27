#include "Engine/Common/Common.h"
#include "Engine/Graphics/Device/GFXDevice.h"
#include "Engine/Graphics/Device/Display.h"
#include "Engine/Resource/ResourceMgr.h"
#include "Engine/Game/GameView.h"
#include "Engine/Scene/Scene.h"
#include "Engine/HID/Input.h"
#include "Engine/Graphics/Device/IHeadMountedDisplay.h"
#include "Engine/Layout/Global2D.h"
#include "Engine/Layout/StringTable.h"
#include "Engine/Scene/ViewContext.h"
#include "Engine/Graphics/Lights/LightMgr.h"
#include "Engine/Framework/EntitySpawnParams.h"
#include "Engine/Graphics/Device/GFXContext.h"
#include "Engine/Core/stl/utility.h"
#include "Engine/Layout/StringTable.h"
#include "Engine/Framework/ComponentEntity.h"
#include "Engine/Framework/FrameworkComponents.pb.h"
#include "UsagiTest/Camera/CameraComponents.pb.h"
#include "UsagiTest/Camera/CameraEvents.pb.h"
#include "Engine/Framework/EntityLoader.h"
#include "Engine/Audio/MusicManager.h"
#include "Engine/Framework/EntityLoaderHandle.h"
#include "Engine/Framework/EventManager.h"
#include "UsagiTest/audio_gen/UsagiTest.pb.h"
#include "ModeSpaceFlight.h"

void RegisterUsagiTestSystems(usg::SystemCoordinator& systemCoordinator);

ModeSpaceFlight::ModeSpaceFlight() :
	Inherited(RegisterUsagiTestSystems)
{

}

ModeSpaceFlight::~ModeSpaceFlight()
{
	vdelete m_pGameView;
	m_pGameView = nullptr;
}

void ModeSpaceFlight::InitRoot()
{
	Required<usg::EntityLoaderHandle, FromSelfOrParents> entityLoader;
	GetComponent(GetRootEntity(), entityLoader);
	ASSERT(entityLoader.IsValid());
	
	entityLoader->pHandle->ApplyTemplateToEntity("Entities/Root.vent", GetRootEntity());
}

void ModeSpaceFlight::InitText(usg::GFXDevice* pDevice, usg::ResourceMgr* pMgr)
{
	// Temporary 2D test code
	m_2DConstants.Init(pDevice, usg::g_global2DCBDecl);
	m_2DDescriptor.Init(pDevice, pDevice->GetDescriptorSetLayout(usg::g_sGlobalDescriptors2D));
	m_2DDescriptor.SetConstantSet(0, &m_2DConstants);
	
	m_text.Init(pDevice, pMgr, pDevice->GetDisplay(0)->GetRenderPass());
	m_text.SetPosition(10.0f, 10.0f);
	m_text.SetScale(usg::Vector2f(26.f, 26.f));
	const usg::StringTable::KeyString& string = usg::StringTable::Inst()->Find("Test");
	m_text.SetText(string.pStr->text);
	m_text.SetFont(usg::ResourceMgr::Inst()->GetFont(pDevice, "FontDef"));
	m_text.SetGradationEndColor(usg::Color(0.8f, 0.8f, 0.8f, 1.0f));
	m_text.SetGradationStartColor(usg::Color(0.3f, 0.3f, 0.3f, 1.0f));
	m_text.SetBackgroundColor(usg::Color(1.0f, 1.0f, 1.0f, 0.0f));
	//m_text.SetColor(usg::Color::Green);
	m_text.UpdateBuffers(pDevice);


	usg::Global2DConstants* pConstants = m_2DConstants.Lock<usg::Global2DConstants>();
	pConstants->mProjMat.Orthographic(0.0f, 1280.0f, 0.0f, 720.0f, 0.0f, 10.0f);
	m_2DConstants.Unlock();

	m_2DConstants.UpdateData(pDevice);
	m_2DDescriptor.UpdateDescriptors(pDevice);
}

void ModeSpaceFlight::Init(usg::GFXDevice* pDevice, usg::ResourceMgr* pMgr)
{
	Inherited::Init(pDevice, pMgr);

	usg::Scene& scene = GetScene();
	usg::AABB worldBounds;
	worldBounds.SetMinMax(-500 * usg::Vector3f::ONE, 500 * usg::Vector3f::ONE);
	scene.Init(pDevice, pMgr, worldBounds);

	InitRoot();
	InitGameView(pDevice, pMgr);
	InitScene();
	InitPlayer();
	InitText(pDevice, pMgr);

	usg::MusicManager::Create()->PlayMusic(MISSION_01, 1.0f, usg::MusicManager::FADE_TYPE_WAIT, usg::MusicManager::FADE_TYPE_FADE, 0.3f, 0);

	GetScene().SetActiveCamera(utl::CRC32("ExternalCam"), 0);
}

void ModeSpaceFlight::Cleanup(usg::GFXDevice* pDevice)
{
	usg::Scene& scene = GetScene();
	m_pGameView->Cleanup(pDevice, scene);
	m_text.Cleanup(pDevice);
	m_2DConstants.Cleanup(pDevice);
	m_2DDescriptor.Cleanup(pDevice);
	m_postFXTV.Cleanup(pDevice);
	Inherited::Cleanup(pDevice);
}

void ModeSpaceFlight::InitScene()
{
	Required<usg::EntityLoaderHandle, FromSelfOrParents> entityLoader;
	GetComponent(GetRootEntity(), entityLoader);
	ASSERT(entityLoader.IsValid());

	usg::EntitySpawnParams spawnParams;
	usg::TransformComponent initialTrans;
	usg::TransformComponent_init(&initialTrans);
	spawnParams.SetTransform(initialTrans);
	entityLoader->pHandle->SpawnEntityFromTemplate("Entities/Sector.vent", GetRootEntity(), spawnParams);

	// Disabling this model as it has issues
	//initialTrans.position.Assign(0.0f, -150.0f, 300.0f);
	//spawnParams.SetTransform(initialTrans);
	//entityLoader->pHandle->SpawnEntityFromTemplate("Entities/Cube.vent", GetRootEntity(), spawnParams);
}

void ModeSpaceFlight::InitPlayer()
{
	Required<usg::EntityLoaderHandle, FromSelfOrParents> entityLoader;
	GetComponent(GetRootEntity(), entityLoader);
	ASSERT(entityLoader.IsValid());

	Required<usg::EventManagerHandle, FromSelfOrParents> eventManager;
	GetComponent(GetRootEntity(), eventManager);
	ASSERT(eventManager.IsValid());

	usg::EntitySpawnParams spawnParams;
	usg::TransformComponent initialTrans;
	usg::TransformComponent_init(&initialTrans);

	spawnParams.SetTransform(initialTrans);
	spawnParams.SetOwnerNUID(usg::string_crc("Player0").Get());
	usg::Entity player = entityLoader->pHandle->SpawnEntityFromTemplate("Entities/Player.vent", GetRootEntity(), spawnParams);

	usg::Entity cameraEntity = player->GetChildEntityByName(*dynamic_cast<usg::UnsafeComponentGetter*>(this), "cam");
	ASSERT(cameraEntity != nullptr);

	usg::SetAspectRatio setAspectRatio;
	usg::SetAspectRatio_init(&setAspectRatio);
	setAspectRatio.fAspectRatio = m_pGameView->GetBounds().width / (float32)(m_pGameView->GetBounds().height);
	eventManager->handle->RegisterEventWithEntity(cameraEntity, setAspectRatio, ON_ENTITY);

#ifdef DEBUG_BUILD
	entityLoader->pHandle->SpawnEntityFromTemplate("Entities/DebugCamera.vent", GetRootEntity(), spawnParams);
#endif
}

void ModeSpaceFlight::InitGameView(usg::GFXDevice* pDevice, usg::ResourceMgr* pMgr)
{
	usg::Scene& scene = GetScene();

	uint32 uWidthTV, uHeightTV;
	pDevice->GetDisplay(0)->GetDisplayDimensions(uWidthTV, uHeightTV, false);

	usg::IHeadMountedDisplay* pHMD = pDevice->GetHMD();
	if(pHMD)
	{
		pHMD->GetRenderTargetDim(usg::IHeadMountedDisplay::Eye::Left, 1.0, uWidthTV, uHeightTV);
	}

	constexpr uint32 uPostFXTV = usg::PostFXSys::EFFECT_DEFERRED_SHADING | usg::PostFXSys::EFFECT_SKY_FOG | usg::PostFXSys::EFFECT_SMAA | usg::PostFXSys::EFFECT_SSAO;// usg::PostFXSys::EFFECT_DEFERRED_SHADING |usg::PostFXSys::EFFECT_SKY_FOG | usg::PostFXSys::EFFECT_FXAA | usg::PostFXSys::EFFECT_DEFERRED_SHADING | usg::PostFXSys::EFFECT_BLOOM;// usg::PostFXSys::EFFECT_SMAA |  | usg::PostFXSys::EFFECT_SKY_FOG | usg::PostFXSys::EFFECT_DEFERRED_SHADING;
	m_postFXTV.Init(pDevice, pMgr, uWidthTV, uHeightTV, uPostFXTV);
	m_postFXTV.SetSkyTexture(pDevice, usg::ResourceMgr::Inst()->GetTexture(pDevice, "purplenebula"));
	const usg::GFXBounds bounds = m_postFXTV.GetBounds();

	usg::GFXBounds viewBounds;
	viewBounds.x = 0; viewBounds.y = 0;
	viewBounds.width = uWidthTV;
	viewBounds.height = uHeightTV;
		
	usg::GameView* pView = vnew(usg::ALLOC_OBJECT)usg::GameView(pDevice, scene, pMgr, m_postFXTV, viewBounds);
	m_pGameView = pView;
}

bool ModeSpaceFlight::Update(float fElapsed)
{
	bool bDebug = GetScene().GetViewContext(0)->GetCamera()->GetID() == utl::CRC32("DebugCam");


	m_postFXTV.Update(&GetScene(), fElapsed);
	{
		Inherited::Update(bDebug ? 0.0f : fElapsed);
	}
	return false;
}

void ModeSpaceFlight::PreDraw(usg::GFXDevice* pDevice, usg::GFXContext* pImmContext)
{
	if (m_debugCam.GetActive())
	{
		usg::Scene& scene = GetScene();
		scene.Update(pDevice);
	}
	m_postFXTV.UpdateGPU(pDevice);
}

void ModeSpaceFlight::Draw(usg::Display* pDisplay, usg::IHeadMountedDisplay* pHMD, usg::GFXContext* pImmContext)
{
	usg::Scene& scene = GetScene();
	scene.PreDraw(pImmContext);
	scene.GetLightMgr().ViewShadowRender(pImmContext, &scene, m_pGameView->GetViewContext());
	
	uint32 uDrawCount = pHMD ? 2 : 1;
	for (uint32 i = 0; i < uDrawCount; i++)
	{
		m_postFXTV.BeginScene(pImmContext, 0);
		//pImmContext->ClearRenderTarget(usg::RenderTarget::RT_FLAG_DS | usg::RenderTarget::RT_FLAG_COLOR);	// No longer necessary
		m_pGameView->Draw(&m_postFXTV, pDisplay, pImmContext, nullptr, pHMD ? (i == 0 ? usg::VIEW_LEFT_EYE : usg::VIEW_RIGHT_EYE) : usg::VIEW_CENTRAL);
		m_postFXTV.EndScene();

		if (pHMD)
		{
			pImmContext->TransferToHMD(m_postFXTV.GetFinalRT(), pHMD, i == 0);
		}
		else
		{
			pImmContext->TransferRect(m_postFXTV.GetFinalRT(), pDisplay, m_postFXTV.GetBounds(), m_pGameView->GetBounds());
		}
	}

	// Still working on 2D
	if(!pHMD)
	{
#if 1
		pImmContext->RenderToDisplay(pDisplay);
		pImmContext->SetDescriptorSet(&m_2DDescriptor, 0);
		m_text.Draw(pImmContext);
#endif
	}

	if(pHMD)
	{
		pHMD->SubmitFrame();
		pImmContext->TransferSpectatorDisplay(pHMD, pDisplay);
		pImmContext->RenderToDisplay(pDisplay);
	}	
}

void ModeSpaceFlight::PostDraw(usg::GFXDevice* pDevice)
{
	
}

void ModeSpaceFlight::NotifyResize(usg::GFXDevice* pDevice, uint32 uDisplay, uint32 uWidth, uint32 uHeight)
{
	m_postFXTV.Resize(pDevice, uWidth, uHeight);

	Required<usg::EventManagerHandle, FromSelfOrParents> eventManager;
	GetComponent(GetRootEntity(), eventManager);
	ASSERT(eventManager.IsValid());

	usg::SetAspectRatio setAspectRatio;
	usg::SetAspectRatio_init(&setAspectRatio);
	setAspectRatio.fAspectRatio = (float)uWidth/(float)uHeight;
	eventManager->handle->RegisterEventWithEntity(GetRootEntity(), setAspectRatio, ON_ENTITY|ON_CHILDREN);

		
	usg::GFXBounds bounds{ 0, 0, (sint32)uWidth, (sint32)uHeight };
	m_pGameView->SetBounds(bounds);

}

