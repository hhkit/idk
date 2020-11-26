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

#include <network/EventManager.h>
#include <network/ConnectionManager.h>
#include <network/IDManager.h>

#include <core/Scheduler.h>
#include <core/Scheduler.inl>
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
#include <math/arith.inl>

#include <res/ResourceHandle.inl>

#include <meta/variant.inl>

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
		IDK_ASSERT_MSG(mb_itr->second.CacheThunk("UpdateCoroutines"), "could not cache UpdateCoroutines");

		auto ev_itr = _types.find("ElectronView");
		IDK_ASSERT_MSG(ev_itr != _types.end(), "cannot find idk.ElectronView");
		IDK_ASSERT_MSG(ev_itr->second.CacheThunk("Reserialize", 1), "could not cache Deserialize");

		auto en_itr = _types.find("ElectronNetwork");
		IDK_ASSERT_MSG(en_itr != _types.end(), "cannot find idk.ElectronView");
		IDK_ASSERT_MSG(en_itr->second.CacheThunk("Serialize", 1), "could not cache ElectronNetwork.Seserialize");
		IDK_ASSERT_MSG(en_itr->second.CacheThunk("Reserialize", 1), "could not cache ElectronNetwork.Deserialize");

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

	struct vec2ret 
	{
		float val[2];
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

		
		BIND_START("idk.Bindings::ObjectGetObjectsOfType", MonoArray*, MonoString* raw_type)
		{
			auto s = unbox(raw_type);
			string_view type_name = s.get();

			vector<Handle<mono::Behavior>> behaviors;
			for (auto& elem : Core::GetGameState().GetObjectsOfType<mono::Behavior>())
				if (auto type = elem.GetObject().Type())
					if (type && type->IsOrDerivedFrom(type_name))
						behaviors.emplace_back(elem.GetHandle());

			auto klass = Core::GetSystem<ScriptSystem>().ScriptEnvironment().Type(type_name);

			if (!klass)
				return nullptr;

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

		BIND_START("idk.Bindings::SceneGetActiveScene", Guid)
		{
			return Core::GetSystem<SceneManager>().GetActiveScene().guid;
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
				auto str = unbox(component).get();
				auto sv = string_view{ str };

				switch (string_hash(sv))
				{
					case string_hash("BoxCollider"):
					{
						auto col = go->AddComponent<Collider>();
						col->shape = idk::box{};
						return col.id;
					}
					case string_hash("SphereCollider"):
					{
						auto col = go->AddComponent<Collider>();
						col->shape = idk::sphere{};
						return col.id;
					}
					case string_hash("CapsuleCollider"):
					{
						auto col = go->AddComponent<Collider>();
						col->shape = idk::capsule{};
						return col.id;
					}
					default:
						return go->AddComponent(sv).id;
				}
			}
		BIND_END();

		BIND_START("idk.Bindings::GameObjectGetEngineComponent",  uint64_t, Handle<GameObject> go, MonoString* component) // note: return value optimization
			{
				auto query = unbox(component);
				auto str = string_view{ query.get() };
				switch (string_hash(str))
				{
				case string_hash( "Renderer"):
					{
						auto mrend = go->GetComponent<MeshRenderer>();
						return mrend ? mrend.id : go->GetComponent<SkinnedMeshRenderer>().id;
					}
					case string_hash("BoxCollider"):
					{
						for (auto& elem : go->GetComponents())
							if (elem.is_type<Collider>() && std::get_if<idk::box>(&handle_cast<Collider>(elem)->shape))
									return elem.id;
						return 0;
					}
					case string_hash("SphereCollider"):
					{
						for (auto& elem : go->GetComponents())
							if (elem.is_type<Collider>() && std::get_if<idk::sphere>(&handle_cast<Collider>(elem)->shape))
								return elem.id;
						return 0;
					}
					case string_hash("CapsuleCollider"):
					{
						for (auto& elem : go->GetComponents())
							if (elem.is_type<Collider>() && std::get_if<idk::capsule>(&handle_cast<Collider>(elem)->shape))
									return elem.id;
						return 0;
					}
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
					if (auto type = elem->GetObject().Type())
						if (type->IsOrDerivedFrom(findme))
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

		BIND_START("idk.Bindings::GameObjectGetLayer", int, Handle<GameObject> go)
		{
			return go->Layer();
		}
		BIND_END();

		BIND_START("idk.Bindings::GameObjectSetLayer", void, Handle<GameObject> go, int layer)
		{
			go->Layer(static_cast<decltype(idk::Layer::index)>(layer));
		}
		BIND_END();

        BIND_START("idk.Bindings::GameObjectFindWithTag", uint64_t, MonoString* tag)
        {
			const auto s = unbox(tag);
            auto ret = Core::GetSystem<TagManager>().Find(s.get());
            return ret.id;
        }
		BIND_END();

		BIND_START("idk.Bindings::GameObjectFindGameObjectsWithTag", MonoArray*, MonoString* tag)
		{
			const auto go_klass = Core::GetSystem<mono::ScriptSystem>().Environment().Type("GameObject");
			const auto s = unbox(tag);

			auto vec = Core::GetSystem<TagManager>().FindAll(s.get());
			auto retval = mono_array_new(mono_domain_get(), go_klass->Raw(), vec.size() ? vec.size() : 0);
			for (size_t i = 0; i < vec.size(); ++i)
			{
				auto mo = mono_object_new(mono_domain_get(), go_klass->Raw());
				auto method = mono_class_get_method_from_name(go_klass->Raw(), ".ctor", 1);
				void* args[] = { &vec[i].id };
				mono_runtime_invoke(method, mo, args, nullptr);
				mono_array_setref(retval, i, mo);
			}

			return retval;
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
				auto old_parent = h->parent;
				h->SetParent(parent_gameobject, preserve_global);
				Core::GetSystem<SceneManager>().ReparentObject(h->GetGameObject(), old_parent);
				if (const auto rt = h->GetGameObject()->GetComponent<RectTransform>())
					Core::GetSystem<UISystem>().RecalculateRects(rt);
			}
		BIND_END();

		BIND_START("idk.Bindings::TransformGetChildren", MonoArray*, Handle<Transform> h)
		{
			auto sg = Core::GetSystem<SceneManager>().FetchSceneGraphFor(h->GetGameObject());

			auto go_klass = Core::GetSystem<mono::ScriptSystem>().Environment().Type("GameObject");

			auto retval = mono_array_new(mono_domain_get(), go_klass->Raw(), sg ? sg.GetNumChildren() : 0);
			if (sg)
			{
				auto sz = sg.GetNumChildren();
				auto ptr = sg.begin();
				for (int i = 0; i < sz; ++i)
				{
					auto mo = mono_object_new(mono_domain_get(), go_klass->Raw());
					auto method = mono_class_get_method_from_name(go_klass->Raw(), ".ctor", 1);
					auto child_go = *ptr++;
					void* args[] = { &child_go.id };
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
				if (first.raycast_succ.distance_to_collision <= max_dist)
					return ManagedRaycast{
						true,
						first.collider.id,
						first.raycast_succ.distance_to_collision,
						first.raycast_succ.surface_normal,
						first.raycast_succ.point_of_collision };
			}

			return ManagedRaycast{ .valid = false };
		}
		BIND_END()

		BIND_START("idk.Bindings::PhysicsRaycastAll", MonoArray*, vec3 origin, vec3 direction, float max_dist, int mask, bool hit_triggers)
		{
			auto res = Core::GetSystem<PhysicsSystem>().Raycast(ray{ .origin = origin,.velocity = direction }, LayerMask{ mask }, hit_triggers);
			const auto end = std::remove_if(res.begin(), res.end(), [max_dist](const RaycastHit& hit) { return hit.raycast_succ.distance_to_collision > max_dist; });
			const auto sz = end - res.begin();
			
			const auto managed_raycast_type = Core::GetSystem<mono::ScriptSystem>().Environment().Type("ManagedRaycast");
			auto retval = mono_array_new(mono_domain_get(), managed_raycast_type->Raw(), sz);
			for (int i = 0; i < sz; ++i)
			{
				const ManagedRaycast managed_raycast{
					true,
					res[i].collider.id,
					res[i].raycast_succ.distance_to_collision,
					res[i].raycast_succ.surface_normal,
					res[i].raycast_succ.point_of_collision };
				mono_array_set(retval, ManagedRaycast, i, managed_raycast);
			}

			return retval;
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
			return rb->velocity();
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

		BIND_START("idk.Bindings::RigidBodyGetGravityScale", float, Handle < RigidBody> rb)
		{
			return rb->gravity_scale;
		}
		BIND_END();

		BIND_START("idk.Bindings::RigidBodySetGravityScale", void, Handle < RigidBody> rb, float gs)
		{
			rb->gravity_scale = gs;
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

		BIND_START("idk.Bindings::ColliderSetStaticFriction", void, Handle<Collider> col, float val)
		{
			col->static_friction = val;
		}
		BIND_END();

		BIND_START("idk.Bindings::ColliderGetStaticFriction", float, Handle<Collider> col)
		{
			return col->static_friction;
		}
		BIND_END();

		BIND_START("idk.Bindings::ColliderSetDynamicFriction", void, Handle<Collider> col, float val)
		{
			col->dynamic_friction = val;
		}
		BIND_END();

		BIND_START("idk.Bindings::ColliderGetDynamicFriction", float, Handle<Collider> col)
		{
			return col->dynamic_friction;
		}
		BIND_END();

		BIND_START("idk.Bindings::ColliderSetBounciness", void, Handle<Collider> col, float val)
		{
			col->bounciness = val;
		}
		BIND_END();

		BIND_START("idk.Bindings::ColliderGetBounciness", float, Handle<Collider> col)
		{
			return col->bounciness;
		}
		BIND_END();

		// BoxCollider

		BIND_START("idk.Bindings::ColliderBoxSetCenter", void, Handle<Collider> col, vec3 val)
		{
			std::get<idk::box>(col->shape).center = val;
		}
		BIND_END();

		BIND_START("idk.Bindings::ColliderBoxGetCenter", vec3, Handle<Collider> col)
		{
			return std::get<idk::box>(col->shape).center;
		}
		BIND_END();

		BIND_START("idk.Bindings::ColliderBoxSetSize", void, Handle<Collider> col, vec3 val)
		{
			std::get<idk::box>(col->shape).extents = val;
		}
		BIND_END();

		BIND_START("idk.Bindings::ColliderBoxGetSize", vec3, Handle<Collider> col)
		{
			return std::get<idk::box>(col->shape).extents;
		}
		BIND_END();

		// sphere collider
		BIND_START("idk.Bindings::ColliderSphereSetCenter", void, Handle<Collider> col, vec3 val)
		{
			std::get<idk::sphere>(col->shape).center = val;
		}
		BIND_END();

		BIND_START("idk.Bindings::ColliderSphereGetCenter", vec3, Handle<Collider> col)
		{
			return std::get<idk::sphere>(col->shape).center;
		}
		BIND_END();

		BIND_START("idk.Bindings::ColliderSphereSetRadius", void, Handle<Collider> col, float val)
		{
			std::get<idk::sphere>(col->shape).radius = val;
		}
		BIND_END();

		BIND_START("idk.Bindings::ColliderSphereGetRadius", float, Handle<Collider> col)
		{
			return std::get<idk::sphere>(col->shape).radius;
		}
		BIND_END();

		// capsule collider
		BIND_START("idk.Bindings::ColliderCapsuleSetCenter", void, Handle<Collider> col, vec3 val)
		{
			std::get<idk::capsule>(col->shape).center = val;
		}
		BIND_END();

		BIND_START("idk.Bindings::ColliderCapsuleGetCenter", vec3, Handle<Collider> col)
		{
			return std::get<idk::capsule>(col->shape).center;
		}
		BIND_END();

		BIND_START("idk.Bindings::ColliderCapsuleSetDirection", void, Handle<Collider> col, vec3 val)
		{
			std::get<idk::capsule>(col->shape).dir = val.get_normalized();
		}
		BIND_END();

		BIND_START("idk.Bindings::ColliderCapsuleGetDirection", vec3, Handle<Collider> col)
		{
			return std::get<idk::capsule>(col->shape).dir;
		}
		BIND_END();

		BIND_START("idk.Bindings::ColliderCapsuleSetRadius", void, Handle<Collider> col, float val)
		{
			std::get<idk::capsule>(col->shape).radius = val;
		}
		BIND_END();

		BIND_START("idk.Bindings::ColliderCapsuleGetRadius", float, Handle<Collider> col)
		{
			return std::get<idk::capsule>(col->shape).radius;
		}
		BIND_END();

		BIND_START("idk.Bindings::ColliderCapsuleSetHeight", void, Handle<Collider> col, float val)
		{
			std::get<idk::capsule>(col->shape).height = val;
		}
		BIND_END();

		BIND_START("idk.Bindings::ColliderCapsuleGetHeight", float, Handle<Collider> col)
		{
			return std::get<idk::capsule>(col->shape).height;
		}
		BIND_END();

		// Animator
		BIND_START("idk.Bindings::AnimatorPlay",  bool, Handle<Animator> animator, MonoString* name, MonoString* layer = nullptr)
		{
			auto s = unbox(name);
			auto l = unbox(layer);
			int index = l.get()[0] ? s_cast<int>(animator->FindLayerIndex(l.get())) : 0;

			return animator->Play(s.get(), 0.0f, index);
		}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorBlendTo", bool, Handle<Animator> animator, MonoString* name, float time = 0.2f, MonoString* layer = nullptr)
		{
			auto s = unbox(name);
			auto l = unbox(layer);
			int index = l.get()[0] ? s_cast<int>(animator->FindLayerIndex(l.get())) : 0;
			
			return animator->BlendTo(s.get(), time, index);
		}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorBlendToInSeconds", bool, Handle<Animator> animator, MonoString* name, float timeInSeconds = 0.0f, MonoString* layer = nullptr)
		{
			auto s = unbox(name);
			auto l = unbox(layer);
			int index = l.get()[0] ? s_cast<int>(animator->FindLayerIndex(l.get())) : 0;
			auto state = animator->GetState(s.get(), index);
			float time = 0.0f;
			if (state.valid)
				time = timeInSeconds / state.duration;
			return animator->BlendTo(s.get(), time, index);
		}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorPause", bool, Handle<Animator> animator, MonoString* layer = nullptr)
		{
			auto l = unbox(layer);
			int index = l.get()[0] ? s_cast<int>(animator->FindLayerIndex(l.get())) : 0;

			return animator->Pause(index);
		}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorResume", bool, Handle<Animator> animator, MonoString* layer = nullptr)
		{
			auto l = unbox(layer);
			int index = l.get()[0] ? s_cast<int>(animator->FindLayerIndex(l.get())) : 0;

			return animator->Resume(index);
		}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorStop",  bool, Handle<Animator> animator, MonoString* layer = nullptr)
		{
			auto l = unbox(layer);
			int index = l.get()[0] ? s_cast<int>(animator->FindLayerIndex(l.get())) : 0;

			return animator->Stop(index);
		}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorGetState", CSharpState, Handle<Animator> animator, MonoString* name, MonoString* layer = nullptr)
		{
			auto s = unbox(name);
			auto l = unbox(layer);
			int index = l.get()[0] ? s_cast<int>(animator->FindLayerIndex(l.get())) : 0;

			return animator->GetState(s.get(), index);
		}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorDefaultStateName", MonoString*, Handle<Animator> animator, MonoString* layer = nullptr)
		{
			auto l = unbox(layer);
			int index = l.get()[0] ? s_cast<int>(animator->FindLayerIndex(l.get())) : 0;

			return mono_string_new(mono_domain_get(), animator->DefaultStateName(index).c_str());
		}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorCurrentStateName", MonoString*, Handle<Animator> animator, MonoString* layer = nullptr)
		{
			auto l = unbox(layer);
			int index = l.get()[0] ? s_cast<int>(animator->FindLayerIndex(l.get())) : 0;

			return mono_string_new(mono_domain_get(), animator->CurrentStateName(index).c_str());
		}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorCurrentStateTime", float, Handle<Animator> animator, MonoString* layer = nullptr)
		{
			auto l = unbox(layer);
			int index = l.get()[0] ? s_cast<int>(animator->FindLayerIndex(l.get())) : 0;

			return animator->CurrentStateTime(index);
		}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorBlendStateName", MonoString*, Handle<Animator> animator, MonoString* layer = nullptr)
		{
			auto l = unbox(layer);
			int index = l.get()[0] ? s_cast<int>(animator->FindLayerIndex(l.get())) : 0;

			return mono_string_new(mono_domain_get(), animator->BlendStateName(index).c_str());
		}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorIsPlaying",  bool, Handle<Animator> animator, MonoString* layer = nullptr)
		{
			auto l = unbox(layer);
			int index = l.get()[0] ? s_cast<int>(animator->FindLayerIndex(l.get())) : 0;

			return animator->IsPlaying(index);
		}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorIsBlending",  bool, Handle<Animator> animator, MonoString* layer = nullptr)
		{
			auto l = unbox(layer);
			int index = l.get()[0] ? s_cast<int>(animator->FindLayerIndex(l.get())) : 0;

			return animator->IsBlending(index);
		}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorHasCurrAnimEnded",  bool, Handle<Animator> animator, MonoString* layer = nullptr)
		{
			auto l = unbox(layer);
			int index = l.get()[0] ? s_cast<int>(animator->FindLayerIndex(l.get())) : 0;

			return animator->HasCurrAnimEnded(index);
		}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorHasState",  bool, Handle<Animator> animator, MonoString* name, MonoString* layer = nullptr)
		{
			auto s = unbox(name);
			auto l = unbox(layer);
			int index = l.get()[0] ? s_cast<int>(animator->FindLayerIndex(l.get())) : 0;

			auto ret_val = animator->HasState(s.get(), index);
			return ret_val;
		}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorGetWeight", float, Handle<Animator> animator, MonoString* layer = nullptr)
		{
			auto l = unbox(layer);
			int index = l.get()[0] ? s_cast<int>(animator->FindLayerIndex(l.get())) : 0;

			return animator->GetWeight(index);
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

		BIND_START("idk.Bindings::AnimatorSetWeight", bool, Handle<Animator> animator, float weight, MonoString* layer = nullptr)
		{
			auto l = unbox(layer);
			int index = l.get()[0] ? s_cast<int>(animator->FindLayerIndex(l.get())) : 0;

			return animator->SetWeight(weight, index);
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

		BIND_START("idk.Bindings::AnimatorGetOffset", vec3, Handle<Animator> animator)
		{
			return animator->offset;
		}
		BIND_END();

		BIND_START("idk.Bindings::AnimatorSetOffset", void, Handle<Animator> animator, vec3 offset)
		{
			animator->offset = offset;
		}
		BIND_END();

		//AudioListener
		//----------------------------------------------------------------------------------------------------
		BIND_START("idk.Bindings::AudioListenerGetEnabledState", bool, Handle<AudioListener> audiolistener)
		{
			return audiolistener->is_active;
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioListenerSetEnabledState", void, Handle<AudioListener> audiolistener, bool val)
		{
			audiolistener->is_active = val;
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
		//AudioSystem
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		BIND_START("idk.Bindings::AudioSystemSetMASTERVolume", void, float newVolume)
		{
			newVolume = newVolume > 1 ? 1 : (newVolume < 0 ? 0 : newVolume); //Clamp
			Core::GetSystem<AudioSystem>().SetChannel_MASTER_Volume(newVolume);
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSystemGetMASTERVolume", float)
		{
			return Core::GetSystem<AudioSystem>().GetChannel_MASTER_Volume();
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSystemSetSFXVolume", void, float newVolume)
		{
			newVolume = newVolume > 1 ? 1 : (newVolume < 0 ? 0 : newVolume); //Clamp
			Core::GetSystem<AudioSystem>().SetChannel_SFX_Volume(newVolume);
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSystemGetSFXVolume", float)
		{
			return Core::GetSystem<AudioSystem>().GetChannel_SFX_Volume();
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSystemSetMUSICVolume", void, float newVolume)
		{
			newVolume = newVolume > 1 ? 1 : (newVolume < 0 ? 0 : newVolume); //Clamp
			Core::GetSystem<AudioSystem>().SetChannel_MUSIC_Volume(newVolume);
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSystemGetMUSICVolume", float)
		{
			return Core::GetSystem<AudioSystem>().GetChannel_MUSIC_Volume();
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSystemSetAMBIENTVolume", void, float newVolume)
		{
			newVolume = newVolume > 1 ? 1 : (newVolume < 0 ? 0 : newVolume); //Clamp
			Core::GetSystem<AudioSystem>().SetChannel_AMBIENT_Volume(newVolume);
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSystemGetAMBIENTVolume", float)
		{
			return Core::GetSystem<AudioSystem>().GetChannel_AMBIENT_Volume();
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSystemSetDIALOGUEVolume", void, float newVolume)
		{
			newVolume = newVolume > 1 ? 1 : (newVolume < 0 ? 0 : newVolume); //Clamp
			Core::GetSystem<AudioSystem>().SetChannel_DIALOGUE_Volume(newVolume);
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSystemGetDIALOGUEVolume", float)
		{
			return Core::GetSystem<AudioSystem>().GetChannel_DIALOGUE_Volume();
		}
		BIND_END();
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		BIND_START("idk.Bindings::AudioSystemSetMASTERPause", void, bool newState)
		{
			Core::GetSystem<AudioSystem>().SetChannel_MASTER_Pause(newState);
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSystemGetMASTERPause", bool)
		{
			return Core::GetSystem<AudioSystem>().GetChannel_MASTER_Pause();
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSystemSetSFXPause", void, bool newState)
		{
			Core::GetSystem<AudioSystem>().SetChannel_SFX_Pause(newState);
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSystemGetSFXPause", bool)
		{
			return Core::GetSystem<AudioSystem>().GetChannel_SFX_Pause();
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSystemSetMUSICPause", void, bool newState)
		{
			Core::GetSystem<AudioSystem>().SetChannel_MUSIC_Pause(newState);
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSystemGetMUSICPause", bool)
		{
			return Core::GetSystem<AudioSystem>().GetChannel_MUSIC_Pause();
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSystemSetAMBIENTPause", void, bool newState)
		{
			Core::GetSystem<AudioSystem>().SetChannel_AMBIENT_Pause(newState);
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSystemGetAMBIENTPause", bool)
		{
			return Core::GetSystem<AudioSystem>().GetChannel_AMBIENT_Pause();
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSystemSetDIALOGUEPause", void, bool newState)
		{
			Core::GetSystem<AudioSystem>().SetChannel_DIALOGUE_Pause(newState);
		}
		BIND_END();

		BIND_START("idk.Bindings::AudioSystemGetDIALOGUEPause", bool)
		{
			return Core::GetSystem<AudioSystem>().GetChannel_DIALOGUE_Pause();
		}
		BIND_END();

		///////////////////////////////////////////////////////////////////////////////////////////////////////



		BIND_START("idk.Bindings::AudioSystemStopAll", void)
		{
			Core::GetSystem<AudioSystem>().StopAllAudio();
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

            switch (hash)
            {
				VALIDATE_RESOURCE(Texture);
				VALIDATE_RESOURCE(Material);
				VALIDATE_RESOURCE(MaterialInstance);
				VALIDATE_RESOURCE(Prefab);
				VALIDATE_RESOURCE(Scene);
            default: return false;
            }
        }
		BIND_END();
#undef VALIDATE_RESOURCE

#define NAME_OF_RESOURCE(RES) case string_hash(#RES): { \
	auto rid = RscHandle<RES>(guid); \
	auto path = Core::GetResourceManager().GetPath(RscHandle<RES>(guid)); \
	auto name = RscHandle<RES>(guid) ? Core::GetResourceManager().Get<RES>(guid).Name() : string_view{""}; \
	return mono_string_new(mono_domain_get(), rid ? (name.size() ? name.data() : PathHandle{ *path }.GetStem().data()) : ""); }
        BIND_START("idk.Bindings::ResourceGetName",  MonoString*, Guid guid, MonoString* type)
        {
            // TODO: make get jumptable...
			// TODO: research on perfect jumping
            auto s = unbox(type);
            auto hash = string_hash(s.get());

			LOG_TO(LogPool::MONO, "GET RESOURCE %s: %s", s.get(), string{ guid }.c_str());

            switch (hash)
            {
				NAME_OF_RESOURCE(Texture);
				NAME_OF_RESOURCE(Material);
				NAME_OF_RESOURCE(MaterialInstance);
			//case string_hash("MaterialInstance"): 
			//{ 
			//	auto path = Core::GetResourceManager().GetPath(RscHandle<MaterialInstance>(guid)); 
			//	auto name = Core::GetResourceManager().Get<MaterialInstance>(guid).Name(); 
			//	return mono_string_new(mono_domain_get(), name.size() ? name.data() : PathHandle{ *path }.GetStem().data()); 
			//}
				NAME_OF_RESOURCE(Prefab);
				NAME_OF_RESOURCE(Scene);
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
			return res ? std::get<idk::vec2>(*res) : idk::vec2(0, 0);
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

		// Texture
		BIND_START("idk.Bindings::TextureGetSize", vec2ret, RscHandle<Texture> handle)
		{
			if (!handle) { return vec2ret{ 0, 0 }; }
			uvec2 ret = handle->Size();
			return vec2ret{ static_cast<float>(ret.x), static_cast<float>(ret.y) };
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

		// Graphics
		BIND_START("idk.Bindings::GraphicsGetSRGB", bool)
		{
			return RscHandle<RenderTarget>{}->Srgb();
		}
		BIND_END();

		BIND_START("idk.Bindings::GraphicsVarBoolIsSet", bool, MonoString* str)
		{
			string name = unbox(str).get();
			auto& vars = Core::GetSystem<GraphicsSystem>().extra_vars.extra_vars;
			auto itr = vars.find(name);
			return itr != vars.end() && itr->second.index() == index_in_variant_v<bool, decltype(itr->second)>;
		}
		BIND_END();
		BIND_START("idk.Bindings::GraphicsGetVarBool", bool, MonoString* str)
		{
			string name = unbox(str).get();
			auto& vars = Core::GetSystem<GraphicsSystem>().extra_vars.extra_vars;
			auto itr = vars.find(name);
			return itr != vars.end() && itr->second.index() == index_in_variant_v<bool, decltype(itr->second)>&& std::get<bool>(itr->second);
		}
		BIND_END();
		BIND_START("idk.Bindings::GraphicsSetVarBool", void, MonoString* str, bool b)
		{
			string name = unbox(str).get();
			Core::GetSystem<GraphicsSystem>().extra_vars.Set(name,b);
		}
		BIND_END();

		BIND_START("idk.Bindings::GraphicsVarIntIsSet", bool, MonoString* str)
		{
			string name = unbox(str).get();
			auto& vars = Core::GetSystem<GraphicsSystem>().extra_vars.extra_vars;
			auto itr = vars.find(name);
			return itr != vars.end() && itr->second.index() == index_in_variant_v<int, decltype(itr->second)>;
		}
		BIND_END();
		BIND_START("idk.Bindings::GraphicsGetVarInt", int, MonoString* str)
		{
			string name = unbox(str).get();
			auto& vars = Core::GetSystem<GraphicsSystem>().extra_vars.extra_vars;
			auto itr = vars.find(name);
			return (itr != vars.end() && itr->second.index() == index_in_variant_v<int, decltype(itr->second)>) ? std::get<int>(itr->second) : -1;
		}
		BIND_END();
		BIND_START("idk.Bindings::GraphicsSetVarInt", void, MonoString* str, int b)
		{
			string name = unbox(str).get();
			Core::GetSystem<GraphicsSystem>().extra_vars.Set(name, b);
		}
		BIND_END();
		//BIND_START("idk.Bindings::GraphicsGetGammaCorrection", float)
		//{
		//	return Core::GetSystem<GraphicsSystem>().extra_vars.Get;
		//}
		//BIND_END();

		BIND_START("idk.Bindings::GraphicsSetGammaCorrection", void, float gamma_correction)
		{
			Core::GetSystem<GraphicsSystem>().extra_vars.Set("gamma_correction", gamma_correction);
		}
		BIND_END();
		BIND_START("idk.Bindings::GraphicsGetGammaCorrection", float)
		{
			auto gamma_correction = Core::GetSystem<GraphicsSystem>().extra_vars.Get<float>("gamma_correction");
			return gamma_correction ? *gamma_correction : 1.0f;
		}
		BIND_END();
		BIND_START("idk.Bindings::GraphicsDisableGammaCorrection", void)
		{
			Core::GetSystem<GraphicsSystem>().extra_vars.Unset("gamma_correction");
		}
		BIND_END();

		// //////Camera///////////////
		BIND_START("idk.Bindings::CameraGetFOV", real, Handle<Camera> h)
		{
			return deg(h->field_of_view).value;
		}
		BIND_END();

		BIND_START("idk.Bindings::CameraSetFOV", void, Handle<Camera> h, float r)
		{
			h->field_of_view = deg(r);
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

		BIND_START("idk.Bindings::CameraGetCullingMask", int, Handle<Camera> h)
		{
			return reinterpret_cast<int&>(h->layer_mask);
		}
		BIND_END();

		BIND_START("idk.Bindings::CameraSetCullingMask", void, Handle<Camera> h, int mask)
		{
			h->layer_mask = LayerMask(mask);
		}
		BIND_END();


		BIND_START("idk.Bindings::CameraGetUseFog", bool, Handle<Camera> h)
		{
			return h->ppe.useFog;
		}
		BIND_END();

		BIND_START("idk.Bindings::CameraSetUseFog", void, Handle<Camera> h, bool r)
		{
			h->ppe.useFog = r;
		}
		BIND_END();

		BIND_START("idk.Bindings::CameraGetUseBloom", bool, Handle<Camera> h)
		{
			return h->ppe.useFog;
		}
		BIND_END();

		BIND_START("idk.Bindings::CameraSetUseBloom", void, Handle<Camera> h, bool r)
		{
			h->ppe.useBloom = r;
		}
		BIND_END();

		BIND_START("idk.Bindings::CameraGetFogColor", color, Handle<Camera> h)
		{
			return h->ppe.fogColor;
		}
		BIND_END();

		BIND_START("idk.Bindings::CameraSetFogColor", void, Handle<Camera> h, color r)
		{
			h->ppe.fogColor = r;
		}
		BIND_END();

		BIND_START("idk.Bindings::CameraGetFogDensity", float, Handle<Camera> h)
		{
			return h->ppe.fogDensity;
		}
		BIND_END();

		BIND_START("idk.Bindings::CameraSetFogDensity", void, Handle<Camera> h, float r)
		{
			h->ppe.fogDensity = r;
		}
		BIND_END();

		BIND_START("idk.Bindings::CameraGetThreshold", vec3, Handle<Camera> h)
		{
			return h->ppe.threshold;
		}
		BIND_END();

		BIND_START("idk.Bindings::CameraSetThreshold", void, Handle<Camera> h, vec3 r)
		{
			h->ppe.threshold = r;
		}
		BIND_END();

		BIND_START("idk.Bindings::CameraGetBlurStrength", float, Handle<Camera> h)
		{
			return h->ppe.blurStrength;
		}
		BIND_END();

		BIND_START("idk.Bindings::CameraSetBlurStrength", void, Handle<Camera> h, float r)
		{
			h->ppe.blurStrength = r;
		}
		BIND_END();

		BIND_START("idk.Bindings::CameraGetBlurScale", float, Handle<Camera> h)
		{
			return h->ppe.blurScale;
		}
		BIND_END();

		BIND_START("idk.Bindings::CameraSetBlurScale", void, Handle<Camera> h, float r)
		{
			h->ppe.blurScale = r;
		}
		BIND_END();

		BIND_START("idk.Bindings::CameraGetDepth", int, Handle<Camera> h)
		{
			return h->depth;
		}
		BIND_END();

		BIND_START("idk.Bindings::CameraSetDepth", void, Handle<Camera> h, int d)
		{
			h->depth = d;
		}
		BIND_END();

		// //////lights////////////////
		BIND_START("idk.Bindings::LightGetEnabled", bool, Handle<Light> h)
		{
			return h->enabled;
		}
		BIND_END();

		BIND_START("idk.Bindings::LightSetEnabled", void, Handle<Light> h, bool value)
		{
			h->enabled = value;
		}
		BIND_END();

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

		BIND_START("idk.Bindings::LightGetInnerSpotAngle", real, Handle<Light> h)
		{
			return std::visit([&](auto& light_variant)-> const real
			{
				using T = std::decay_t<decltype(light_variant)>;
				if constexpr (std::is_same_v<T, SpotLight>)
					return light_variant.inner_angle.value;
				else
					return 0.f;
			}, h->light);
		}
		BIND_END();

		BIND_START("idk.Bindings::LightSetInnerSpotAngle", void, Handle<Light> h, real i)
		{
			std::visit([&, val = i](auto& light_variant)
			{
				using T = std::decay_t<decltype(light_variant)>;
				if constexpr (std::is_same_v<T, SpotLight>)
					light_variant.inner_angle.value = val;
			}, h->light);
		}
		BIND_END();

		BIND_START("idk.Bindings::LightGetOuterSpotAngle", real, Handle<Light> h)
		{
			return std::visit([&](auto& light_variant)-> const real
			{
				using T = std::decay_t<decltype(light_variant)>;
				if constexpr (std::is_same_v<T, SpotLight>)
					return light_variant.outer_angle.value;
				else
					return 0.f;
			}, h->light);
		}
		BIND_END();

		BIND_START("idk.Bindings::LightSetOuterSpotAngle", void, Handle<Light> h, real i)
		{
			std::visit([&, val = i](auto& light_variant)
			{
				using T = std::decay_t<decltype(light_variant)>;
				if constexpr (std::is_same_v<T, SpotLight>)
					light_variant.outer_angle.value = val;
			}, h->light);
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

		BIND_START("idk.Bindings::InputGetAnyKeyDown", bool)
		{
			auto& app = Core::GetSystem<Application>();
			for (unsigned char key = s_cast<unsigned char>(idk::Key::LButton); key <= s_cast<unsigned char>(idk::Key::AltRight); ++key)
			{
				if (app.GetKeyDown(s_cast<idk::Key>(key)))
					return true;
			}

			constexpr auto check_gamepad_buttons = [](char player)
			{
				const auto& gamepad = Core::GetSystem<GamepadSystem>();
				for (int bit = 0; bit < 16; ++bit)
				{
					if (gamepad.GetButtonDown(player, s_cast<idk::GamepadButton>(1 << bit)))
						return true;
				}
				return false;
			};
			const auto connected = Core::GetSystem<GamepadSystem>().GetConnectedPlayers();

			if ((connected & 1) && check_gamepad_buttons(0))
				return true;
			if ((connected & 2) && check_gamepad_buttons(1))
				return true;
			if ((connected & 4) && check_gamepad_buttons(2))
				return true;
			if ((connected & 8) && check_gamepad_buttons(3))
				return true;

			return false;
		}
		BIND_END();

		BIND_START("idk.Bindings::InputGetAnyKey", bool)
		{
			auto& app = Core::GetSystem<Application>();
			for (unsigned char key = s_cast<unsigned char>(idk::Key::LButton); key <= s_cast<unsigned char>(idk::Key::AltRight); ++key)
			{
				if (app.GetKey(s_cast<idk::Key>(key)))
					return true;
			}

			constexpr auto check_gamepad_buttons = [](char player)
			{
				const auto& gamepad = Core::GetSystem<GamepadSystem>();
				for (int bit = 0; bit < 16; ++bit)
				{
					if (gamepad.GetButton(player, s_cast<idk::GamepadButton>(1 << bit)))
						return true;
				}
				return false;
			};
			const auto connected = Core::GetSystem<GamepadSystem>().GetConnectedPlayers();

			if ((connected & 1) && check_gamepad_buttons(0))
				return true;
			if ((connected & 2) && check_gamepad_buttons(1))
				return true;
			if ((connected & 4) && check_gamepad_buttons(2))
				return true;
			if ((connected & 8) && check_gamepad_buttons(3))
				return true;

			return false;
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

		BIND_START("idk.Bindings::TimeGetAccumTime", float)
		{
			return Core::GetScheduler().GetRemainingTime().count();
		}
		BIND_END();

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
				return Core::GetScheduler().GetRealDeltaTime().count();
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

		BIND_START("idk.Bindings::TimeGetTimeSinceStart", float)
		{
			return Core::GetScheduler().GetTimeSinceStart().count();
		}
		BIND_END();



        // RectTransform

        BIND_START("idk.Bindings::RectTransformGetOffsetMin", vec2ret, Handle<RectTransform> h)
        {
			return vec2ret{ h->offset_min.x, h->offset_min.y };
        }
        BIND_END();

        BIND_START("idk.Bindings::RectTransformSetOffsetMin", void, Handle<RectTransform> h, vec2 v)
        {
            h->offset_min = v;
			Core::GetSystem<UISystem>().RecalculateRects(h);
        }
        BIND_END();

        BIND_START("idk.Bindings::RectTransformGetOffsetMax", vec2ret, Handle<RectTransform> h)
        {
			return vec2ret{ h->offset_max.x, h->offset_max.y };
        }
        BIND_END();

        BIND_START("idk.Bindings::RectTransformSetOffsetMax", void, Handle<RectTransform> h, vec2 v)
        {
            h->offset_max = v;
			Core::GetSystem<UISystem>().RecalculateRects(h);
        }
        BIND_END();

        BIND_START("idk.Bindings::RectTransformGetAnchorMin", vec2ret, Handle<RectTransform> h)
        {
			return vec2ret{ h->anchor_min.x, h->anchor_min.y };
        }
        BIND_END();

        BIND_START("idk.Bindings::RectTransformSetAnchorMin", void, Handle<RectTransform> h, vec2 v)
        {
            h->anchor_min = v;
			Core::GetSystem<UISystem>().RecalculateRects(h);
        }
        BIND_END();

        BIND_START("idk.Bindings::RectTransformGetAnchorMax", vec2ret, Handle<RectTransform> h)
        {
			return vec2ret{ h->anchor_max.x, h->anchor_max.y };
        }
        BIND_END();

        BIND_START("idk.Bindings::RectTransformSetAnchorMax", void, Handle<RectTransform> h, vec2 v)
        {
            h->anchor_max = v;
			Core::GetSystem<UISystem>().RecalculateRects(h);
        }
        BIND_END();

        BIND_START("idk.Bindings::RectTransformGetPivot", vec2ret, Handle<RectTransform> h)
        {
			return vec2ret{ h->pivot.x, h->pivot.y };
        }
        BIND_END();

        BIND_START("idk.Bindings::RectTransformSetPivot", void, Handle<RectTransform> h, vec2 v)
        {
            h->pivot = v;
			Core::GetSystem<UISystem>().RecalculateRects(h);
        }
        BIND_END();

		BIND_START("idk.Bindings::RectTransformGetRect", rect, Handle<RectTransform> h)
		{
			return h->_local_rect;
		}
		BIND_END();

		BIND_START("idk.Bindings::RectTransformGetAnchoredPosition", vec2ret, Handle<RectTransform> h)
		{
			vec2 anchored_pos = lerp(h->offset_min, h->offset_max, h->pivot);
			return vec2ret{ anchored_pos.x, anchored_pos.y };
		}
		BIND_END();

		BIND_START("idk.Bindings::RectTransformSetAnchoredPosition", void, Handle<RectTransform> h, vec2 v)
		{
			vec2 d = v - lerp(h->offset_min, h->offset_max, h->pivot);
			h->offset_min += d;
			h->offset_max += d;
			Core::GetSystem<UISystem>().RecalculateRects(h);
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


		// LayerMask

		BIND_START("idk.Bindings::LayerMaskLayerToName", MonoString*, int index)
		{
			return mono_string_new(mono_domain_get(), Core::GetSystem<LayerManager>().LayerIndexToName(static_cast<decltype(idk::Layer::index)>(index)).data());
		}
		BIND_END();

		BIND_START("idk.Bindings::LayerMaskNameToLayer", int, MonoString* s)
		{
			return Core::GetSystem<LayerManager>().NameToLayerIndex(unbox(s).get());
		}
		BIND_END();

		// Networking

		BIND_START("idk.Bindings::NetworkGetIsHost", bool)
		{
			return Core::GetSystem<NetworkSystem>().IsHost();
		}
		BIND_END();

		BIND_START("idk.Bindings::NetworkGetClients", MonoArray*)
		{
			vector<int> clients;
			auto& network = Core::GetSystem<NetworkSystem>();
			for (auto host = s_cast<int>(Host::CLIENT0); host < s_cast<int>(Host::CLIENT_MAX); ++host)
				if (network.GetConnectionTo(s_cast<Host>(host))) // has connection
					clients.emplace_back(host);

			auto retval = mono_array_new(mono_domain_get(), mono_get_int32_class(), clients.size());
			for (int i = 0; i < clients.size(); ++i)
				mono_array_set(retval, int, i, clients[i]);

			return retval;
		}
		BIND_END();

		BIND_START("idk.Bindings::NetworkGetIsConnected", bool)
		{
			return Core::GetSystem<NetworkSystem>().GetConnectionTo();
		}
		BIND_END();

		BIND_START("idk.Bindings::NetworkEvictClient", void, int id)
		{
			Core::GetSystem<NetworkSystem>().EvictClient(id);
		}
		BIND_END();

		BIND_START("idk.Bindings::NetworkDisconnect", void)
		{
			Core::GetSystem<NetworkSystem>().Disconnect();
		}
		BIND_END();

		BIND_START("idk.Bindings::NetworkCreateLobby", void, int lobby_type)
		{
			Core::GetSystem<NetworkSystem>().CreateLobby(static_cast<ELobbyType>(lobby_type));
		}
		BIND_END();

		BIND_START("idk.Bindings::NetworkJoinLobby", void, CSteamID lobby_id)
		{
			Core::GetSystem<NetworkSystem>().JoinLobby(lobby_id);
		}
		BIND_END();

		BIND_START("idk.Bindings::NetworkLeaveLobby", void)
		{
			Core::GetSystem<NetworkSystem>().LeaveLobby();
		}
		BIND_END();

		BIND_START("idk.Bindings::NetworkGetCurrentLobby", uint64_t)
		{
			return Core::GetSystem<NetworkSystem>().GetLobbyID().ConvertToUint64();
		}
		BIND_END();

		BIND_START("idk.Bindings::NetworkGetLocalClient", int)
		{
			return static_cast<int>(Core::GetSystem<NetworkSystem>().GetMe());
		}
		BIND_END();

		BIND_START("idk.Bindings::NetworkOpenLobbyInviteDialog", void)
		{
			SteamFriends()->ActivateGameOverlayInviteDialog(Core::GetSystem<NetworkSystem>().GetLobbyID());
		}
		BIND_END();

		BIND_START("idk.Bindings::NetworkFindLobbies", void)
		{
			Core::GetSystem<NetworkSystem>().FindLobbies();
		}
		BIND_END();

		BIND_START("idk.Bindings::NetworkGetLobbyData", MonoString*, CSteamID lobby_id, MonoString* key)
		{
			return mono_string_new(mono_domain_get(), SteamMatchmaking()->GetLobbyData(lobby_id, unbox(key).get()));
		}
		BIND_END();

		BIND_START("idk.Bindings::NetworkSetLobbyData", void, MonoString* key, MonoString* value)
		{
			SteamMatchmaking()->SetLobbyData(Core::GetSystem<NetworkSystem>().GetLobbyID(), unbox(key).get(), unbox(value).get());
		}
		BIND_END();

		BIND_START("idk.Bindings::NetworkSendLobbyMsg", void, MonoArray* msg)
		{
			int sz = static_cast<int>(mono_array_length(msg));
			auto* bytes = mono_array_addr(msg, std::byte, 0);
			SteamMatchmaking()->SendLobbyChatMsg(Core::GetSystem<NetworkSystem>().GetLobbyID(), bytes, sz);
		}
		BIND_END();

		BIND_START("idk.Bindings::NetworkGetLobbyNumMembers", int, CSteamID lobby_id)
		{
			return SteamMatchmaking()->GetNumLobbyMembers(lobby_id);
		}
		BIND_END();

		BIND_START("idk.Bindings::NetworkClientLobbyIndex", int, int id)
		{
			return Core::GetSystem<NetworkSystem>().GetLobbyMemberIndex(static_cast<Host>(id));
		}
		BIND_END();

		BIND_START("idk.Bindings::NetworkClientName", MonoString*, int id)
		{
			auto steamid = Core::GetSystem<NetworkSystem>().GetLobbyMember(static_cast<Host>(id));
			return mono_string_new(mono_domain_get(), SteamFriends()->GetFriendPersonaName(steamid));
		}
		BIND_END();

		BIND_START("idk.Bindings::NetworkConnectToLobbyOwner", void)
		{
			Core::GetSystem<NetworkSystem>().ConnectToLobbyOwner();
		}
		BIND_END();

		BIND_START("idk.Bindings::NetworkLoadScene", void, Guid g)
		{
			RscHandle<Scene> scene{ g };
			EventManager::BroadcastLoadLevel(scene);
		}
		BIND_END();

		BIND_START("idk.Bindings::NetworkInstantiatePrefabPosition", uint64_t, Guid g, vec3 pos)
		{
			return EventManager::BroadcastInstantiatePrefab(RscHandle<Prefab>{g}, pos).id;
		}
		BIND_END();

		BIND_START("idk.Bindings::NetworkInstantiatePrefabPositionRotation", uint64_t, Guid g, vec3 pos, quat rot)
		{
			return EventManager::BroadcastInstantiatePrefab(RscHandle<Prefab>{g}, pos, idk::quat{ rot }).id;
		}
		BIND_END();

		BIND_START("idk.Bindings::NetworkAddCallback", void, uint64_t g)
		{
			Core::GetSystem<NetworkSystem>().AddCallbackTarget(Handle<mono::Behavior>{g});
		}
		BIND_END();

		BIND_START("idk.Bindings::NetworkRemoveCallback", void, uint64_t g)
		{
			Core::GetSystem<NetworkSystem>().RemoveCallbackTarget(Handle<mono::Behavior>{g});
		}
		BIND_END();

		BIND_START("idk.Bindings::ViewGetNetworkId", NetworkID, Handle<ElectronView> ev)
		{
			return ev->network_id;
		}
		BIND_END();

		BIND_START("idk.Bindings::ViewIdGetView", uint64_t, NetworkID id)
		{
			return Core::GetSystem<NetworkSystem>().GetIDManager().GetViewFromId(id).id;
		}
		BIND_END();

		BIND_START("idk.Bindings::ViewDestroy", void, Handle<ElectronView> network_id)
		{
			EventManager::BroadcastDestroyView(network_id);
		}
		BIND_END();

		BIND_START("idk.Bindings::ViewIsMine", bool, Handle<ElectronView> ev)
		{
			return ev->IsMine();
		}
		BIND_END();

		BIND_START("idk.Bindings::ViewTransferOwnership", void, Handle<ElectronView> ev, int newOwner)
		{
			EventManager::SendTransferOwnership(ev, static_cast<Host>(newOwner));
		}
		BIND_END();

		BIND_START("idk.Bindings::ViewExecRPC", void, Handle<ElectronView> ev, MonoString* method_name, int rpc_target, MonoArray* params)
		{
			auto length = mono_array_length(params);
			vector<vector<unsigned char>> param_vec;
			for (int i = 0; i < length; ++i)
			{
				auto subarr = mono_array_get(params, MonoArray*, i);
				auto subarr_len = mono_array_length(subarr);
				auto& buffer = param_vec.emplace_back();

				for (int j = 0; j < subarr_len; ++j)
					buffer.push_back(mono_array_get(subarr, unsigned char, j));
			}

			EventManager::BroadcastRPC(ev, static_cast<RPCTarget>(rpc_target), unbox(method_name).get(), param_vec);
		}
		BIND_END();

		BIND_START("idk.Bindings::ViewExecRPCOnPlayer", void, Handle<ElectronView> ev, MonoString* method_name, int player_target, MonoArray* params)
		{
			auto connection = Core::GetSystem<NetworkSystem>().GetConnectionTo((Host)player_target);
			if (!connection)
			{
				LOG_TO(LogPool::NETWORK, "Tried to execute RPC on disconnected player");
				return;
			}

			auto* event_manager = connection->GetManager<EventManager>();

			auto length = mono_array_length(params);
			vector<vector<unsigned char>> param_vec;
			for (int i = 0; i < length; ++i)
			{
				auto subarr = mono_array_get(params, MonoArray*, i);
				auto subarr_len = mono_array_length(subarr);
				auto& buffer = param_vec.emplace_back();

				for (int j = 0; j < subarr_len; ++j)
					buffer.push_back(mono_array_get(subarr, unsigned char, j));
			}

			event_manager->SendRPC(ev, unbox(method_name).get(), param_vec);
		}
		BIND_END();

	}
}
