#include "stdafx.h"
#include "ElectronAnimatorView.h"
#include <core/GameObject.inl>
#include <anim/Animator.h>
#include <network/ElectronView.inl>
#include <network/GhostFlags.h>

namespace idk
{
	NetworkID ElectronAnimatorView::GetNetworkID() const
	{
		return GetView()->network_id;
	}
	Handle<ElectronView> ElectronAnimatorView::GetView() const
	{
		return GetGameObject()->GetComponent<ElectronView>();
	}
	void ElectronAnimatorView::Start()
	{
		auto view = GetGameObject()->GetComponent<ElectronView>();
		if (!view)
			return;

		// Get all synced params based on type and name. 
		auto& animator = *GetGameObject()->GetComponent<Animator>();
		const auto register_params = [&](auto& param_names, auto type)
		{
			using ParamType = decltype(type);
			for (auto& synced_p : param_names)
			{
				auto& param = animator.GetParam<ParamType>(synced_p);
				using DataType = std::decay_t<decltype(param.val)>;

				if (param.valid)
				{
					ParameterImpl<DataType> impl;
					impl.getter = [ahandle = animator.GetHandle(), param_name = synced_p]() { return ahandle->GetParam<ParamType>(param_name).val; };
					impl.setter = [ahandle = animator.GetHandle(), param_name = synced_p](DataType val) { ahandle->GetParam<ParamType>(param_name).val = val; };
					view->RegisterMember(param.name, impl);
				}
			}

		};
	}
}
