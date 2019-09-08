#pragma once
#include <idk.h>
#include <core/ISystem.h>
#include <ds/dual_set.h>
#include <res/ResourceHandle.h>
#include <res/SaveableResource.h>
#include <file/FileHandle.h>

namespace idk
{

	// this class has one job: maintain the 1:1 link between saveable resources and their virtual path
	class SaveableResourceManager
		: public ISystem
	{
	public:
		enum class FileAssociateResult
		{
			Ok = 0,
			Err_HandleNotRegistered,
			Err_FileAlreadyExists,
			Err_FileCreationFailure,
		};

		enum class SaveResourceResult
		{
			Ok = 0,
			Err_ResourceNoAssociatedFile,
			Err_HandleNotHandled
		};

		template<typename T> void RegisterHandle(RscHandle<T> handle);               // no file
		template<typename T> void RegisterHandle(RscHandle<T> handle, FileHandle f); // with file
		template<typename T> bool DeregisterHandle(RscHandle<T> handle);

		template<typename T> FileAssociateResult Associate(RscHandle<T> handle, string_view mountPath);

		void SaveDirtyFiles();

		template<typename T>
		SaveResourceResult Save(RscHandle<T>);
	private:
		void Init() override;
		void Shutdown() override;

		struct ControlBlock
		{
			opt<FileHandle> associated_file;
			size_t resource_id;
		};
		SaveResourceResult Save(Guid);

		dual_set  <Guid, string_view> files;
		hash_table<Guid, ControlBlock> control_blocks;
	};
}

#include "SaveableResourceManager.inl"