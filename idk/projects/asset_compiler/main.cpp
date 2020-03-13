// asset_compiler.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <filesystem>
#include <windows.h>

#include <idk.h>
#include <core/GameState.h>
#include <res/ResourceHandle.h>
#include <anim/Animation.h>
#include <serialize/binary.h>
#include <util/ioutils.h>

#include "CompilerCore.h"
#include "CompilerList.h"
#include "SceneUpdater.h"

namespace fs = std::filesystem;

/*
	Usage: 
	- arg[0] is executable name
	- arg[1] is input file
	- arg[2] is output directory
*/
int main(int argc, const char* argv[]);

int WINAPI wWinMain(
	HINSTANCE   hInstance,
	HINSTANCE   hPrevInstance,
	PWSTR       lpCmdLine,
	int         nCmdShow
)
{
	int argc{};
	LPWSTR* wargv = CommandLineToArgvW(lpCmdLine, &argc);;
	std::vector<idk::string> strargv;
	std::vector<const char*> argv;
	for (auto itr = wargv; *itr; ++itr)
	{
		auto wstr = std::wstring{ *itr };
		int size = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, &wstr[0], static_cast<int>(wstr.size()), NULL, 0, NULL, NULL);
		std::string ret = std::string(size, 0);
		WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, &wstr[0], static_cast<int>(wstr.size()), &ret[0], size, NULL, NULL);
		strargv.push_back(ret);
	}

	argv.emplace_back("");
	for (auto& elem : strargv)
		argv.emplace_back(elem.data());
	argv.emplace_back(nullptr);

	LocalFree(wargv);

	return main(argc + 1, argv.data());
}

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

	if (string(argv[1]) == "update")
	{
		auto src = fs::absolute(fs::path{ argv[2] });
		UpdateScene(src.string());
		return 0;
	}

	auto src = fs::absolute(fs::path{ argv[1] });
	auto dest = fs::absolute(argc >= 3 ? fs::path{ argv[2] } : fs::path{ argv[1] }.parent_path());
	auto time_dir = dest; if (argc >= 4) time_dir = argv[3];

	std::cout << "TEST compiling " << src.generic_string();
	std::cout << " -> " << dest.generic_string() << "\n";

	CompilerCore c;
	c.time_dir = time_dir.string();
	c.SetDestination(dest.generic_string());
	c.RegisterCompiler<DDSCompiler>(".dds");
	c.RegisterCompiler<TextureCompiler>(".bmp");
	c.RegisterCompiler<TextureCompiler>(".tga");
	c.RegisterCompiler<TextureCompiler>(".png");
	c.RegisterCompiler<TextureCompiler>(".gif");
	c.RegisterCompiler<TextureCompiler>(".jpg");
	c.RegisterCompiler<TextureCompiler>(".jpeg");
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
