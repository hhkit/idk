#pragma once
#include <idk.h>
#include <core/Component.h>
#include <network/network.h>
#include <network/GhostPack.h>
#include <network/MovePack.h>

namespace idk
{
	enum class PredictionFunction
	{
		None,
		Linear,
		Quadratic,
	};

	template<typename T>
	struct ParameterImpl
	{

		function<T()>                          getter;
		function<void(const T&)>               setter;
		function <T(const T&, const T&, real)> interpolator;
		function <T(const T&, const T&)>       differ = [](const T& lhs, const T& rhs) -> T { return lhs - rhs; };
		function <T(const T&, const T&)>       adder  = [](const T& lhs, const T& rhs) -> T { return lhs + rhs; };
		function<bool(const T&, const T&)>     equater = std::equal_to<T>{};
		function<bool(const T&, const T&)>     send_condition = std::not_equal_to<T>{};
		PredictionFunction                     predict_func = PredictionFunction::Linear;

		ParameterImpl();

		template<typename Hnd, typename Mem>
		ParameterImpl(Handle<Hnd> obj, T(Mem::* ptr));
	};

	class ElectronView
		: public Component<ElectronView>
	{
	public:
		// state machines
		struct Master {};        // master object for the server. server owns this regardless of object ownership
		struct Ghost  {};         // ghost object for clients

		struct ClientObject {};  // when the client owns the object, the move_state is placed into clientobject
		struct ControlObject {}; // the server holds the controlobject

		ElectronView() = default;
		ElectronView(const ElectronView&);
		ElectronView(ElectronView&&) noexcept;
		ElectronView& operator=(const ElectronView&) ;
		ElectronView& operator=(ElectronView&&) noexcept ;
		~ElectronView();

		NetworkID network_id{};
		vector<GenericHandle> observed_components;
		int state_mask{};

		Host owner = Host::SERVER;

		variant<monostate, Master, Ghost> ghost_state;
		variant<monostate, ClientObject, ControlObject> move_state;

		bool IsMine() const;

		void Setup();
		void SetAsClientObject();
		void SetAsControlObject();

		void CacheSentData();
		void PrepareDataForSending();
		void MoveGhost(seconds delta);
		MovePack PackMoveData();
		GhostPack MasterPackData(int incoming_state_mask);
		void UnpackGhostData(SeqNo sequence_number, const GhostPack& data_pack);
		void UnpackMoveData(const MovePack& data_pack);

		hash_table<string, reflect::dynamic> GetParameters() const;

		template<typename T>
		ParameterImpl<T>& RegisterMember(string_view name, ParameterImpl<T> param, float interp = 1.f);
	private:
		struct BaseParameter;
		template<typename T>
		struct DerivedParameter;

		vector<unique_ptr<BaseParameter>> parameters;
	};

	struct ElectronView::BaseParameter
	{
		struct MasterData
		{
			SeqNo last_packed;

			virtual bool ValueChanged() const = 0;
			virtual void CacheValue(SeqNo pack_date)  = 0;
			virtual string PackData() = 0;
			virtual ~MasterData() = default;
		};

		struct GhostData
		{
			SeqNo value_index;
			real t = 1;

			virtual void UnpackData(SeqNo index, string_view) = 0;
			virtual void Update(real dt) = 0;
			virtual ~GhostData() = default;
		};

		// move data
		struct ClientObjectData
		{
			virtual void Init() = 0;
			virtual void CalculateMove(SeqNo curr_seq) = 0;
			virtual small_vector<SeqAndPack> PackData(SeqNo curr_seq) = 0;
			virtual void UnpackGhost(SeqNo index, string_view data) = 0;
			virtual ~ClientObjectData() = default;
		};

		struct ControlObjectData
		{
			virtual void Init() = 0;
			virtual MoveAck AcknowledgeMoves(SeqNo curr_seq) = 0;
			virtual void RecordPrediction(SeqNo curr_seq) = 0;
			virtual int UnpackMove(span<const SeqAndPack>) = 0;
			virtual ~ControlObjectData() = default;
		};

		string param_name;
		real   interp_over = 0.2f;

		virtual MasterData*        GetMaster() = 0;
		virtual GhostData*         GetGhost() = 0;
		virtual ClientObjectData*  GetClientObject() = 0;
		virtual ControlObjectData* GetControlObject() = 0;
		virtual ~BaseParameter() = default;
	};



	template<typename T>
	template<typename Hnd, typename Mem>
	inline ParameterImpl<T>::ParameterImpl(Handle<Hnd> obj, T(Mem::* ptr))
		: ParameterImpl{}
	{
		getter = [obj, ptr]()->T { return std::invoke(ptr, *obj); };
		setter = [obj, ptr](const T& val) { std::invoke(ptr, *obj) = val; };
	}

	template<typename T>
	inline ParameterImpl<T>::ParameterImpl()
	{
		if constexpr (std::is_same_v<T, real>)
			interpolator = &lerp<real, real>;

		if constexpr (std::is_same_v<T, vec3>)
			interpolator = &lerp<vec3, real>;

		if constexpr (std::is_same_v<T, quat>)
		{
			differ = [](const quat& lhs, const quat& rhs) -> quat { return lhs * rhs.inverse(); };
			adder = [](const quat& lhs, const quat& rhs) -> quat { return lhs * rhs; };
			interpolator = static_cast<quat(*)(const quat&, const quat&, real)>(&slerp<quat, real>);
		}
	}
}
