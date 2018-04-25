#include "Engine/Common/Common.h"
#include "Engine/Framework/System.h"
#include "Engine/Framework/GameComponents.h"
#include "Engine/Framework/FrameworkComponents.pb.h"
#include "Engine/Framework/EntityLoaderHandle.h"
#include "Engine/Framework/EntityLoader.h"
#include "Engine/Framework/ComponentManager.h"
#include "Engine/Physics/PhysicsEvents.pb.h"
#include "Engine/Physics/CollisionMask.pb.h"
#include "Engine/Physics/CollisionData.pb.h"
#include "Engine/Framework/EventManager.h"
#include "Engine/Scene/Common/SceneComponents.pb.h"
#include "Engine/Framework/FrameworkEvents.pb.h"
#include "Engine/Physics/PhysicsComponents.pb.h"
#include "Engine/Framework/EntitySpawnParams.h"
#include "UsagiTest/Weapon/WeaponComponents.pb.h"
#include "UsagiTest/Input/ShipControlComponents.pb.h"

namespace Systems
{
	class FireGunSystem : public usg::System
	{
	public:
		struct Inputs
		{
			usg::Required<ShipControlInput, FromSelfOrParents> controller;
			usg::Required<usg::NetworkOwner, FromSelfOrParents> nuid;
			usg::Required<usg::EntityLoaderHandle, FromSelfOrParents> entityLoader;
			usg::Required<usg::TransformComponent, FromSelfOrParents>	transform;
			usg::Required < usg::EntityID, FromParentWith<usg::SceneComponent> > sceneEntity;
			usg::Required<GunComponent>	gun;
		};

		struct Outputs
		{
			usg::Required<GunComponent> gun;
		};


		DECLARE_SYSTEM(usg::SYSTEM_POST_GAMEPLAY)	// Get the input as early as we can

		static void Run(const Inputs& inputs, Outputs& outputs, float fDelta)
		{
			if (inputs.gun->fireCountdown > 0.0)
			{
				outputs.gun.Modify().fireCountdown = usg::Math::Max(outputs.gun.Modify().fireCountdown - (fDelta * inputs.gun->fireRate), 0.0f);
			}

			if (inputs.gun->fireCountdown == 0.0f && inputs.controller->inputBools[SHIP_BOOL_FIRE])
			{
				// Spawn the laser component
				usg::EntitySpawnParams spawnParams;
				spawnParams.SetTransform(*inputs.transform);
				spawnParams.SetOwnerNUID(inputs.nuid->ownerUID);
				inputs.entityLoader->pHandle->SpawnEntityFromTemplate("Entities/Laser.vent", inputs.sceneEntity->id, spawnParams);
				outputs.gun.Modify().fireCountdown = 1.0f;
			}
		}
	};


	class ApplyConstantForce : public usg::System
	{
	public:
		struct Inputs
		{
			usg::Required<usg::EntityID> self;
			usg::Required<ConstantForce>	force;
			usg::Required<usg::MatrixComponent>	mat;
			usg::Required<usg::EventManagerHandle, FromParents> eventManager;

		};

		struct Outputs
		{
			
		};


		DECLARE_SYSTEM(usg::SYSTEM_POST_TRANSFORM)	// After we have the correct matrix

		static void Run(const Inputs& inputs, Outputs& outputs, float fDelta)
		{
			usg::ApplyForce forceEvt;
			usg::ApplyForce_init(&forceEvt);
			usg::Vector3f vForce = inputs.force->vForce;
			if (!inputs.force->bWorldSpace)
			{
				vForce = inputs.mat->matrix.TransformVec3(vForce, 0.0f);
			}

			forceEvt.vForce = vForce;
			forceEvt.eForceMode = usg::ForceMode_ACCELERATION;
			inputs.eventManager->handle->RegisterEventWithEntity(inputs.self->id, forceEvt, ON_ENTITY);
		}

	};

	class DestroyProjectile : public usg::System
	{
	public:
		struct Inputs
		{
			usg::Required<usg::EntityID>	  self;
			Required<Projectile>		     projectile;
			Required<usg::NetworkOwner>       ownerUID;
			usg::Required < usg::EntityID, FromParentWith<usg::SceneComponent> > sceneEntity;
			usg::Required<usg::EntityLoaderHandle, FromSelfOrParents> entityLoader;
			usg::Required<usg::EventManagerHandle, FromParents> eventManager;
		};

		struct Outputs
		{
			Required<usg::StateComponent>     state;
		};

		struct ColliderInputs
		{
			Required<usg::EntityID> entity;
			Required<usg::CollisionMasks> collisionMasks;
			Optional<usg::NetworkOwner, FromSelfOrParents> ownerUID;
		};

		DECLARE_SYSTEM(usg::SYSTEM_DEFAULT_PRIORITY) COLLIDABLE(usg::COLLM_ANY)

		static void OnCollision(const Inputs& inputs, Outputs& outputs, const ColliderInputs& colliderInputs, const usg::Collision& collision)
		{
			if (colliderInputs.ownerUID.Exists() && colliderInputs.ownerUID.Force()->ownerUID == inputs.ownerUID->ownerUID)
			{
				return;
			}

			usg::KillEntityEvent killEntity;
			inputs.eventManager->handle->RegisterEventWithEntity(inputs.self->id, killEntity, ON_ENTITY);
			usg::EntitySpawnParams spawnParams;
			usg::TransformComponent transform;
			transform.position = collision.vIntersectionPoint - collision.vNormal;
			spawnParams.SetTransform(transform);
			// TODO: Orientation from normal
			inputs.entityLoader->pHandle->SpawnEntityFromTemplate("Entities/Explosion.vent", inputs.sceneEntity->id, spawnParams);
		}
	};

}

#include GENERATED_SYSTEM_CODE(UsagiTest/Weapon/WeaponSystems.cpp)
