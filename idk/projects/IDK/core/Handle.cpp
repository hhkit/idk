#include "stdafx.h"
#include "Handle.h"

namespace idk
{
	GenericHandle::GenericHandle(uint64_t id)
		: id{id}
	{
	}
	GenericHandle::GenericHandle(uint8_t type, uint8_t scene, uint16_t gen, uint32_t index)
		: type{ type }, scene{ scene }, gen{gen}, index{index}
	{
	}
}