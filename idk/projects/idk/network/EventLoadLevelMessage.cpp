#include "stdafx.h"
#include "EventLoadLevelMessage.h"
#include <network/ElectronView.h>

namespace idk
{
	EventLoadLevelMessage& EventLoadLevelMessage::SetScene(RscHandle<Scene> scn)
	{
		scene = scn;
		return *this;
	}
	EventLoadLevelMessage& EventLoadLevelMessage::AddView(Handle<ElectronView> view)
	{
		++obj_count;
		obj_list.emplace_back(ViewMapping{ view, view->network_id });
		return *this;
	}
}
