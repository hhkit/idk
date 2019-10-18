#include "stdafx.h"
#include "MonoWrapperEnviroment.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/sgen-bridge.h>
#include <mono/utils/mono-dl-fallback.h>
#include <mono/metadata/mono-gc.h>
#include <mono/utils/mono-logger.h>

#include <IncludeComponents.h>
#include <IncludeResources.h>
#include <IncludeSystems.h>

#include <script/ValueBoxer.h>
#include <script/MonoBinder.h>

namespace idk::mono
{
	MonoWrapperEnvironment::MonoWrapperEnvironment(string_view full_path_to_game_dll)
	{

		_domain = mono_jit_init("MasterDomain");
		_assembly = mono_domain_assembly_open(_domain, full_path_to_game_dll.data());

		BindCoreFunctions();

		IDK_ASSERT_MSG(_assembly, "cannot load idk.dll");

		{
			auto img = mono_assembly_get_image(_assembly);
			auto klass = mono_class_from_name(img, "idk", "IDK");
			auto method = mono_class_get_method_from_name(klass, "Main", 1);
			void* args[] = { 0 };
			mono_runtime_invoke(method, nullptr, args, nullptr);
		}
		ScanTypes();

		auto mb_itr = _types.find("MonoBehavior");
		IDK_ASSERT_MSG(mb_itr != _types.end(), "cannot find idk.MonoBehavior");
		IDK_ASSERT_MSG(mb_itr->second.CacheThunk("UpdateCoroutines"), "could not cache method");
	}

	MonoWrapperEnvironment::~MonoWrapperEnvironment()
	{
		mono_jit_cleanup(_domain);
	}


	void MonoWrapperEnvironment::BindCoreFunctions()
	{
		constexpr auto Bind = mono_add_internal_call;
		// handleable
		Bind("idk.Bindings::ObjectValidate", decay(
			[](GenericHandle go) -> bool
		{
			return GameState::GetGameState().ValidateHandle(go);
		}
		));

		Bind("idk.Bindings::ObjectDestroy", decay(
			[](GenericHandle go)
		{
			GameState::GetGameState().DestroyObject(go);
		}
		));

		// game object

		Bind("idk.Bindings::GameObjectAddEngineComponent", decay(
			[](Handle<GameObject> go, MonoString* component) -> uint64_t
			{
				auto* s = mono_string_to_utf8(component);
				auto retval = go->AddComponent(string_view{ s }).id;
				mono_free(s);
				return retval;
			}
		));

//		Bind("idk.Bindings::GameObjectGetEngineComponent", &poop);
		Bind("idk.Bindings::GameObjectGetEngineComponent", decay(
			[](Handle<GameObject> go, MonoString* component) -> uint64_t // note: return value optimization
			{
				auto* s = mono_string_to_utf8(component);
				auto retval = go->GetComponent(string_view{ s }).id;
				mono_free(s);
				return retval;
			}
		));

		Bind("idk.Bindings::GameObjectGetActiveInHierarchy", decay(
			[](Handle<GameObject> go) -> bool
			{
				return go->ActiveInHierarchy();
			}
		));

		Bind("idk.Bindings::GameObjectActiveSelf", decay(
			[](Handle<GameObject> go) -> bool
			{
				return go->ActiveSelf();
			}
		));

		Bind("idk.Bindings::GameObjectSetActive", decay(
			[](Handle<GameObject> go, bool set) -> void
			{
				go->SetActive(set);
			}
		));

		// component
		Bind("idk.Bindings::ComponentGetGameObject", decay(
			[](GenericHandle go) -> uint64_t
		{
			return GameState::GetGameState().GetGameObject(go).id;
		}
		));


		// transform

		Bind("idk.Bindings::TransformGetPosition", decay(
			[](Handle<Transform> h) -> vec3
			{ 
				return h->GlobalPosition();
			}));

		Bind("idk.Bindings::TransformSetPosition", decay(
			[](Handle<Transform> h, vec3 v)
			{
				h->GlobalPosition(v);
			}));

		Bind("idk.Bindings::TransformGetScale", decay(
			[](Handle<Transform> h) -> vec3
			{
				return h->GlobalScale();
			}));

		Bind("idk.Bindings::TransformSetScale", decay(
			[](Handle<Transform> h, vec3 v)
			{
				h->GlobalScale(v);
			}));

		Bind("idk.Bindings::TransformGetRotation", decay(
			[](Handle<Transform> h) -> quat
			{
				return h->GlobalRotation();
			}));

		Bind("idk.Bindings::TransformSetRotation", decay(
			[](Handle<Transform> h, quat v)
			{
				h->GlobalRotation(v);
			}));

		Bind("idk.Bindings::TransformForward", decay(
			[](Handle<Transform> h) -> vec3
			{
				return h->Forward();
			}));

		Bind("idk.Bindings::TransformUp", decay(
			[](Handle<Transform> h) -> vec3
			{
				return h->Up();
			}));

		Bind("idk.Bindings::TransformRight", decay(
			[](Handle<Transform> h) -> vec3
			{
				return h->Right();
			}));

		// RigidBody
		Bind("idk.Bindings::RigidBodyGetMass", decay(
			[](Handle<RigidBody> rb) ->float
		{
			return rb->mass();
		}));

		Bind("idk.Bindings::RigidBodySetMass", decay(
			[](Handle<RigidBody> rb, float val)
		{
			rb->mass(val);
		}
		));

		Bind("idk.Bindings::RigidBodyGetVelocity", decay(
			[](Handle < RigidBody> rb) -> vec3
		{
			return rb->velocity();
		}));

		Bind("idk.Bindings::RigidBodySetVelocity", decay(
			[](Handle<RigidBody> rb, vec3 val)
		{
			rb->sleep_next_frame = true;
			rb->initial_velocity = val;
		}
		));

		Bind("idk.Bindings::RigidBodyGetPosition", decay(
			[](Handle < RigidBody> rb) -> vec3
		{
			return rb->position();
		}));

		Bind("idk.Bindings::RigidBodySetPosition", decay(
			[](Handle<RigidBody> rb, vec3 val)
		{
			rb->position(val);
		}
		));

		Bind("idk.Bindings::RigidBodyGetUseGravity", decay(
			[](Handle < RigidBody> rb) -> bool
		{
			return rb->use_gravity;
		}));

		Bind("idk.Bindings::RigidBodySetUseGravity", decay(
			[](Handle<RigidBody> rb, bool val)
		{
			rb->use_gravity = val;
		}));

		Bind("idk.Bindings::RigidBodySleep", decay(
			[](Handle<RigidBody> rb)
		{
			rb->sleep_next_frame = true;
		}));

		Bind("idk.Bindings::RigidBodyTeleport", decay(
			[](Handle<RigidBody> rb, vec3 val)
		{
			rb->TeleportBy(val);
		}));

		Bind("idk.Bindings::RigidBodyAddForce", decay(
			[](Handle<RigidBody> rb, vec3 val)
		{
			rb->AddForce(val);
		}));
		
		// Collider
		Bind("idk.Bindings::ColliderSetEnabled", decay(
			[](Handle<Collider> col, bool val)
		{
			col->enabled = val;
		}
		));

		Bind("idk.Bindings::ColliderGetEnabled", decay(
			[](Handle<Collider> col) ->bool
		{
			return col->enabled;
		}
		));

		Bind("idk.Bindings::ColliderSetTrigger", decay(
			[](Handle<Collider> col, bool val)
		{
			col->is_trigger = val;
		}
		));

		Bind("idk.Bindings::ColliderGetTrigger", decay(
			[](Handle<Collider> col) ->bool
		{
			return col->is_trigger;
		}
		));
	}
}
