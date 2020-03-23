#pragma once
#include <idk.h>
#include <res/Guid.h>
namespace idk
{

	struct SimpleTexData
	{
		uvec2 dimensions{};
		//array of rows
		vector<byte> data{};
	};

	Guid GetDefaultColorGradeGuid();
	SimpleTexData GenerateRgbDefaultColorGradeTexData (byte default_fill=static_cast<byte>(0xFF));
	SimpleTexData GenerateRgbaDefaultColorGradeTexData(byte default_fill=static_cast<byte>(0xFF));
}