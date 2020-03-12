#pragma once

#include <editor/windows/IGE_IWindow.h>

namespace idk {
	class IGE_NetworkSettings :
		public IGE_IWindow
	{
	public:
		IGE_NetworkSettings();

		virtual void Update() override;

	protected:
		virtual void BeginWindow() override;
		virtual void EndWindow_V() override;
	private:
		float network_time = 0.f;
		float packet_loss = 0.f;
	};





}