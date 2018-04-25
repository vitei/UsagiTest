#pragma once

#include "UsagiTest/Camera/CameraComponents.pb.h"
#include "Engine/Framework/GameComponents.h"

namespace usg
{
	class StandardCamera;
	class HMDCamera;

	template<>
	struct RuntimeData<CameraComponent>
	{
		StandardCamera* pCamera;
	};

	template<>
	struct RuntimeData<HMDCameraComponent>
	{
		HMDCamera* pCamera;
	};
}