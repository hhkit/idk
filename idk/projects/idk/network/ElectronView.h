#pragma once
#include <idk.h>
#include <core/Component.h>
#include <network/network.h>
namespace idk
{
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
		void UnpackGhostData(span <string> data_pack);
		void UnpackMoveData(span <string> data_pack);

		template<typename Hnd, typename Obj, typename Mem>
		void RegisterMember(Handle<Hnd> obj, Obj(Mem::*), float interp = 1.f, 
			function<bool(const Obj&, const Obj&)> send_when = std::not_equal_to<Obj>{});
		template<typename Val>
		void RegisterMember(function<Val()> getter, function<void(const Val&)> setter, float interp = 1.f,
			function<bool(const Val&, const Val&)> send_when = std::not_equal_to<Val>{});
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

		virtual void CacheCurrValue() = 0;
		virtual bool ValueChanged() const = 0;
		virtual void ApplyLerp(real delta_t) = 0;
		virtual void UnpackGhost(string_view) = 0;
		virtual void UnpackMove(string_view) = 0;
		virtual string PackGhostData() = 0;
		virtual string PackMoveData() = 0;
		virtual ~BaseParameter() = default;
	};
}
