#pragma once
#include <idk.h>
#include <core/Component.h>
#include <network/network.h>
#include <network/GhostPack.h>
#include <network/MovePack.h>
#include <meta/erased_visitor.h>

namespace idk
{
	using SyncableValue = variant<bool, int, float, vec3, quat>;

	template<typename T>
	struct ParameterImpl
	{

		function<T()>                          getter;
		function<void(const T&)>               setter;
		function<void(const T&)>               custom_move;
		function <T(const T&, const T&, real)> interpolator = []([[maybe_unused]] const T& lhs, const T& rhs, real) { return rhs; };
		function <T(const T&, const T&)>       differ = [](const T& lhs, const T& rhs) -> T { return lhs - rhs; };
		function <T(const T&, const T&)>       adder  = [](const T& lhs, const T& rhs) -> T { return lhs + rhs; };
		function<bool(const T&, const T&)>     equater = std::equal_to<T>{};
		function<bool(const T&, const T&)>     send_condition = std::not_equal_to<T>{};

		ParameterImpl();

		template<typename Hnd, typename Mem>
		ParameterImpl(Handle<Hnd> obj, T(Mem::* ptr));
	};

	class ElectronView
		: public Component<ElectronView>
	{
	public:
		friend class TestComponent;

		struct BaseParameter;

		// state machines
		struct Master {};        // master object for the server. server owns this regardless of object ownership
		struct Ghost  {};         // ghost object for clients

		struct MoveObject {};  // when the client owns the object, the move_state is placed into clientobject
		struct ControlObject {}; // the server holds the controlobject

		ElectronView() = default;
		ElectronView(const ElectronView&);
		ElectronView(ElectronView&&) noexcept;
		ElectronView& operator=(const ElectronView&) ;
		ElectronView& operator=(ElectronView&&) noexcept ;
		~ElectronView();

		NetworkID network_id{};
		vector<GenericHandle> observed_components;
		unsigned state_mask{};

		real interp_bias{ 0.03f };

		Host owner = Host::SERVER;

		variant<monostate, Master, Ghost> ghost_state;
		variant<monostate, MoveObject, ControlObject> move_state;

		bool IsMine() const;

		void Setup();
		void SetAsClientObject();
		void SetAsControlObject();

		void CacheSentData();
		void PrepareDataForSending(SeqNo curr_seq);
		void MoveGhost(seconds delta);
		MovePack PackMoveData(SeqNo curr_seq);
		GhostPack PackGhostData(int incoming_state_mask);

		void UnpackGhostData(SeqNo sequence_number, const GhostPack& data_pack);
		void UnpackMoveData(const MovePack& data_pack);

		void DumpToLog();
		span<const unique_ptr<BaseParameter>> GetParameters() const;

		template<typename T>
		BaseParameter* RegisterMember(string_view name, ParameterImpl<T> param, float interp = 1.f);
	private:
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
		struct MoveObjectData
		{
			// SequenceNumber, acknowledgement state, move
			using BufferVisitor = erased_visitor<void(vec3, SeqNo), void(quat, SeqNo), void(int, SeqNo), void(bool, SeqNo)>;

			SeqNo last_received;

			virtual void Init(SeqNo initial_frame) = 0;
			virtual void PushMove(SeqNo curr_seq, int move_type, const SyncableValue&) = 0;
			virtual small_vector<SeqAndPack> PackData(SeqNo curr_seq) = 0;

			virtual void UnpackGhost(SeqNo index, string_view data) = 0;
			virtual void UpdateGhost(real ghost_bias = 0.03f) = 0;

			virtual void VisitMoveBuffer(const BufferVisitor& visit) = 0;
			virtual ~MoveObjectData() = default;
		};

		struct ControlObjectData
		{
			// SequenceNumber, guess verification state, guess
			using BufferVisitor = erased_visitor<void(vec3, SeqNo), void(quat, SeqNo), void(int, SeqNo)>;

			virtual void Init(SeqNo initial_frame) = 0;
			virtual int  UnpackMove(span<const SeqAndPack>) = 0;
			virtual void ApplyMove() = 0;
			virtual void VisitMoveBuffer(const BufferVisitor& visit) = 0;
			virtual ~ControlObjectData() = default;
		};

		string param_name;
		real   interp_over = 0.2f;

		virtual GhostData*         GetGhost() = 0;
		virtual MasterData*        GetMaster() = 0;
		virtual MoveObjectData*    GetClientObject() = 0;
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
			differ = [](const quat& lhs, const quat& rhs) -> quat { return (rhs.inverse() * lhs).normalize(); };
			adder = [](const quat& lhs, const quat& rhs) -> quat { return (rhs * lhs).normalize(); };
			interpolator = static_cast<quat(*)(const quat&, const quat&, real)>(&slerp<quat, real>);
		}
	}
}
