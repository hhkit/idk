#pragma once

namespace idk {

	class EditorInputs;
	
	namespace edt {
		class I_Interface {
		public:
			I_Interface()=default;
			virtual ~I_Interface() = default;

			virtual void Init() = 0;
			virtual void Shutdown() = 0;

			virtual void ImGuiFrameBegin()=0;
			virtual void ImGuiFrameUpdate()=0;
			virtual void ImGuiFrameEnd()=0;

			virtual void ImGuiFrameRender()=0;

			virtual EditorInputs* Inputs() = 0;
			
			void PushImGuiFunction(std::function<void()>);
			void PushImGuiFunction(void());
		private:
			vector<std::function<void()>> imgui_plainOldFuncList;
			
		};
	};
};