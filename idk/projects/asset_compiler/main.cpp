// asset_compiler.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <filesystem>

#include <idk.h>
#include <core/GameState.h>
#include <res/ResourceHandle.h>
#include <anim/Animation.h>
#include <serialize/binary.h>
#include <ReflectRegistration.h>
#include <util/ioutils.h>

#include "CompilerCore.h"
#include "CompilerList.h"

namespace fs = std::filesystem;

/*
	Usage: 
	- arg[0] is executable name
	- arg[1] is input file
	- arg[2] is output directory
*/
int main(int argc, const char* argv[])
{
	using namespace idk;

	GameState gs;

	if (argc < 2)
	{
		std::cout << "Proper usage:\n";
		std::cout << "   compiler.exe: [input_file] [output_file]\n";
		return -1;
	}
	auto src = fs::absolute(fs::path{ argv[1] });
	auto dest = fs::absolute(argc >= 3 ? fs::path{ argv[2] } : fs::path{ argv[1] }.parent_path());

	std::cout << "compiling " << src.generic_string();
	std::cout << " -> " << dest.generic_string();
	std::cout << std::endl;

	CompilerCore c;
	c.SetDestination(dest.generic_string());
	c.RegisterCompiler<DDSCompiler>(".dds");
	c.RegisterCompiler<TextureCompiler>(".tga");
	c.RegisterCompiler<TextureCompiler>(".png");
	c.RegisterCompiler<TextureCompiler>(".gif");
	c.RegisterCompiler<AssimpCompiler>(".fbx");
	c.RegisterCompiler<AssimpCompiler>(".obj");
	c.RegisterCompiler<AssimpCompiler>(".ma");

	c.Compile(src.generic_string());
	
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
