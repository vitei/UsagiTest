#include "Engine/Common/Common.h"
#include "Engine/Framework/System.h"
#include "Engine/Framework/GameComponents.h"
#include "Engine/Framework/FrameworkComponents.pb.h"
#include "UsagiTest/Camera/CameraComponents.pb.h"
#include "Engine/Scene/Camera/StandardCamera.h"
#include "Engine/Scene/Camera/HMDCamera.h"
#include "UsagiTest/Camera/CameraEvents.pb.h"

namespace usg
{
	namespace Systems
	{
		class UpdateSceneCamera : public usg::System
		{
		public:
			struct Inputs
			{
				usg::Required<usg::MatrixComponent> mtx;
			};

			struct Outputs
			{
				usg::Required<usg::CameraComponent> cam;
				usg::Required<usg::TransformComponent> trans;
			};

			DECLARE_SYSTEM(usg::SYSTEM_DEFAULT_PRIORITY)

			static void LateUpdate(const Inputs& inputs, Outputs& outputs, float fDelta)
			{
				const auto& vPos = inputs.mtx->matrix.vPos().v3();
				const auto& vUp = inputs.mtx->matrix.vUp().v3();
				const auto& vForward = inputs.mtx->matrix.vFace().v3();
				auto& cameraRtd = outputs.cam.GetRuntimeData();
				ASSERT(cameraRtd.pCamera != nullptr);
				Matrix4x4 mtx;
				mtx.LookAt(vPos, vPos + vForward, vUp);
				cameraRtd.pCamera->SetUp(mtx, outputs.cam->fAspectRatio, outputs.cam->fFOV, outputs.cam->fNearPlaneDist, outputs.cam->fFarPlaneDist);
			}

			static void OnEvent(const Inputs& inputs, Outputs& outputs, const SetAspectRatio& evt)
			{
				outputs.cam.Modify().fAspectRatio = evt.fAspectRatio;
			}

			static void OnEvent(const Inputs& inputs, Outputs& outputs, const SetFieldOfView& evt)
			{
				outputs.cam.Modify().fFOV = evt.fFOV;
			}

		};


		class UpdateHMDCameraInternals : public usg::System
		{
		public:
			struct Inputs
			{
				usg::Required<usg::MatrixComponent> mtx;
			};

			struct Outputs
			{
				usg::Required<usg::HMDCameraComponent> cam;
				usg::Required<usg::TransformComponent> trans;
			};

			DECLARE_SYSTEM(usg::SYSTEM_DEFAULT_PRIORITY)

			static void LateUpdate(const Inputs& inputs, Outputs& outputs, float fDelta)
			{
				outputs.cam.GetRuntimeData().pCamera->SetModelMatrix(inputs.mtx->matrix);
				outputs.cam.GetRuntimeData().pCamera->Update();
			}

		};
	}
}

#include GENERATED_SYSTEM_CODE(UsagiTest/Camera/CameraSystems.cpp)
