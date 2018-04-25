#include "Engine/Common/Common.h"
#include "Engine/Framework/System.h"
#include "Engine/Framework/GameComponents.h"
#include "Engine/Framework/FrameworkComponents.pb.h"
#include "Engine/Physics/PhysicsComponents.pb.h"
#include "UsagiTest/Input/ShipControlComponents.pb.h"
#include "Engine/Physics/PhysicsEvents.pb.h"
#include "Engine/Framework/EventManager.h"
#include "Engine/Physics/Raycast.h"
#include "UsagiTest/CollisionMask.pb.h"
#include "Engine/Framework/TransformTool.h"
#include "Engine/Physics/CollisionMask.pb.h"


namespace Systems
{
	class ApplyInputsToShip : public usg::System
	{
	public:
		struct Inputs
		{
			usg::Required<usg::EntityID> self;
			usg::Required<usg::RigidBody> rigidBody;
			usg::Required<usg::MatrixComponent> mtx;
			usg::Required<ShipControlInput> input;
			usg::Required<usg::EventManagerHandle, FromParents> eventManager;
		};

		struct Outputs
		{
			usg::Required<usg::RigidBody> rigidBody;
		};

		struct RaycastHitInputs
		{

		}; 

		DECLARE_SYSTEM(usg::SYSTEM_DEFAULT_PRIORITY) RAYCASTER

		static void Run(const Inputs& inputs, Outputs& outputs, float fDelta)
		{
			UpdateControls(inputs, outputs, fDelta);
		}

		static void UpdateControls(const Inputs& inputs, Outputs& outputs, float fDelta)
		{
			const auto vForward = inputs.mtx->matrix.vFace().v3();
			const auto vUp = inputs.mtx->matrix.vUp().v3();

			const float32 fPhysicsTimeStep = usg::physics::GetTimeStep();
			float32 fThrottle = inputs.input->inputAxes[SHIP_AXIS_THROTTLE];
			if (fThrottle)
			{
				usg::ApplyForce forceEvt;
				usg::ApplyForce_init(&forceEvt);

				const float32 fMul = 100.5f * fPhysicsTimeStep * 60.0f;
				forceEvt.vForce = vForward * fThrottle * fMul;
				forceEvt.eForceMode = usg::ForceMode_ACCELERATION;
				inputs.eventManager->handle->RegisterEventWithEntity(inputs.self->id, forceEvt, ON_ENTITY);
			}

			const float32 fTurn = inputs.input->inputAxes[SHIP_AXIS_YAW];
			const float32 fTurnMul = 0.070f * fPhysicsTimeStep * 60.0f;
			usg::ApplyTorque torqueEvt;
			usg::ApplyTorque_init(&torqueEvt);
			torqueEvt.eForceMode = usg::ForceMode_VELOCITY_CHANGE;
			torqueEvt.vTorque = inputs.mtx->matrix.TransformVec3(usg::Vector3f(0, fTurn*fTurnMul, 0), 0.0f);
			inputs.eventManager->handle->RegisterEventWithEntity(inputs.self->id, torqueEvt, ON_ENTITY);

			const float32 fPitch = inputs.input->inputAxes[SHIP_AXIS_PITCH];
			torqueEvt.eForceMode = usg::ForceMode_VELOCITY_CHANGE;
			torqueEvt.vTorque = inputs.mtx->matrix.TransformVec3(usg::Vector3f(fPitch*fTurnMul, 0, 0), 0.0f);
			inputs.eventManager->handle->RegisterEventWithEntity(inputs.self->id, torqueEvt, ON_ENTITY);

			const float32 fRoll = inputs.input->inputAxes[SHIP_AXIS_ROLL];
			torqueEvt.eForceMode = usg::ForceMode_VELOCITY_CHANGE;
			torqueEvt.vTorque = inputs.mtx->matrix.TransformVec3( usg::Vector3f(0, 0, -fRoll*fTurnMul), 0.0f);
			inputs.eventManager->handle->RegisterEventWithEntity(inputs.self->id, torqueEvt, ON_ENTITY);
		}
	};
}

#include GENERATED_SYSTEM_CODE(UsagiTest/Vehicle/VehicleSystems.cpp)
