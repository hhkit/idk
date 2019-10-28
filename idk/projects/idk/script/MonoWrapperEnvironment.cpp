#include "stdafx.h"
#include "MonoWrapperEnvironment.h"
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

#include <debug/LogSystem.h>
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
	}

	void MonoWrapperEnvironment::Init()
	{
		{
			auto img = mono_assembly_get_image(_assembly);
			auto klass = mono_class_from_name(img, "idk", "IDK");
			main = mono_class_get_method_from_name(klass, "Main", 1);
			void* args[] = { 0 };
			mono_runtime_invoke(main, nullptr, args, nullptr);
		}
		ScanTypes();

		auto mb_itr = _types.find("MonoBehavior");
		IDK_ASSERT_MSG(mb_itr != _types.end(), "cannot find idk.MonoBehavior");
		IDK_ASSERT_MSG(mb_itr->second.CacheThunk("UpdateCoroutines"), "could not cache method");
	}

	bool MonoWrapperEnvironment::IsPrivate(MonoClassField* field)
	{
		return !mono_method_can_access_field(main, field);
	}

	MonoWrapperEnvironment::~MonoWrapperEnvironment()
	{
		mono_jit_cleanup(_domain);
	}


	void MonoWrapperEnvironment::BindCoreFunctions()
	{

		constexpr auto Bind = mono_add_internal_call;

		// bdebug
		Bind("idk.Bindings::DebugLog", decay(
			[](MonoString* preface, MonoString* message)
		{
			LogSingleton::Get().LogMessage(LogLevel::INFO,
				LogPool::GAME,
				unbox(preface).get(),
				unbox(message).get()
			);
		}
		));
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

		Bind("idk.Bindings::GameObjectAddGameComponent", decay(
			[](Handle<GameObject> go, MonoString* component) -> MonoObject*
			{
				auto s = unbox(component);
				auto retval = go->AddComponent<mono::Behavior>();
				if (retval->EmplaceBehavior(s.get()))
					return retval->GetObject().Raw();
				
				go->RemoveComponent(retval);
				return nullptr;
			}
		));

		Bind("idk.Bindings::GameObjectGetGameComponent", decay(
			[](Handle<GameObject> go, MonoString* component) -> MonoObject* // note: return value optimization
			{
				auto s = unbox(component);
				auto& envi = Core::GetSystem<ScriptSystem>().Environment();
				for (auto& elem : go->GetComponents<mono::Behavior>())
				{
					string_view findme = s.get();
					auto type = mono_object_get_class(elem->GetObject().Raw());
					while (type != envi.Type("Object")->Raw())
					{
						if (mono_class_get_name(type) == findme)
							return elem->GetObject().Raw();
						type = mono_class_get_parent(type);
					}
				}

				return nullptr;
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

        Bind("idk.Bindings::GameObjectGetName", decay(
            [](Handle<GameObject> go) -> MonoString*
        {
            return mono_string_new(mono_domain_get(), go->Name().data());
        }
        ));

        Bind("idk.Bindings::GameObjectSetName", decay(
            [](Handle<GameObject> go, MonoString* name) -> void
        {
            char* s = mono_string_to_utf8(name);
            go->Name(s);
            mono_free(s);
        }
        ));

        Bind("idk.Bindings::GameObjectGetTag", decay(
            [](Handle<GameObject> go) -> MonoString*
        {
            return mono_string_new(mono_domain_get(), go->Tag().data());
        }
        ));

        Bind("idk.Bindings::GameObjectSetTag", decay(
            [](Handle<GameObject> go, MonoString* tag) -> void
        {
            char* s = mono_string_to_utf8(tag);
            go->Tag(s);
            mono_free(s);
        }
        ));

        Bind("idk.Bindings::GameObjectFindWithTag", decay(
            [](MonoString* tag) -> uint64_t
        {
            char* s = mono_string_to_utf8(tag);
            auto ret = Core::GetSystem<TagManager>().Find(s);
            mono_free(s);
            return ret.id;
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
		
		Bind("idk.Bindings::TransformGetParent", decay(
			[](Handle<Transform> h) -> uint64_t
			{
				return h->parent.id;
			}));

		Bind("idk.Bindings::TransformSetParent", decay(
			[](Handle<Transform> h, Handle<GameObject> parent_gameobject, bool preserve_global)
			{
				h->SetParent(parent_gameobject, preserve_global);
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
			[](Handle<RigidBody> rb) -> vec3
		{
			return rb->velocity() * Core::GetDT().count();
		}));

		Bind("idk.Bindings::RigidBodySetVelocity", decay(
			[](Handle<RigidBody> rb, vec3 val)
		{
			rb->velocity(val);
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

		// Animator
		Bind("idk.Bindings::AnimatorPlay", decay(
			[](Handle<Animator> animator, MonoString* name)
			{
				auto s = unbox(name);
				animator->Play(s.get());
			}
		));

		Bind("idk.Bindings::AnimatorCrossFade", decay(
			[](Handle<Animator> animator, MonoString* name, float time = 0.2f)
			{
				auto s = unbox(name);
				animator->BlendTo(s.get(), time);
			}
		));

		Bind("idk.Bindings::AnimatorPause", decay(
			[](Handle<Animator> animator)
			{
				animator->Pause();
			}
		));

		Bind("idk.Bindings::AnimatorResume", decay(
			[](Handle<Animator> animator)
			{
				animator->Resume();
			}
		));

		Bind("idk.Bindings::AnimatorStop", decay(
			[](Handle<Animator> animator)
			{
				animator->Stop();
			}
		));

		Bind("idk.Bindings::AnimatorDefaultStateName", decay(
			[](Handle<Animator> animator) ->MonoString* 
			{
				return mono_string_new(mono_domain_get(), animator->DefaultStateName().c_str());
			}
		));

		Bind("idk.Bindings::AnimatorCurrentStateName", decay(
			[](Handle<Animator> animator) ->MonoString* 
			{
				return mono_string_new(mono_domain_get(), animator->CurrentStateName().c_str());
			}
		));

		Bind("idk.Bindings::AnimatorBlendStateName", decay(
			[](Handle<Animator> animator) ->MonoString*
			{
				return mono_string_new(mono_domain_get(), animator->BlendStateName().c_str());
			}
		));

		Bind("idk.Bindings::AnimatorIsPlaying", decay(
			[](Handle<Animator> animator) -> bool
			{
				return animator->IsPlaying();
			}
		));

		Bind("idk.Bindings::AnimatorIsBlending", decay(
			[](Handle<Animator> animator) -> bool
			{
				return animator->IsBlending();
			}
		));

		Bind("idk.Bindings::AnimatorHasCurrAnimEnded", decay(
			[](Handle<Animator> animator) -> bool
			{
				return animator->HasCurrAnimEnded();
			}
		));

		Bind("idk.Bindings::AnimatorHasState", decay(
			[](Handle<Animator> animator, MonoString* name) -> bool
			{
				auto s = unbox(name);
				auto ret_val = animator->HasState(s.get());
				return ret_val;
			}
		));

        // Renderer
        Bind("idk.Bindings::RendererGetMaterialInstance", decay(
            [](GenericHandle renderer) -> Guid
        {
            switch (renderer.type)
            {
            case index_in_tuple_v<MeshRenderer, Handleables>: return handle_cast<MeshRenderer>(renderer)->material_instance.guid;
            case index_in_tuple_v<SkinnedMeshRenderer, Handleables>: return handle_cast<SkinnedMeshRenderer>(renderer)->material_instance.guid;
            default: return {};
            }
        }
        ));

        // Resource
#define VALIDATE_RESOURCE(RES) case string_hash(#RES): return Core::GetResourceManager().Validate<RES>(guid);
        Bind("idk.Bindings::ResourceValidate", decay(
            [](Guid guid, MonoString* type) -> bool
        {
            // TODO: make validate jumptable...
            auto s = unbox(type);
            auto hash = string_hash(s.get());

			LOG_TO(LogPool::GAME, string{ guid }.data());
            switch (hash)
            {
				VALIDATE_RESOURCE(Material);
				VALIDATE_RESOURCE(MaterialInstance);
				VALIDATE_RESOURCE(Prefab);
            default: return false;
            }
        }
        ));
#undef VALIDATE_RESOURCE

#define NAME_OF_RESOURCE(RES) case string_hash(#RES): return mono_string_new(mono_domain_get(), Core::GetResourceManager().Get<RES>(guid).Name().data());
        Bind("idk.Bindings::ResourceGetName", decay(
            [](Guid guid, MonoString* type) -> MonoString*
        {
            // TODO: make get jumptable...
			// TODO: research on perfect jumping
            auto s = unbox(type);
            auto hash = string_hash(s.get());

            switch (hash)
            {
				NAME_OF_RESOURCE(Material);
				NAME_OF_RESOURCE(MaterialInstance);
				NAME_OF_RESOURCE(Prefab);
                default: return mono_string_empty(mono_domain_get());
            }
        }
        ));
#undef NAME_OF_RESOURCE

		// Prefab
		Bind("idk.Bindings::PrefabInstantiate", decay(
			[](Guid guid) -> uint64_t
			{
				auto res = RscHandle<Prefab>{guid};
				if (res)
					return res->Instantiate(*Core::GetSystem<SceneManager>().GetActiveScene()).id;
				return 0;
			}
		));

        // MaterialInstance
        Bind("idk.Bindings::MaterialInstanceGetFloat", decay(
            [](RscHandle<MaterialInstance> handle, MonoString* name) -> float
        {
            auto s = unbox(name);
            if (!handle) { return 0; }
            auto res = handle->GetUniform(s.get());
            return res ? std::get<float>(*res) : 0;
        }
        ));
        Bind("idk.Bindings::MaterialInstanceGetVector2", decay(
            [](RscHandle<MaterialInstance> handle, MonoString* name) -> vec2
        {
			auto s = unbox(name);
			if (!handle) { return vec2{}; }
			auto res = handle->GetUniform(s.get());
            return res ? std::get<vec2>(*res) : vec2(0, 0);
        }
        ));
        Bind("idk.Bindings::MaterialInstanceGetVector3", decay(
            [](RscHandle<MaterialInstance> handle, MonoString* name) -> vec3
        {
			auto s = unbox(name);
			if (!handle) { return vec3{}; }
			auto res = handle->GetUniform(s.get());
            return res ? std::get<vec3>(*res) : vec3(0, 0, 0);
        }
        ));
        Bind("idk.Bindings::MaterialInstanceGetVector4", decay(
            [](RscHandle<MaterialInstance> handle, MonoString* name) -> vec4
        {
			auto s = unbox(name);
			if (!handle) { return vec4{}; }
			auto res = handle->GetUniform(s.get());
            return res ? std::get<vec4>(*res) : vec4(0, 0, 0, 0);
        }
        ));
        Bind("idk.Bindings::MaterialInstanceGetTexture", decay(
            [](RscHandle<MaterialInstance> handle, MonoString* name) -> Guid
        {
			auto s = unbox(name);
			if (!handle) { return Guid{}; }
			auto res = handle->GetUniform(s.get());
            return res ? std::get<RscHandle<Texture>>(*res).guid : Guid();
        }
        ));

        Bind("idk.Bindings::MaterialInstanceSetFloat", decay(
            [](RscHandle<MaterialInstance> handle, MonoString* name, float value) -> void
        {
			auto s = unbox(name);
			if (!handle)
				return;
			handle->SetUniform(s.get(), value);
        }
        ));
        Bind("idk.Bindings::MaterialInstanceSetVector2", decay(
            [](RscHandle<MaterialInstance> handle, MonoString* name, vec2 value) -> void
        {
			auto s = unbox(name);
			if (!handle)
				return;
			handle->SetUniform(s.get(), value);
        }
        ));
        Bind("idk.Bindings::MaterialInstanceSetVector3", decay(
            [](RscHandle<MaterialInstance> handle, MonoString* name, vec3 value) -> void
        {
				auto s = unbox(name);
				if (!handle)
					return;
				handle->SetUniform(s.get(), value);
        }
        ));
        Bind("idk.Bindings::MaterialInstanceSetVector4", decay(
            [](RscHandle<MaterialInstance> handle, MonoString* name, vec4 value) -> void
        {
            auto s = unbox(name);
            if (!handle) 
				return;
            handle->SetUniform(s.get(), value);
        }
        ));
        Bind("idk.Bindings::MaterialInstanceSetTexture", decay(
            [](RscHandle<MaterialInstance> handle, MonoString* name, RscHandle<Texture> tex) -> void
        {
			auto s = unbox(name);
			if (!handle)
				return;
			handle->SetUniform(s.get(), tex);
        }
        ));

		// Input
		Bind("idk.Bindings::InputGetKeyDown", decay(
			[](int code) -> bool
			{
				if (code & 0xFFFF0000)
					return Core::GetSystem<GamepadSystem>().GetButtonDown(0, s_cast<GamepadButton>(code >> 16));
				else
					return Core::GetSystem<Application>().GetKeyDown(s_cast<idk::Key>(code));
			}
		));
		Bind("idk.Bindings::InputGetKeyUp", decay(
			[](int code) -> bool
			{
				if (code & 0xFFFF0000)
					return Core::GetSystem<GamepadSystem>().GetButtonUp(0, s_cast<GamepadButton>(code >> 16));
				else
				return Core::GetSystem<Application>().GetKeyUp(s_cast<idk::Key>(code));
			}
		));
		Bind("idk.Bindings::InputGetKey", decay(
			[](int code) -> bool
			{
				if (code & 0xFFFF0000)
					return Core::GetSystem<GamepadSystem>().GetButton(0, s_cast<GamepadButton>(code >> 16));
				else
				return Core::GetSystem<Application>().GetKey(s_cast<idk::Key>(code));
			}
		));

		Bind("idk.Bindings::InputGetAxis", decay(
			[](char code, int axis) -> float
			{
				return Core::GetSystem<GamepadSystem>().GetAxis(code, s_cast<GamepadAxis>( axis));
			}
		));

		Bind("idk.Bindings::TimeGetFixedDelta", decay(
			[]() -> float
			{
				return Core::GetDT().count();
			}
		));

		Bind("idk.Bindings::TimeGetDelta", decay(
			[]() -> float
			{
				return Core::GetRealDT().count();
			}
		));
	}
}
