#include "pch.h"
#include"Utils.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
namespace idk {
	namespace utl
	{
		std::ostream& cerr()
		{
			return std::cerr;
		}
	}
	std::string GetBinaryFile(const std::string& filepath)
	{
		std::ifstream file{ filepath,std::ios::binary };
		std::stringstream data;
		data << file.rdbuf();
		file.close();
		return data.str();
	}
}
