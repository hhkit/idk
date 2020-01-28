#include "pch.h"
#include <opengl/resource/OpenGLTextureLoader.h>
#include <gfx/GraphicsSystem.h>
#include <res/MetaBundle.inl>
//Dep
#include <stb/stb_image.h>

//Resources
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <res/ResourceUtils.inl>
#include <res/ResourceBundle.inl>
#include <opengl/resource/OpenGLTexture.h>
#include <res/ResourceMeta.inl>

#include <opengl/DDSData.h>
#include <res/Guid.inl>
#include <ds/span.inl>

namespace idk
{
	namespace ogl::detail
	{

		auto ToLGLColor(TextureInternalFormat f)-> GLenum
		{
			switch (f)
			{
			case TextureInternalFormat::RG_8		: return GL_RG;
			case TextureInternalFormat::RG_16_F		: return GL_RG16F;
			case TextureInternalFormat::RGB_8		: return GL_RGB8;
			case TextureInternalFormat::RGBA_8		: return GL_RGBA8;
			case TextureInternalFormat::RGB_16_F	: return GL_RGB16F;
			case TextureInternalFormat::RGB_32_F	: return GL_RGB32F;
			case TextureInternalFormat::RGBA_16_F   : return GL_RGBA16F;
			case TextureInternalFormat::RGBA_32_F   : return GL_RGBA32F;
			case TextureInternalFormat::SRGB_8      : return GL_SRGB;
			case TextureInternalFormat::DEPTH_16    : return GL_DEPTH_COMPONENT;
			case TextureInternalFormat::RGB_DXT1	: return 0x83F1;
			case TextureInternalFormat::RGBA_DXT3	: return 0x83F2;
			case TextureInternalFormat::RGBA_DXT5	: return 0x83F3;
			default: return 0;
			}
		}

		auto FromLGLColor(GLenum f)-> TextureInternalFormat
		{
			switch (f)
			{
			case GL_RG: return TextureInternalFormat::RG_8;
			case  GL_RG16F: return TextureInternalFormat::RG_16_F;
			case  GL_RGB8: return TextureInternalFormat::RGB_8;
			case  GL_RGBA8: return TextureInternalFormat::RGBA_8;
			case  GL_RGB16F: return TextureInternalFormat::RGB_16_F;
			case  GL_RGB32F: return TextureInternalFormat::RGB_32_F;
			case  GL_RGBA16F: return TextureInternalFormat::RGBA_16_F;
			case  GL_RGBA32F: return TextureInternalFormat::RGBA_32_F;
			case  GL_SRGB: return TextureInternalFormat::SRGB_8;
			case  GL_DEPTH_COMPONENT: return TextureInternalFormat::DEPTH_16;
			case  0x83F1: return TextureInternalFormat::RGB_DXT1;
			case  0x83F2: return TextureInternalFormat::RGBA_DXT3;
			case  0x83F3: return TextureInternalFormat::RGBA_DXT5;
			default: return {};
			}
		}
		
	};

	ResourceBundle OpenGLTextureLoader::LoadFile(PathHandle path_to_resource, const MetaBundle& path_to_meta)
	{
		if (path_to_resource.GetExtension() == ".dds")
			return Load_DDS(path_to_resource, path_to_meta);
		else
			return Load_stbi(path_to_resource,path_to_meta);
	}

	ResourceBundle OpenGLTextureLoader::Load_stbi(PathHandle path_to_resource, const MetaBundle& path_to_meta)
	{
		auto meta = path_to_meta.FetchMeta<Texture>();
		const auto texture_handle = meta 
			? Core::GetResourceManager().LoaderEmplaceResource<ogl::OpenGLTexture>(meta->guid)
			: Core::GetResourceManager().LoaderEmplaceResource<ogl::OpenGLTexture>();

		if (meta)
			texture_handle->GetMeta() = *meta->GetMeta<Texture>();
		
		ivec2 size{};
		int channels{};

		auto data = std::unique_ptr<void, decltype(&stbi_image_free)>
		{
			stbi_load(path_to_resource.GetFullPath().data(), &size.x, &size.y, &channels, 0),
			stbi_image_free
		};

		if (data) // stbi image can fail
		{
			auto texture_meta = *meta->GetMeta<Texture>();
			texture_handle->Buffer(data.get(), 4 * size.x * size.y, uvec2{ size }, texture_meta.internal_format, texture_meta.is_srgb);

			return texture_handle;
		}

		return texture_handle;
	}

	ResourceBundle OpenGLTextureLoader::Load_DDS(PathHandle path_to_resource, const MetaBundle& path_to_meta)
	{
		using namespace ogl;
		//unsigned char header[124];
		DDS_HEADER header;

		auto meta = path_to_meta.FetchMeta<Texture>();
		const auto texture_handle = meta
			? Core::GetResourceManager().LoaderEmplaceResource<ogl::OpenGLTexture>(meta->guid)
			: Core::GetResourceManager().LoaderEmplaceResource<ogl::OpenGLTexture>();


		Texture::Metadata v;
		if (meta)
			v = *meta->GetMeta<Texture>();

		const char* filePath = path_to_resource.GetFullPath().data();
		FILE* fp = nullptr;

		/* try to open the file */
		fopen_s(&fp, filePath, "rb");
		if (fp == NULL)
			return texture_handle;

		/* verify the type of file */
		char filecode[4];
		fread(filecode, 1, 4, fp);

		//Check for magic code and see if this is a dds texture
		if (strncmp(filecode, "DDS ", 4) != 0) {
			fclose(fp);
			return texture_handle;
		}


		// get the header data
		fread(&header, sizeof(header), 1, fp);

		//Get the width, height and mipmapcount of the data
		const unsigned int height = header.dwHeight;
		const unsigned int width = header.dwWidth;
		unsigned int mipMapCount = header.dwMipMapCount;

		//Load dds info
		DdsLoadInfo* li = nullptr;

		//Check header to load the dds format accordingly
		if (PF_IS_DXT1(header.ddspf)) {
			li = &loadInfoDXT1;
		}
		else if (PF_IS_DXT3(header.ddspf)) {
			li = &loadInfoDXT3;
		}
		else if (PF_IS_DXT5(header.ddspf)) {
			li = &loadInfoDXT5;
		}
		else if (PF_IS_BGR8(header.ddspf)) {
			li = &loadInfoBGR8;
		}
		else if (PF_IS_BGR565(header.ddspf)) {
			li = &loadInfoBGR565;
		}
		else if (PF_IS_INDEX8(header.ddspf)) {
			li = &loadInfoIndex8;
		}
		else
		{
			//The texture might not be a dds image, return
			return texture_handle;
		}

		//Construct new texture

		//Set x and y as current texture (x & y is configurable)
		unsigned int x = width;
		unsigned int y = height;

		//Bind current texture
		//temp_t->Bind(0);
		texture_handle->Bind();

		//Set texture parameter to generate mipmap 
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // don't forget to enable mipmaping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // don't forget to enable mipmaping
		mipMapCount = (header.dwFlags & DDSD_MIPMAPCOUNT) ? mipMapCount : 1;

		//Check if image is compressed
		if (li->compressed) {
			//Image is a compressed image
			GLsizei size = std::max(li->divSize, x) / li->divSize * std::max(li->divSize, y) / li->divSize * li->blockBytes;

			//Check if this is a compressed dds image that uses linear size
			//assert(header.dwFlags & DDSD_LINEARSIZE);
			unsigned char* data = (unsigned char*)malloc(size);

			//Fail if data is corrupted
			if (!data)
				return texture_handle;
		
			//Get the original texture image data
			texture_handle->Size(uvec2(width, height));
			//Read the mip maps and create textures based on them
			fread(data, 1, size, fp);
			unsigned offset = 0;
			//Read the data to start pulling out mip maps of the texture
			for (unsigned int ix = 0; ix < mipMapCount; ++ix) {

				//Ensure that mipmap size is nvr less than its division size (ratio of the image)
				size = std::max(li->divSize, x) / li->divSize * std::max(li->divSize, y) / li->divSize * li->blockBytes;
				glCompressedTexImage2D(GL_TEXTURE_2D, ix, li->internalFormat, x, y, 0, size, data + offset);

				//Down the mipmap size
				x = (x + 1) >> 1;
				y = (y + 1) >> 1;

				offset += size;
			}

			//Free the image data
			free(data);
		}
		else if (li->palette) {
			
			//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
			//mipMapCount = (header.dwFlags & DDSD_MIPMAPCOUNT) ? header.dwMipMapCount : 1;
			//Image is a uncompressed image
			//assert(header.dwFlags & DDSD_PITCH);
			//assert(header.ddspf.dwRGBBitCount == 8);
			size_t size = header.dwPitchOrLinearSize * height;

			//Assert if mipmap padding is not equals to the size of the dds block format
			assert(size == x * y * li->blockBytes);

			//Get the original texture image data
			texture_handle->Size(uvec2(width, height));

			//get the data for the image
			unsigned char* data = (unsigned char*)malloc(size);
			unsigned int palette[256];
			unsigned int* unpacked = (unsigned int*)malloc(size * sizeof(unsigned int));

			//Read the data to start pulling out mip maps of the texture
			fread(palette, 4, 256, fp);
			for (unsigned int ix = 0; ix < mipMapCount; ++ix) {

				//Read the mip maps and create textures based on them
				fread(data, 1, size, fp);
				for (unsigned int zz = 0; zz < size; ++zz) {
					unpacked[zz] = palette[data[zz]];
				}

				//Down the mipmap size
				glPixelStorei(GL_UNPACK_ROW_LENGTH, y);
				glTexImage2D(GL_TEXTURE_2D, ix, li->internalFormat, x, y, 0, li->externalFormat, li->type, unpacked);

				//Ensure that mipmap size is nvr less than its original texture size
				x = (x + 1) >> 1;
				y = (y + 1) >> 1;
				size = x * y * li->blockBytes;
			}

			//Free the image data
			free(data);
			free(unpacked);

			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipMapCount - 1);
			
		}
		else {
			//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
			//mipMapCount = (header.dwFlags & DDSD_MIPMAPCOUNT) ? header.dwMipMapCount : 1;
			//Image is other kinds of dds format
			if (li->swap) {
				glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_TRUE);
			}
			size_t size = x * y * li->blockBytes;

			//Get the original texture image data
			texture_handle->Size(uvec2(width, height));

			unsigned char* data = (unsigned char*)malloc(size);

			for (unsigned int ix = 0; ix < mipMapCount; ++ix) {
				//Read the data for each mip maps of the texture
				fread(data, 1, size, fp);

				//Down the mipmap size
				glPixelStorei(GL_UNPACK_ROW_LENGTH, y);
				glTexImage2D(GL_TEXTURE_2D, ix, li->internalFormat, x, y, 0, li->externalFormat, li->type, data);

				//Ensure that mipmap size is nvr less than its original texture size
				x = (x + 1) >> 1;
				y = (y + 1) >> 1;
				size = x * y * li->blockBytes;
			}

			//Free the image data
			free(data);
			glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_FALSE);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipMapCount - 1);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipMapCount - 1);
		

		//Store the path of the image

		texture_handle->GetMeta() = v;
		//Unbind the image
		//temp_t->Unbind();
		glBindTexture(GL_TEXTURE_2D, 0);

		//Close the connection
		fclose(fp);

		return texture_handle;
	}

};