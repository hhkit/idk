#pragma once
#include <idk.h>
#include <res/FileLoader.h>
#include <vkn/VknTextureLoader.h>
namespace idk::vkn
{
	enum class BlockType
	{
		eNone,
		eBC1,
		eBC2,
		eBC3,
		eBC4,
		eBC5
	};
	hash_table<BlockType, TextureFormat> MapBlockTypeToTextureFormat();
	TextureFormat BlockTypeToTextureFormat(BlockType tf);
	BlockType TextureFormatToBlockType(TextureFormat tf);

	struct DdsPixelFormat {
		uint32_t size;
		uint32_t flags;
		uint32_t fourCC;
		uint32_t rgbBitCount;
		uint32_t rBitMask;
		uint32_t gBitMask;
		uint32_t bBitMask;
		uint32_t aBitMask;
	};
	struct DdsHeader {
		uint32_t           header_size;
		uint32_t           flags;
		uint32_t           height;
		uint32_t           width;
		uint32_t           pitch_or_linear_size;
		uint32_t           depth;
		uint32_t           mip_map_count;
		uint32_t           reserved1[11];
		DdsPixelFormat     pf;
		uint32_t           caps;
		uint32_t           caps2;
		uint32_t           caps3;
		uint32_t           caps4;
		uint32_t           reserved2;
	};
	struct DdsFileInternal
	{
		uint32_t magic;
		DdsHeader header;
		char  data[1]; //To represent the start of the data.
		BlockType GetBlockType()const;
		const char(&GetFormatString()const)[4];
		uint32_t BlockSize()const;
		size_t NumBytes()const;
		string_view Data()const {return string_view{ data,NumBytes() };}
	};

	struct DdsFile
	{
		DdsFile() =default;
		DdsFile(string file) :_memory{ std::move(file) }, _view{ *_memory }, file{ r_cast<const DdsFileInternal*>(_view.data()) }{}
		DdsFile(string_view file)noexcept :_view{ file }, file{ r_cast<const DdsFileInternal*>(_view.data()) }{}

		DdsFile(const DdsFile& file) = default;
		DdsFile(DdsFile&& file) = default;
		DdsFile& operator=(const DdsFile& file) = default;
		DdsFile& operator=(DdsFile&& file) = default;


		DdsFile& operator=(string _file)
		{
			DdsFile tmp{ std::move(_file) };
			(*this) = std::move(tmp);
			return *this;
		}
		DdsFile& operator=(string_view _file) 
		{
			DdsFile tmp{ _file };
			(*this) = std::move(tmp);
			return *this;
		}

		string_view Data()const { return file->Data(); }
		const DdsFileInternal& File()const { return *file; }
		ivec2 Dimensions()const { return ivec2{ file->header.width, file->header.height }; }
	private:
		std::optional<std::string> _memory;
		string_view _view;
		const DdsFileInternal* file;
	};

	class DdsLoader
		: public IFileLoader
	{
	public:
		DdsLoader();
		ResourceBundle LoadFile(PathHandle path_to_resource, const MetaBundle& path_to_meta) override;
		void LoadTexture(VknTexture& tex,string_view entire_file, const TextureOptions& tex_opt);
		const hlp::MemoryAllocator& Allocator()const;
	private:
		vk::UniqueFence load_fence;
		TextureLoader loader;
		hlp::MemoryAllocator allocator;
	};

}