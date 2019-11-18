#pragma once
#include <core/ISystem.h>
#include <res/ResourceBundle.h>
namespace idk
{
	class AssetImporter
		: public ISystem
	{
	public:
		virtual void CheckImportDirectory();
		virtual ResourceBundle GetFile(string_view) = 0;
	};
}