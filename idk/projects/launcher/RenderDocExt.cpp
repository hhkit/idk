#include <Windows.h>
#include <renderdoc/renderdoc_app.h>

RENDERDOC_API_1_1_2*& GetRDocApi()
{
	static RENDERDOC_API_1_1_2* ptr=nullptr;
	return ptr;
}