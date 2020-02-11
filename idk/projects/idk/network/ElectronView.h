#pragma once
#include <idk.h>
#include <core/Component.h>
#include <network/network.h>
namespace idk
{
	template<typename T>
	struct ParameterImpl
	{
		function<T()>                          getter;
		function<void(const T&)>               setter;
		function <T(const T&, const T&, real)> interpolator;
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
		struct Master {};
		struct Ghost {};
		struct ClientObject {};

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

		variant<void*, Master, Ghost, ClientObject> network_data;

		void Setup();
		void SetAsClientObject();

		void CacheMasterValues();
		void UpdateClient();
		void UpdateMaster();
		void UpdateGhost();
		vector<string> PackMoveData();
		vector<string> PackGhostData();
		void UnpackGhostData(SeqNo sequence_number, span <string> data_pack);
		void UnpackMoveData(SeqNo sequence_number, span <string> data_pack);

		template<typename T>
		ParameterImpl<T>& RegisterMember(ParameterImpl<T> param, float interp = 1.f);
	private:
		struct BaseParameter;
		template<typename T>
		struct DerivedParameter;

		vector<unique_ptr<BaseParameter>> parameters;
	};

	struct ElectronView::BaseParameter
	{
		real t = 1;
		real interp_over = 1;
		SeqNo latest_seq;

		virtual void CacheCurrValue() = 0;
		virtual bool ValueChanged() const = 0;
		virtual void ApplyLerp(real delta_t) = 0;
		virtual void UnpackGhost(string_view) = 0;
		virtual void UnpackMove(SeqNo, string_view) = 0;
		virtual string PackGhostData() = 0;
		virtual string PackMoveData() = 0;
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
			interpolator = static_cast<quat(*)(const quat&, const quat&, real)>(&slerp<quat, real>);
	}
}
