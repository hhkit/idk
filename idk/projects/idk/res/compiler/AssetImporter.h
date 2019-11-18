#pragma once
#include <core/ISystem.h>
#include <res/ResourceBundle.h>
namespace idk
{
	class AssetImporter
		: public ISystem
	{
	public:
		virtual void CheckImportDirectory() = 0;
		virtual ResourceBundle GetFile(string_view) = 0;
	};
}