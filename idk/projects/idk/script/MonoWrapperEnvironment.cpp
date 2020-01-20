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
#include <mono/metadata/reflection.h>

#include <core/Scheduler.h>
#include <core/NullHandleException.h>
#include <IncludeComponents.h>
#include <IncludeResources.h>
#include <IncludeSystems.h>

#include <debug/LogSystem.h>
#include <script/ValueBoxer.h>
#include <script/ValueUnboxer.h>
#include <script/MonoBinder.h>

#include <core/GameObject.inl>
#include <util/ioutils.h>
#include <ds/span.inl>

#include <res/ResourceHandle.inl>


namespace idk::mono
{
	MonoWrapperEnvironment::MonoWrapperEnvironment(string_view full_path_to_game_dll)
	{
		// setup
		_domain = mono_jit_init("MasterDomain");

		// open file
		std::ifstream file{ full_path_to_game_dll, std::ios::binary };
		assembly_data = stringify(file);

		// load assembly
		mono_domain_set(_domain, true);
		_assembly = mono_domain_assembly_open(_domain, full_path_to_game_dll.data());
		// bind functions
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

	bool MonoWrapperEnvironment::IsAbstract(MonoType* type)
	{
		auto img = mono_assembly_get_image(_assembly);
		auto klass = mono_class_from_name(img, "idk", "IDK");
		main = mono_class_get_method_from_name(klass, "TypeIsAbstract", 1);
		void* args[] = { type, 0 };
		auto retval = mono_runtime_invoke(main, nullptr, args, nullptr);
		return *static_cast<bool*>(mono_object_unbox(retval));
	}

	MonoWrapperEnvironment::~MonoWrapperEnvironment()
	{
		mono_jit_cleanup(_domain);
	}

	/*
	template<typename T>
	constexpr auto Bind(string_view label, T&& fn)
	{
		mono_add_internal_call(label.data(), decay(fn));
	}
	*/
	
	template<typename T>
	struct default_val
	{
		static auto ret() { return T{}; }
	};

	template<>
	struct default_val<void>
	{
		static void ret() {}
	};

	MonoException* make_nullref_exception()
	{
		auto exc_type = Core::GetSystem<ScriptSystem>().Environment().Type("NullRef");
		IDK_ASSERT(exc_type);
		return (MonoException*) exc_type->ConstructTemporary();
	}

	struct vec4
	{
		float val[4];
		operator idk::vec4() const
		{
			return idk::vec4{ val[0], val[1], val[2], val[3] };
		}
	};

	struct quat
	{
		float val[4];
		operator idk::quat() const
		{
			return idk::quat{ val[0], val[1], val[2], val[3] };
		}
	};

	struct color
	{
		float val[4];
		operator idk::color() const
		{
			return idk::color{ val[0], val[1], val[2], val[3] };
		}
	};

	template<typename Ret>
	struct Retval { using T = Ret; };

	template<> struct Retval<vec4> { using T = idk::vec4; };
	template<> struct Retval<quat> { using T = idk::quat; };
	template<> struct Retval<color> { using T = idk::color; };

#define BIND_START(LABEL, RET, ...)\
	{ using Ret = Retval<RET>::T;\
	mono_add_internal_call(LABEL, decay([] (__VA_ARGS__) -> Ret{\
	using idk::quat; using idk::vec4; using idk::color;\
	try

#define BIND_END() \
	catch(NullHandleException ex) \
	{ \
		LOG_TO(LogPool::GAME, "Null reference at %lld {%llx}", ex.GetHandle().id, ex.GetHandle().id);\
		mono_raise_exception(make_nullref_exception());\
		return default_val<Ret>::ret(); }\
	}));}

	void MonoWrapperEnvironment::BindCoreFunctions()
	{
		//constexpr auto Bind = mono_add_internal_call;

		// bdebug
		BIND_START("idk.Bindings::DebugLog", void, MonoString * preface, MonoString * message)
		{
			LogSingleton::Get().LogMessage(LogLevel::INFO,
				LogPool::GAME,
				unbox(preface).get(),
				unbox(message).get()
			);
		}
		BIND_END();

		// handleable
		BIND_START("idk.Bindings::ObjectValidate", bool, GenericHandle go)
		{
			return GameState::GetGameState().ValidateHandle(go);
		}
		BIND_END();
		
		BIND_START("idk.Bindings::ObjectDestroy", void, GenericHandle go)
		{
			GameState::GetGameState().DestroyObject(go);
		}
		BIND_END();

		
		BIND_START("idk.Bindings::ObjectGetObjectsOfType", MonoArray*, MonoString* type)
		{
			auto s = unbox(type);
			string_view type_name = s.get();

			vector<Handle<mono::Behavior>> behaviors;
			for (auto& elem : Core::GetGameState().GetObjectsOfType<mono::Behavior>())
				if (elem.GetObject().Type()->IsOrDerivedFrom(type_name))
					behaviors.emplace_back(elem.GetHandle());

			auto klass = Core::GetSystem<ScriptSystem>().ScriptEnvironment().Type(type_name);

			IDK_ASSERT(klass);
			auto retval = mono_array_new(mono_domain_get(), klass->Raw(), behaviors.size());
			for (int i = 0; i < behaviors.size(); ++i)
				mono_array_setref(retval, i, behaviors[i]->GetObject().Raw());

			return retval;
		}
		BIND_END();

		// app
		BIND_START("idk.Bindings::EngineKill", void)
		{
			if (&Core::GetSystem<IEditor>() == nullptr)
				Core::Shutdown();
		}
		BIND_END();

		// scene
		BIND_START("idk.Bindings::SceneChangeScene", void, Guid scene)
		{
			Core::GetSystem<SceneManager>().SetNextScene(RscHandle<Scene>{scene});
		}
		BIND_END();

		// game object
		
		BIND_START("idk.Bindings::GameObjectNew", uint64_t)
		{
			return Core::GetSystem<SceneManager>().GetActiveScene()->CreateGameObject().id;
		}
		BIND_END();


		BIND_START("idk.Bindings::GameObjectAddEngineComponent",uint64_t, Handle<GameObject> go, MonoString* component)
			{
				return go->AddComponent(string_view{ unbox(component).get() }).id;
			}
		BIND_END();

		BIND_START("idk.Bindings::GameObjectGetEngineComponent",  uint64_t, Handle<GameObject> go, MonoString* component) // note: return value optimization
			{
				auto query = unbox(component);
				auto str = string_view{ query.get() };
				if (str == "Renderer")
				{
					auto mrend = go->GetComponent<MeshRenderer>();
					return mrend ? mrend.id : go->GetComponent<SkinnedMeshRenderer>().id;
				}

				return go->GetComponent(str).id;
			}
		BIND_END();

		BIND_START("idk.Bindings::GameObjectAddGameComponent",  MonoObject*, Handle<GameObject> go, MonoString* component)
			{
				auto s = unbox(component);
				auto retval = go->AddComponent<mono::Behavior>();
				if (retval->EmplaceBehavior(s.get()))
					return retval->GetObject().Raw();
				
				go->RemoveComponent(retval);
				return nullptr;
			}
		BIND_END();

		BIND_START("idk.Bindings::GameObjectGetGameComponent",  MonoObject*, Handle<GameObject> go, MonoString* component) // note: return value optimization
			{
				auto s = unbox(component);
				string_view findme = s.get();
				for (auto& elem : go->GetComponents<mono::Behavior>())
				{
					if (elem->GetObject().Type()->IsOrDerivedFrom(findme))
						return elem->GetObject().Raw();
				}

				return nullptr;
			}
		BIND_END();

		BIND_START("idk.Bindings::GameObjectGetActiveInHierarchy",  bool, Handle<GameObject> go)
			{
				return go->ActiveInHierarchy();
			}
		BIND_END();

		BIND_START("idk.Bindings::GameObjectActiveSelf",  bool, Handle<GameObject> go)
			{
				return go->ActiveSelf();
			}
		BIND_END();
		
		BIND_START("idk.Bindings::GameObjectSetActive",  void, Handle<GameObject> go, bool active)
			{
				go->SetActive(active);
			}
		BIND_END();

		BIND_START("idk.Bindings::GameObjectSetActive",  void, Handle<GameObject> go, bool active)
			{
				go->SetActive(active);
			}
		BIND_END();
		
        BIND_START("idk.Bindings::GameObjectGetName",  MonoString*, Handle<GameObject> go) //wtf?
        {
            return mono_string_new(mono_domain_get(), go->Name().data());
        }
        BIND_END();
		
        BIND_START("idk.Bindings::GameObjectSetName",  void, Handle<GameObject> go, MonoString* name)
        {
            auto s = unbox(name);
            go->Name(s.get());
        }
		BIND_END();

        BIND_START("idk.Bindings::GameObjectGetTag",  MonoString*, Handle<GameObject> go)
        {
            return mono_string_new(mono_domain_get(), go->Tag().data());
        }
		BIND_END();

        BIND_START("idk.Bindings::GameObjectSetTag",  void, Handle<GameObject> go, MonoString* tag)
        {
            char* s = mono_string_to_utf8(tag);
            go->Tag(s);
            mono_free(s);
        }
		BIND_END();

        BIND_START("idk.Bindings::GameObjectFindWithTag",  uint64_t, MonoString* tag)
        {
            char* s = mono_string_to_utf8(tag);
            auto ret = Core::GetSystem<TagManager>().Find(s);
            mono_free(s);
            return ret.id;
        }
		BIND_END();


		// behavior

		BIND_START("idk.Bindings::MonoBehaviorGetEnable", bool, Handle<mono::Behavior> mb)
		{
			return mb->enabled;
		}
		BIND_END();

		BIND_START("idk.Bindings::MonoBehaviorSetEnable", void, Handle<mono::Behavior> mb, bool set_enable)
		{
			mb->enabled = set_enable;
		}
		BIND_END();

		// component
		BIND_START("idk.Bindings::ComponentGetGameObject",  uint64_t, GenericHandle go)
		{
			return GameState::GetGameState().GetGameObject(go).id;
		}
		BIND_END();


		// transform

		BIND_START("idk.Bindings::TransformGetPosition",  vec3, Handle<Transform> h)
			{ 
				return h->GlobalPosition();
			}
		BIND_END();

		BIND_START("idk.Bindings::TransformSetPosition",  void, Handle<Transform> h, vec3 v)
			{
				h->GlobalPosition(v);
			}
		BIND_END();

		BIND_START("idk.Bindings::TransformGetScale",  vec3, Handle<Transform> h)
			{
				return h->GlobalScale();
			}
		BIND_END();

		BIND_START("idk.Bindings::TransformSetScale",  void, Handle<Transform> h, vec3 v)
			{
				if (v.length_sq() > epsilon)
					h->GlobalScale(v);
			}
		BIND_END();

		BIND_START("idk.Bindings::TransformGetRotation",  quat, Handle<Transform> h)
			{
				return h->GlobalRotation();
			}
		BIND_END();

		BIND_START("idk.Bindings::TransformSetRotation",  void, Handle<Transform> h, quat v)
			{
				h->GlobalRotation(v);
			}
		BIND_END();

		BIND_START("idk.Bindings::TransformGetLocalPosition", vec3, Handle<Transform> h)
		{
			return h->position;
		}
		BIND_END();

		BIND_START("idk.Bindings::TransformSetLocalPosition", void, Handle<Transform> h, vec3 v)
		{
			h->position = v;
		}
		BIND_END();

		BIND_START("idk.Bindings::TransformGetLocalScale", vec3, Handle<Transform> h)
		{
			return h->scale;
		}
		BIND_END();

		BIND_START("idk.Bindings::TransformSetLocalScale", void, Handle<Transform> h, vec3 v)
		{
			if (v.length_sq() > epsilon)
				h->scale = v;
		}
		BIND_END();

		BIND_START("idk.Bindings::TransformGetLocalRotation", quat, Handle<Transform> h)
		{
			return h->rotation;
		}
		BIND_END();

		BIND_START("idk.Bindings::TransformSetLocalRotation", void, Handle<Transform> h, quat v)
		{
			h->rotation = v;
		}
		BIND_END();

		BIND_START("idk.Bindings::TransformForward",  vec3, Handle<Transform> h)
			{
				return h->Forward();
			}
		BIND_END();

		BIND_START("idk.Bindings::TransformUp",  vec3, Handle<Transform> h)
			{
				return h->Up();
			}
		BIND_END();

		BIND_START("idk.Bindings::TransformRight",  vec3, Handle<Transform> h)
			{
				return h->Right();
			}
		BIND_END();
		
		BIND_START("idk.Bindings::TransformGetParent",  uint64_t, Handle<Transform> h)
			{
				return h->parent.id;
			}
		BIND_END();

		BIND_START("idk.Bindings::TransformSetParent",  void, Handle<Transform> h, Handle<GameObject> parent_gameobject, bool preserve_global)
			{
				h->SetParent(parent_gameobject, preserve_global);
				Core::GetSystem<SceneManager>().ReparentObject(h->GetGameObject(), parent_gameobject);
			}
		BIND_END();

		BIND_START("idk.Bindings::TransformGetChildren", MonoArray*, Handle<Transform> h)
		{
			auto sg = Core::GetSystem<SceneManager>().FetchSceneGraphFor(h->GetGameObject());

			auto go_klass = Core::GetSystem<mono::ScriptSystem>().Environment().Type("GameObject");

			auto retval = mono_array_new(mono_domain_get(), go_klass->Raw(), sg ? sg->size() : 0);
			if (sg)
			{
				auto sz = sg->size();
				auto ptr = sg->begin();
				for (int i = 0; i < sz; ++i)
				{
					auto mo = mono_object_new(mono_domain_get(), go_klass->Raw());
					auto method = mono_class_get_method_from_name(go_klass->Raw(), ".ctor", 1);
					void* args[] = { &ptr++->obj.id };
					mono_runtime_invoke(method, mo, args, nullptr);
					mono_array_setref(retval, i, mo);
				}
			}
			return retval;
		}
		BIND_END();

		BIND_START("idk.Bindings::TransformSetForward", void, Handle<Transform> h, vec3 forward)
		{
			auto curr_fwd = h->Forward();
			const float new_f_len = forward.length();
			forward /= new_f_len;
			const float new_f_dot_curr = forward.dot(curr_fwd);

			// If dot prod returns us the square of the length, we know they are the same vector.
			// In which case, we just return and dont apply any rotation
			if (fabs(new_f_dot_curr - 1.0f) < 0.001f)
				return;

			// if the two vectors are inversed
			if (fabs(new_f_dot_curr - (-1.0f)) < 0.001f)
			{
				auto up = h->Up();
				h->GlobalRotation(quat{ up, deg{180} } *h->GlobalRotation());
				return;
			}

			auto axis = curr_fwd.cross(forward);
			auto axis_len = axis.length();

			auto angle = acos(new_f_dot_curr);
			axis /= axis_len;

			h->GlobalRotation(quat{axis, angle} * h->GlobalRotation());
		}
		BIND_END();

		// physics
		struct ManagedRaycast
		{
			bool valid;
			uint64_t collider_id;
			float distance;
			vec3 normal;
			vec3 point_of_collision;
		};
		BIND_START("idk.Bindings::PhysicsRaycast", ManagedRaycast, vec3 origin, vec3 direction, float max_dist, int mask, bool hit_triggers)
		{
			auto res = Core::GetSystem<PhysicsSystem>().Raycast(ray{ .origin = origin,.velocity = direction }, LayerMask{ mask }, hit_triggers);
			if (res.size())
			{
				auto& first = res.front();
				if (first.raycast_succ.distance_to_collision < max_dist)
					return ManagedRaycast{ .valid = true,.collider_id = first.collider.id,.distance = first.raycast_succ.distance_to_collision,.point_of_collision = first.raycast_succ.point_of_collision };
			}

			return ManagedRaycast{ .valid = false };
		}
		BIND_END()

		// RigidBody
		BIND_START("idk.Bindings::RigidBodyGetMass", float, Handle<RigidBody> rb)
		{
			return rb->mass();
		}
		BIND_END();

		BIND_START("idk.Bindings::RigidBodySetMass",  void, Handle<RigidBody> rb, float val)
		{
			rb->mass(val);
		}
		BIND_END();

		BIND_START("idk.Bindings::RigidBodyGetVelocity",  vec3, Handle<RigidBody> rb)
		{
			return rb->velocity() * Core::GetDT().count();
		}
		BIND_END();

		BIND_START("idk.Bindings::RigidBodySetVelocity",  void, Handle<RigidBody> rb, vec3 val)
		{
			rb->velocity(val);
		}
		BIND_END();

		BIND_START("idk.Bindings::RigidBodyGetPosition",  vec3, Handle < RigidBody> rb)
		{
			return rb->position();
		}
		BIND_END();

		BIND_START("idk.Bindings::RigidBodySetPosition",  void, Handle<RigidBody> rb, vec3 val)
		{
			rb->position(val);
		}
		BIND_END();

		BIND_START("idk.Bindings::RigidBodyGetUseGravity",  bool, Handle < RigidBody> rb)
		{
			return rb->use_gravity;
		}
		BIND_END();

		BIND_START("idk.Bindings::RigidBodySetUseGravity",  void, Handle<RigidBody> rb, bool val)
		{
			rb->use_gravity = val;
		}
		BIND_END();

		BIND_START("idk.Bindings::RigidBodyGetIsKinematic", bool, Handle < RigidBody> rb)
		{
			return rb->is_kinematic;
		}
		BIND_END();

		BIND_START("idk.Bindings::RigidBodySetIsKinematic", void, Handle<RigidBody> rb, bool val)
		{
			rb->is_kinematic= val;
		}
		BIND_END();

		BIND_START("idk.Bindings::RigidBodySleep",  void, Handle<RigidBody> rb)
		{
			rb->sleep_next_frame = true;
		}
		BIND_END();

		BIND_START("idk.Bindings::RigidBodyTeleport",  void, Handle<RigidBody> rb, vec3 val)
		{
			rb->TeleportBy(val);
		}
		BIND_END();

		BIND_START("idk.Bindings::RigidBodyAddForce",  void, Handle<RigidBody> rb, vec3 val)
		{
			rb->AddForce(val);
		}
		BIND_END();
		
		// Collider
		BIND_START("idk.Bindings::ColliderSetEnabled",  void, Handle<Collider> col, bool val)
		{
			col->enabled = val;
		}
		BIND_END();

		BIND_START("idk.Bindings::ColliderGetEnabled", bool, Handle<Collider> col)
		{
			return col->enabled;
		}
		BIND_END();

		BIND_START("idk.Bindings::ColliderSetTrigger",  void, Handle<Collider> col, bool val)
		{
			col->is_trigger = val;
		}
		BIND_END();

		BIND_START("idk.Bindings::ColliderGetTrigger", bool, Handle<Collider> col)
		{
			return col->is_trigger;
		}
		BIND_END();


		// BIND_START("idk.Bindings::ColliderGetShape", MonoObject*, Handle<Collider> col)
		// {
		// 	return std::visit([&](auto& elem) -> MonoObject *
		// 		{
		// 			using T = std::decay_t<decltype(elem)>;
		// 			
		// 			if constexpr (std::is_same_v<T, idk::box>)
		// 			{
		// 				idk::box& shape = elem;
		// 				auto box_klass = Core::GetSystem<ScriptSystem>().Environment().Type("Box");
		// 				IDK_ASSERT(box_klass);
		// 				auto box_obj = box_klass->Construct();
		// 				box_obj.Visit([](const auto& key, auto& val, int depth)
		// 					{
		// 						using T = std::decay_t<decltype(val)>;
		// 						if constexpr (std::is_same_v<T, vec3>)
		// 						{
		// 							if (key == "center")
		// 								val = shape.center;
		// 							if (key == "extents")
		// 								val = shape.extents;
		// 						}
		// 					});
		// 				return box_obj.Raw();
		// 			}
		// 
		// 			if constexpr (std::is_same_v<T, idk::sphere>)
		// 			{
		// 				idk::sphere& shape = elem;
		// 				auto sphere_klass = Core::GetSystem<ScriptSystem>().Environment().Type("Sphere");
		// 				IDK_ASSERT(sphere_klass);
		// 				auto sphere_obj = sphere_klass->Construct();
		// 				sphere_obj.Visit([](const auto& key, auto& val, int depth)
		// 					{
		// 						using T = std::decay_t<decltype(val)>;
		// 						if constexpr (std::is_same_v<T, vec3>)
		// 						{
		// 							if (key == "center")
		// 								val = shape.center;
		// 						}
		// 						if constexpr (std::is_same_v<T, idk::real>)
		// 						{
		// 							if (key == "radius")
		// 								val = shape.radius;
		// 						}
		// 					});
		// 				return sphere_obj.Raw();
		// 			}
		// 		}, col->shape);
		// }
		// BIND_END();
		// 
		// BIND_START("idk.Bindings::ColliderSetShape", void, Handle<Collider> col, MonoObject* obj)
		// {
		// 	ManagedObject managed_shape{ obj };
		// 	if (managed_shape.TypeName() == "Box")
		// 	{
		// 		idk::box box_obj;
		// 		managed_shape.Visit([](const auto& key, auto& val, int depth)
		// 			{
		// 				using T = std::decay_t<decltype(val)>;
		// 				if constexpr (std::is_same_v<T, vec3>)
		// 				{
		// 					if (key == "center")
		// 						box_obj.center = val;
		// 					if (key == "extents")
		// 						box_obj.extents = val;
		// 				}
		// 			});
		// 		col->shape = box_obj;
		// 	}
		// 
		// 	if (managed_shape.TypeName() == "Sphere")
		// 	{
		// 		idk::sphere sphere_obj;
		// 		managed_shape.Visit([](const auto& key, auto& val, int depth)
		// 			{
		// 				using T = std::decay_t<decltype(val)>;
		// 				if constexpr (std::is_same_v<T, vec3>)
		// 				{
		// 					if (key == "center")
		// 						sphere_obj.center = val;
		// 				}
		// 				if constexpr (std::is_same_v<T, idk::real>)
		// 				{
		// 					if (key == "radius")
		// 						sphere_obj.radius = val;
		// 				}
		// 			});
		// 		col->shape = sphere_obj;
		// 	}
		// }
		// BIND_END();

		// Animator
		BIND_START("idk.Bindings::AnimatorPlay",  void, Handle<Animator> animator, MonoString* name)
		{
			auto s = unbox(name);
			animator->Play(s.get());
		}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorCrossFade",  void, Handle<Animator> animator, MonoString* name, float time = 0.2f)
		{
			auto s = unbox(name);
			animator->BlendTo(s.get(), time);
		}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorPause",  void, Handle<Animator> animator)
		{
			animator->Pause();
		}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorResume",  void, Handle<Animator> animator)
		{
			animator->Resume();
		}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorStop",  void, Handle<Animator> animator)
		{
			animator->Stop();
		}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorDefaultStateName", MonoString*, Handle<Animator> animator)
			{
				return mono_string_new(mono_domain_get(), animator->DefaultStateName().c_str());
			}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorCurrentStateName", MonoString*, Handle<Animator> animator)
			{
				return mono_string_new(mono_domain_get(), animator->CurrentStateName().c_str());
			}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorBlendStateName", MonoString*, Handle<Animator> animator)
			{
				return mono_string_new(mono_domain_get(), animator->BlendStateName().c_str());
			}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorIsPlaying",  bool, Handle<Animator> animator)
			{
				return animator->IsPlaying();
			}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorIsBlending",  bool, Handle<Animator> animator)
			{
				return animator->IsBlending();
			}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorHasCurrAnimEnded",  bool, Handle<Animator> animator)
			{
				return animator->HasCurrAnimEnded();
			}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorHasState",  bool, Handle<Animator> animator, MonoString* name)
			{
				auto s = unbox(name);
				auto ret_val = animator->HasState(s.get());
				return ret_val;
			}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorGetInt", int, Handle<Animator> animator, MonoString* name)
		{
			auto s = unbox(name);
			return animator->GetInt(s.get());
		}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorGetFloat", float, Handle<Animator> animator, MonoString* name)
		{
			auto s = unbox(name);
			return animator->GetFloat(s.get());
		}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorGetBool", bool, Handle<Animator> animator, MonoString* name)
		{
			auto s = unbox(name);
			return animator->GetBool(s.get());
		}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorGetTrigger", bool, Handle<Animator> animator, MonoString* name)
		{
			auto s = unbox(name);
			return animator->GetTrigger(s.get());
		}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorSetInt", bool, Handle<Animator> animator, MonoString* name, int val)
		{
			auto s = unbox(name);
			return animator->SetInt(s.get(), val);
		}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorSetFloat", bool, Handle<Animator> animator, MonoString* name, float val)
		{
			auto s = unbox(name);
			return animator->SetFloat(s.get(), val);
		}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorSetBool", bool, Handle<Animator> animator, MonoString* name, bool val)
		{
			auto s = unbox(name);
			return animator->SetBool(s.get(), val);
		}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorSetTrigger", bool, Handle<Animator> animator, MonoString* name, bool val)
		{
			auto s = unbox(name);
			return animator->SetTrigger(s.get(), val);
		}
		BIND_END();

		//AudioSource
		//----------------------------------------------------------------------------------------------------
		BIND_START("idk.Bindings::AudioSourcePlay", void, Handle<AudioSource> audiosource, int index)
		{
			audiosource->Play(index);
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSourcePlayAll", void, Handle<AudioSource> audiosource)
		{
			audiosource->PlayAll();
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSourceStop", void, Handle<AudioSource> audiosource, int index)
		{
			audiosource->Stop(index);
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSourceStopAll", void, Handle<AudioSource> audiosource)
		{
			audiosource->StopAll();
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSourceGetVolume", float, Handle<AudioSource> audiosource)
		{
			return audiosource->volume;
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSourceSetVolume", void, Handle<AudioSource> audiosource, float volume)
		{
			audiosource->volume = volume;
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSourceClipGetVolume", float, Handle<AudioSource> audiosource, int index)
		{
			return audiosource->audio_clip_volume[index];
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSourceClipSetVolume", void, Handle<AudioSource> audiosource, int index, float volume)
		{
			audiosource->audio_clip_volume[index] = volume;
		}
		BIND_END();


		BIND_START("idk.Bindings::AudioSourceGetPitch", float, Handle<AudioSource> audiosource)
		{
			return audiosource->pitch;
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSourceSetPitch", void, Handle<AudioSource> audiosource, float pitch)
		{
			audiosource->pitch = pitch;
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSourceGetLoop", bool, Handle<AudioSource> audiosource)
		{
			return audiosource->isLoop;
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSourceSetLoop", void, Handle<AudioSource> audiosource, bool loop)
		{
			audiosource->isLoop = loop;
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSourceSize", int, Handle<AudioSource> audiosource)
		{
			return static_cast<int>(audiosource->audio_clip_list.size());
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSourceIsAudioClipPlaying", bool, Handle<AudioSource> audiosource, int index)
		{
			return audiosource->IsAudioClipPlaying(index);
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSourceIsAnyAudioClipPlaying", bool, Handle<AudioSource> audiosource)
		{
			return audiosource->IsAnyAudioClipPlaying();
		}
		BIND_END();

		BIND_START("idk.Bindings::FindAudio", int, Handle<AudioSource> audiosource, MonoString* name)
		{
			auto s = unbox(name);
			return audiosource->FindAudio(s.get());
		}
		BIND_END();

		BIND_START("idk.Bindings::AddAudioClip", int, Handle<AudioSource> audiosource, MonoString* name)
		{
			auto s = unbox(name);
			return audiosource->AddAudioClip(s.get());
		}
		BIND_END();
		//----------------------------------------------------------------------------------------------------

		// Renderer
        BIND_START("idk.Bindings::RendererGetMaterialInstance",  Guid, GenericHandle renderer)
        {
            switch (renderer.type)
            {
            case index_in_tuple_v<MeshRenderer, Handleables>: return handle_cast<MeshRenderer>(renderer)->material_instance.guid;
			case index_in_tuple_v<SkinnedMeshRenderer, Handleables>: return handle_cast<SkinnedMeshRenderer>(renderer)->material_instance.guid;
            default: return {};
            }
        }
        BIND_END();

		BIND_START("idk.Bindings::RendererSetMaterialInstance", void, GenericHandle renderer, Guid guid)
		{
			switch (renderer.type)
			{
			case index_in_tuple_v<MeshRenderer, Handleables>: handle_cast<MeshRenderer>(renderer)->material_instance = RscHandle<MaterialInstance>{ guid }; return;
			case index_in_tuple_v<SkinnedMeshRenderer, Handleables>: handle_cast<SkinnedMeshRenderer>(renderer)->material_instance = RscHandle<MaterialInstance>{ guid }; return;
			default: return;
			}
		}
		BIND_END();

		BIND_START("idk.Bindings::RendererGetActive", bool, GenericHandle renderer)
		{
			switch (renderer.type)
			{
			case index_in_tuple_v<MeshRenderer, Handleables>: return handle_cast<MeshRenderer>(renderer)->enabled;
			case index_in_tuple_v<SkinnedMeshRenderer, Handleables>: return handle_cast<SkinnedMeshRenderer>(renderer)->enabled;
			default: return {};
			}
		}
		BIND_END();

		BIND_START("idk.Bindings::RendererSetActive", void, GenericHandle renderer, bool set)
		{
			switch (renderer.type)
			{
            case index_in_tuple_v<MeshRenderer, Handleables>: handle_cast<MeshRenderer>(renderer)->enabled = set; return;
			case index_in_tuple_v<SkinnedMeshRenderer, Handleables>: handle_cast<SkinnedMeshRenderer>(renderer)->enabled = set; return;
			default: return;
			}
		}
		BIND_END();

        // Resource
#define VALIDATE_RESOURCE(RES) case string_hash(#RES): return Core::GetResourceManager().Validate<RES>(guid);
        BIND_START("idk.Bindings::ResourceValidate",  bool, Guid guid, MonoString* type)
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
		BIND_END();
#undef VALIDATE_RESOURCE

#define NAME_OF_RESOURCE(RES) case string_hash(#RES): return mono_string_new(mono_domain_get(), Core::GetResourceManager().Get<RES>(guid).Name().data());
        BIND_START("idk.Bindings::ResourceGetName",  MonoString*, Guid guid, MonoString* type)
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
        BIND_END();
#undef NAME_OF_RESOURCE

		// Prefab
		BIND_START("idk.Bindings::PrefabInstantiate",  uint64_t, Guid guid)
			{
				auto res = RscHandle<Prefab>{guid};
				if (res)
					return res->Instantiate(*Core::GetSystem<SceneManager>().GetActiveScene()).id;
				return 0;
			}
		BIND_END();

        // MaterialInstance
        BIND_START("idk.Bindings::MaterialInstanceGetFloat",  float, RscHandle<MaterialInstance> handle, MonoString* name)
        {
            auto s = unbox(name);
            if (!handle) { return 0; }
            auto res = handle->GetUniform(s.get());
            return res ? std::get<float>(*res) : 0;
        }
		BIND_END();
        BIND_START("idk.Bindings::MaterialInstanceGetVector2",  vec2, RscHandle<MaterialInstance> handle, MonoString* name)
        {
			auto s = unbox(name);
			if (!handle) { return vec2{}; }
			auto res = handle->GetUniform(s.get());
            return res ? std::get<vec2>(*res) : vec2(0, 0);
        }
		BIND_END();
        BIND_START("idk.Bindings::MaterialInstanceGetVector3",  vec3, RscHandle<MaterialInstance> handle, MonoString* name)
        {
			auto s = unbox(name);
			if (!handle) { return vec3{}; }
			auto res = handle->GetUniform(s.get());
            return res ? std::get<vec3>(*res) : vec3(0, 0, 0);
        }
		BIND_END();
        BIND_START("idk.Bindings::MaterialInstanceGetVector4",  vec4, RscHandle<MaterialInstance> handle, MonoString* name)
        {
			auto s = unbox(name);
			if (!handle) { return vec4{}; }
			auto res = handle->GetUniform(s.get());
            return res ? std::get<vec4>(*res) : vec4(0, 0, 0, 0);
        }
		BIND_END();
        BIND_START("idk.Bindings::MaterialInstanceGetTexture",  Guid, RscHandle<MaterialInstance> handle, MonoString* name)
        {
			auto s = unbox(name);
			if (!handle) { return Guid{}; }
			auto res = handle->GetUniform(s.get());
            return res ? std::get<RscHandle<Texture>>(*res).guid : Guid();
        }
		BIND_END();

        BIND_START("idk.Bindings::MaterialInstanceSetFloat",  void, RscHandle<MaterialInstance> handle, MonoString* name, float value)
        {
			auto s = unbox(name);
			if (!handle)
				return;
			handle->SetUniform(s.get(), value);
        }
		BIND_END();
        BIND_START("idk.Bindings::MaterialInstanceSetVector2",  void, RscHandle<MaterialInstance> handle, MonoString* name, vec2 value)
        {
			auto s = unbox(name);
			if (!handle)
				return;
			handle->SetUniform(s.get(), value);
        }
		BIND_END();
        BIND_START("idk.Bindings::MaterialInstanceSetVector3",  void, RscHandle<MaterialInstance> handle, MonoString* name, vec3 value)
        {
				auto s = unbox(name);
				if (!handle)
					return;
				handle->SetUniform(s.get(), value);
        }
		BIND_END();
        BIND_START("idk.Bindings::MaterialInstanceSetVector4",  void, RscHandle<MaterialInstance> handle, MonoString* name, vec4 value)
        {
            auto s = unbox(name);
            if (!handle) 
				return;
            handle->SetUniform(s.get(), value);
        }
		BIND_END();
        BIND_START("idk.Bindings::MaterialInstanceSetTexture",  void, RscHandle<MaterialInstance> handle, MonoString* name, RscHandle<Texture> tex)
        {
			auto s = unbox(name);
			if (!handle)
				return;
			handle->SetUniform(s.get(), tex);
        }
		BIND_END();

		// //////TextMesh///////////////
		BIND_START("idk.Bindings::TextMeshGetText", MonoString*, Handle<TextMesh> h)
		{
            return mono_string_new(mono_domain_get(), h->text.c_str());
		}
		BIND_END();

		BIND_START("idk.Bindings::TextMeshSetText", void, Handle<TextMesh> h, MonoString* str)
		{
            auto s = unbox(str);
			h->text = s.get();
		}
		BIND_END();

		BIND_START("idk.Bindings::TextMeshGetColor", color, Handle<TextMesh> h)
		{
			return h->color;
		}
		BIND_END();

		BIND_START("idk.Bindings::TextMeshSetColor", void, Handle<TextMesh> h, color r)
		{
			h->color = r;
		}
		BIND_END();

		// //////Camera///////////////
		BIND_START("idk.Bindings::CameraGetFOV", rad, Handle<Camera> h)
		{
			return h->field_of_view;
		}
		BIND_END();

		BIND_START("idk.Bindings::CameraSetFOV", void, Handle<Camera> h, rad r)
		{
			h->field_of_view = r;
		}
		BIND_END();

		BIND_START("idk.Bindings::CameraGetNearPlane", real, Handle<Camera> h)
		{
			return h->near_plane;
		}
		BIND_END();

		BIND_START("idk.Bindings::CameraSetNearPlane", void, Handle<Camera> h, real r)
		{
			h->near_plane = r;
		}
		BIND_END();

		BIND_START("idk.Bindings::CameraGetFarPlane", real, Handle<Camera> h)
		{
			return h->far_plane;
		}
		BIND_END();

		BIND_START("idk.Bindings::CameraSetFarPlane", void, Handle<Camera> h, real r)
		{
			h->far_plane = r;
		}
		BIND_END();


		BIND_START("idk.Bindings::CameraGetViewport", rect, Handle<Camera> h)
		{
			return h->viewport;
		}
		BIND_END();

		BIND_START("idk.Bindings::CameraSetViewport", void, Handle<Camera> h, rect r)
		{
			h->viewport = r;
		}
		BIND_END();

		BIND_START("idk.Bindings::CameraGetEnabledState", bool, Handle<Camera> h)
		{
			return h->enabled;
		}
		BIND_END();

		BIND_START("idk.Bindings::CameraSetEnabledState", void, Handle<Camera> h, bool r)
		{
			h->enabled = r;
		}
		BIND_END();

		// //////lights////////////////
		BIND_START("idk.Bindings::LightGetColor", color, Handle<Light> h)
		{
			return h->GetColor();
		}
		BIND_END();

		BIND_START("idk.Bindings::LightSetColor", void, Handle<Light> h,color c)
		{
			h->SetColor(c);
		}
		BIND_END();

		BIND_START("idk.Bindings::LightGetCastShadow", bool, Handle<Light> h)
		{
			return h->GetCastShadow();
		}
		BIND_END();

		BIND_START("idk.Bindings::LightSetCastShadow", void, Handle<Light> h, bool i)
		{
			h->SetCastShadow(i);
		}
		BIND_END();

		BIND_START("idk.Bindings::LightGetShadowBias", real, Handle<Light> h)
		{
			return h->GetShadowBias();
		}
		BIND_END();

		BIND_START("idk.Bindings::LightSetShadowBias", void, Handle<Light> h, real i)
		{
			h->SetShadowBias(i);
		}
		BIND_END();

		BIND_START("idk.Bindings::LightGetIntensity", real, Handle<Light> h)
		{
			return h->GetLightIntensity();
		}
		BIND_END();

		BIND_START("idk.Bindings::LightSetIntensity", void, Handle<Light> h, real i)
		{
			h->SetLightIntensity(i);
		}
		BIND_END();

		BIND_START("idk.Bindings::LightGetAttenuationRadius", real, Handle<Light> h)
		{
			return std::visit([&](auto& light_variant)-> const real
			{
				using T = std::decay_t<decltype(light_variant)>;
				if constexpr (
					std::is_same_v<T, PointLight> ||
					std::is_same_v<T, SpotLight>
					)
					return light_variant.GetAttenuationRadius();
				else
					return 0.f;
			}
			, h->light);
		}
		BIND_END();

		BIND_START("idk.Bindings::LightSetAttenuationRadius", void, Handle<Light> h, real i)
		{
			std::visit([&, val = i](auto& light_variant)
			{
				using T = std::decay_t<decltype(light_variant)>;
				if constexpr (std::is_same_v<T, PointLight> ||
					std::is_same_v<T, SpotLight>
					)
					light_variant.SetAttenuationRadius(val);
			}
			, h->light);
		}
		BIND_END();

		BIND_START("idk.Bindings::LightGetIsInverseAttSqRadius", bool, Handle<Light> h)
		{
			return std::visit([&](auto& light_variant)-> const bool
			{
				using T = std::decay_t<decltype(light_variant)>;
				if constexpr (
					std::is_same_v<T, PointLight> ||
					std::is_same_v<T, SpotLight>
					)
					return light_variant.GetInvSqAtten();
				else
					return false;
			}
			, h->light);
		}
		BIND_END();

		BIND_START("idk.Bindings::LightSetIsInverseAttSqRadius", void, Handle<Light> h, bool i)
		{
			std::visit([&, val = i](auto& light_variant)
			{
				using T = std::decay_t<decltype(light_variant)>;
				if constexpr (std::is_same_v<T, PointLight> ||
					std::is_same_v<T, SpotLight>
					)
					light_variant.SetInvSqAtten(val);
			}
			, h->light);
		}
		BIND_END();


		// Input

		BIND_START("idk.Bindings::InputGetKeyDown",  bool, int code)
		{
			if (code & 0xFFFF0000)
				return Core::GetSystem<GamepadSystem>().GetButtonDown((code >> 8) & 0xFF, s_cast<GamepadButton>(code >> 16));
			else
				return Core::GetSystem<Application>().GetKeyDown(s_cast<idk::Key>(code));
		}
		BIND_END();

		BIND_START("idk.Bindings::InputGetKeyUp",  bool, int code)
		{
			if (code & 0xFFFF0000)
				return Core::GetSystem<GamepadSystem>().GetButtonUp((code >> 8) & 0xFF, s_cast<GamepadButton>(code >> 16));
			else
			return Core::GetSystem<Application>().GetKeyUp(s_cast<idk::Key>(code));
		}
		BIND_END();

		BIND_START("idk.Bindings::InputGetKey",  bool, int code)
		{
			if (code & 0xFFFF0000)
				return Core::GetSystem<GamepadSystem>().GetButton((code >> 8) & 0xFF, s_cast<GamepadButton>(code >> 16));
			else
			return Core::GetSystem<Application>().GetKey(s_cast<idk::Key>(code));
		}
		BIND_END();

		BIND_START("idk.Bindings::InputGetAxis",  float, char code, int axis)
		{
			return Core::GetSystem<GamepadSystem>().GetAxis(code, s_cast<GamepadAxis>( axis));
		}
		BIND_END();

		BIND_START("idk.Bindings::InputSetRumble", void, char code, float low_freq, float high_freq)
		{
			Core::GetSystem<GamepadSystem>().SetRumble(code, low_freq, high_freq);
		}
		BIND_END();

        BIND_START("idk.Bindings::InputGetConnectedPlayers", char)
        {
            return Core::GetSystem<GamepadSystem>().GetConnectedPlayers();
        }
        BIND_END();


        // Time

		BIND_START("idk.Bindings::TimeGetTimeScale", float)
		{
			return Core::GetScheduler().time_scale;
		}
		BIND_END();

		BIND_START("idk.Bindings::TimeSetTimeScale", void, float ts)
		{
			Core::GetScheduler().time_scale = ts;
		}
		BIND_END();

		BIND_START("idk.Bindings::TimeGetFixedDelta",  float)
			{
				return Core::GetScheduler().GetFixedDeltaTime().count();
			}
		BIND_END();

		BIND_START("idk.Bindings::TimeGetDelta",  float)
			{
				return Core::GetScheduler().GetDeltaTime().count();
			}
		BIND_END();

		BIND_START("idk.Bindings::TimeGetUnscaledFixedDelta", float)
		{
			return Core::GetScheduler().GetFixedDeltaTime().count();
		}
		BIND_END();

		BIND_START("idk.Bindings::TimeGetUnscaledDelta", float)
		{
			return Core::GetScheduler().GetUnscaledDeltaTime().count();
		}
		BIND_END();



        // RectTransform

        BIND_START("idk.Bindings::RectTransformGetOffsetMin", vec2, Handle<RectTransform> h)
        {
            return h->offset_min;
        }
        BIND_END();

        BIND_START("idk.Bindings::RectTransformSetOffsetMin", void, Handle<RectTransform> h, vec2 v)
        {
            h->offset_min = v;
        }
        BIND_END();

        BIND_START("idk.Bindings::RectTransformGetOffsetMax", vec2, Handle<RectTransform> h)
        {
            return h->offset_max;
        }
        BIND_END();

        BIND_START("idk.Bindings::RectTransformSetOffsetMax", void, Handle<RectTransform> h, vec2 v)
        {
            h->offset_max = v;
        }
        BIND_END();

        BIND_START("idk.Bindings::RectTransformGetAnchorMin", vec2, Handle<RectTransform> h)
        {
            return h->anchor_min;
        }
        BIND_END();

        BIND_START("idk.Bindings::RectTransformSetAnchorMin", void, Handle<RectTransform> h, vec2 v)
        {
            h->anchor_min = v;
        }
        BIND_END();

        BIND_START("idk.Bindings::RectTransformGetAnchorMax", vec2, Handle<RectTransform> h)
        {
            return h->anchor_max;
        }
        BIND_END();

        BIND_START("idk.Bindings::RectTransformSetAnchorMax", void, Handle<RectTransform> h, vec2 v)
        {
            h->anchor_max = v;
        }
        BIND_END();

        BIND_START("idk.Bindings::RectTransformGetPivot", vec2, Handle<RectTransform> h)
        {
            return h->pivot;
        }
        BIND_END();

        BIND_START("idk.Bindings::RectTransformSetPivot", void, Handle<RectTransform> h, vec2 v)
        {
            h->pivot = v;
        }
        BIND_END();


        // Image

        BIND_START("idk.Bindings::ImageGetTexture", Guid, Handle<idk::Image> h)
        {
            return h->texture.guid;
        }
        BIND_END();

        BIND_START("idk.Bindings::ImageSetTexture", void, Handle<idk::Image> h, Guid guid)
        {
            h->texture = RscHandle<Texture>{ guid };
        }
        BIND_END();

        BIND_START("idk.Bindings::ImageGetMaterialInstance", Guid, Handle<idk::Image> h)
        {
            return h->material.guid;
        }
        BIND_END();

        BIND_START("idk.Bindings::ImageSetMaterialInstance", void, Handle<idk::Image> h, Guid guid)
        {
            h->material = RscHandle<MaterialInstance>{ guid };
        }
        BIND_END();

        BIND_START("idk.Bindings::ImageGetColor", color, Handle<idk::Image> h)
        {
            return h->tint;
        }
        BIND_END();

        BIND_START("idk.Bindings::ImageSetColor", void, Handle<idk::Image> h, color v)
        {
            h->tint = v;
        }
        BIND_END();


        // Text

        BIND_START("idk.Bindings::TextGetText", MonoString*, Handle<idk::Text> h)
        {
            return mono_string_new(mono_domain_get(), h->text.c_str());
        }
        BIND_END();

        BIND_START("idk.Bindings::TextSetText", void, Handle<idk::Text> h, MonoString* s)
        {
            h->text = unbox(s).get();
        }
        BIND_END();

        BIND_START("idk.Bindings::TextGetMaterialInstance", Guid, Handle<idk::Text> h)
        {
            return h->material.guid;
        }
        BIND_END();

        BIND_START("idk.Bindings::TextSetMaterialInstance", void, Handle<idk::Text> h, Guid guid)
        {
            h->material = RscHandle<MaterialInstance>{ guid };
        }
        BIND_END();

        BIND_START("idk.Bindings::TextGetColor", color, Handle<idk::Text> h)
        {
            return h->color;
        }
        BIND_END();

        BIND_START("idk.Bindings::TextSetColor", void, Handle<idk::Text> h, color v)
        {
            h->color = v;
        }
        BIND_END();

        BIND_START("idk.Bindings::TextGetFontSize", unsigned, Handle<idk::Text> h)
        {
            return h->font_size;
        }
        BIND_END();

        BIND_START("idk.Bindings::TextSetFontSize", void, Handle<idk::Text> h, unsigned v)
        {
            h->font_size = v;
        }
        BIND_END();
	}
}
