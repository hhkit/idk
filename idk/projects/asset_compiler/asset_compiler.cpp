// asset_compiler.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>

#include <idk.h>
#include <res/ResourceHandle.h>
#include <anim/Animation.h>
#include <serialize/binary.h>
#include <ReflectRegistration.h>
#include <util/ioutils.h>
/*
	Usage: 
	- arg[0] is executable name
	- arg[1] is input file
	- arg[2] is output directory
*/
int main(int argc, const char* argv[])
{
	using namespace idk;

	if (argc < 3)
	{
		std::cout << "Proper usage:\n";
		std::cout << "   compiler.exe: [input_file] [output_file]\n";
	}
	else
	{
		anim::Animation a;
		a.AddAnimatedBone(anim::AnimatedBone{ "nani", {
			{vec3{}, 0.5f}
		}});
		a.SetSpeeds(30.f, 1.f, 10.f);
		{
			std::ofstream ostr;
			ostr.open(argv[2], std::ios::binary);
			ostr << serialize_binary(a);
		}
		{
			anim::Animation b;
			std::ifstream istr(argv[2], std::ios::binary);

			auto str = stringify(istr);
			parse_binary(str, b);

			
		}
	}
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
