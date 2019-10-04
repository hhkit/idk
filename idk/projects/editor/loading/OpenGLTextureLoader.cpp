#include "pch.h"
#include <loading/OpenGLTextureLoader.h>
#include <gfx/GraphicsSystem.h>
#include <res/MetaBundle.h>
//Dep
#include <stb/stb_image.h>

//Resources
#include <idk_opengl/resource/OpenGLTexture.h>

#include <idk_opengl/DDSData.h>

namespace idk
{
	namespace ogl::detail
	{
		auto ToLGLinputChannels(InputChannels i)-> GLint
		{
			switch (i)
			{
			case InputChannels::RED:  return GL_RED;
			case InputChannels::RG:   return GL_RG;
			case InputChannels::RGB:  return GL_RGB;
			case InputChannels::RGBA: return GL_RGBA;
			case InputChannels::DEPTH_COMPONENT: return GL_DEPTH_COMPONENT;
			default: return 0;
			}
		}
		auto FromLGLinputChannels(GLint i)-> InputChannels
		{
			switch (i)
			{
			case GL_RED: return InputChannels::RED;
			case GL_RG:   return InputChannels::RG;
			case GL_RGB:  return InputChannels::RGB;
			case GL_RGBA: return InputChannels::RGBA;
			case GL_DEPTH_COMPONENT : return InputChannels::DEPTH_COMPONENT;
			default: return 0;
			}
		}
		auto ToLGLColor(ColorFormat f)-> GLenum
		{
			switch (f)
			{
			case ColorFormat::RG_8			 : return GL_RG;
			case ColorFormat::RGF_16		 : return GL_RG16F;
			case ColorFormat::RGB_8			 : return GL_RGB8;
			case ColorFormat::RGBA_8		 : return GL_RGBA8;
			case ColorFormat::RGBF_16		 : return GL_RGB16F;
			case ColorFormat::RGBF_32		 : return GL_RGB32F;
			case ColorFormat::RGBAF_16       : return GL_RGBA16F;
			case ColorFormat::RGBAF_32       : return GL_RGBA32F;
			case ColorFormat::SRGB: return GL_SRGB;
			case ColorFormat::DEPTH_COMPONENT: return GL_DEPTH_COMPONENT;
			case ColorFormat::DXT1			 : return 0x83F1;
			case ColorFormat::DXT3			 : return 0x83F2;
			case ColorFormat::DXT5			 : return 0x83F3;
			default: return 0;
			}
		}
		auto FromLGLColor(GLenum f)-> ColorFormat
		{
			switch (f)
			{
			case GL_RG: return GL_RG;
			case  GL_RG16F: return ColorFormat::RGF_16;
			case  GL_RGB8: return ColorFormat::RGB_8;
			case  GL_RGBA8: return ColorFormat::RGBA_8;
			case  GL_RGB16F: return ColorFormat::RGBF_16;
			case  GL_RGB32F: return ColorFormat::RGBF_32;
			case  GL_RGBA16F: return ColorFormat::RGBAF_16;
			case  GL_RGBA32F: return ColorFormat::RGBAF_32;
			case GL_SRGB: return ColorFormat::SRGB;
			case  GL_DEPTH_COMPONENT: return ColorFormat::DEPTH_COMPONENT;
			case  0x83F1: return ColorFormat::DXT1;
			case  0x83F2: return ColorFormat::DXT3;
			case  0x83F3: return ColorFormat::DXT5;
			default: return 0;
			}
		}
		
	};

	ResourceBundle OpenGLTextureLoader::LoadFile(PathHandle path_to_resource)
	{//Assert for now
		assert(Core::GetSystem<GraphicsSystem>().GetAPI() == GraphicsAPI::OpenGL);

		if (path_to_resource.GetExtension() == ".dds")
			return Load_DDS(path_to_resource);
		else
			return Load_stbi(path_to_resource);
		
	}

	ResourceBundle OpenGLTextureLoader::LoadFile(PathHandle path_to_resource, const MetaBundle& path_to_meta)
	{
		//auto& metadata = path_to_meta.metadatas[0];
		//auto texture_handle = Core::GetResourceManager().LoaderEmplaceResource<ogl::OpenGLTexture>(metadata.guid);
		if (path_to_resource.GetExtension() == ".dds")
			return Load_DDS(path_to_resource, path_to_meta);
		else
			return Load_stbi(path_to_resource,path_to_meta);

		//return texture_handle;
	}

	ResourceBundle OpenGLTextureLoader::Load_stbi(PathHandle path_to_resource)
	{
		const auto texture_handle = Core::GetResourceManager().LoaderEmplaceResource<ogl::OpenGLTexture>();

		auto tm = texture_handle->GetMeta();

		ivec2 size{};
		int channels{};

		auto data = std::unique_ptr<void, decltype(&stbi_image_free)>
		{
			stbi_load(path_to_resource.GetFullPath().data(), &size.x, &size.y, &channels, 0),
			stbi_image_free
		};
		if (data) // stbi image can fail
		{
			const auto col_format = [&]() -> InputChannels
			{	switch (channels)
			{
			default:
			case 1: return InputChannels::RED;
			case 2: return InputChannels::RG;
			case 3: return InputChannels::RGB;
			case 4: return InputChannels::RGBA;
			}
			}();

			texture_handle->Buffer(data.get(), size, col_format);
		}

		return texture_handle;
	}

	ResourceBundle OpenGLTextureLoader::Load_DDS(PathHandle path_to_resource)
	{
		using namespace ogl;
		
		
		DDS_HEADER header;
	const	auto texture_handle = Core::GetResourceManager().LoaderEmplaceResource<ogl::OpenGLTexture>(true);
		auto tm = texture_handle->GetMeta();
		const char* filePath = path_to_resource.GetFullPath().data();

		FILE* fp;

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
		const unsigned int linearSize = header.dwPitchOrLinearSize;

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
			if (!data) {
				goto failure;
			}
			//Get the original texture image data
			tm.internal_format = ogl::detail::FromLGLColor(li->internalFormat);
			tm.format = ogl::detail::FromLGLinputChannels(li->externalFormat);
			texture_handle->Size(ivec2(width, height));
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
			
			//Image is a uncompressed image
			assert(header.dwFlags & DDSD_PITCH);
			assert(header.ddspf.dwRGBBitCount == 8);
			size_t size = header.dwPitchOrLinearSize * height;

			//Assert if mipmap padding is not equals to the size of the dds block format
			assert(size == x * y * li->blockBytes);

			//Get the original texture image data
			//temp_t->width = width;
			//temp_t->height = height;
			//temp_t->Image_Format = li->externalFormat;
			//temp_t->Internal_Format = li->internalFormat;
			tm.internal_format = ogl::detail::FromLGLColor(li->internalFormat);
			tm.format = ogl::detail::FromLGLinputChannels(li->externalFormat);
			texture_handle->Size(ivec2(width, height));

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
			
		}
		else {
			//Image is other kinds of dds format
			if (li->swap) {
				glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_TRUE);
			}
			size_t size = x * y * li->blockBytes;

			//Get the original texture image data
			tm.internal_format = ogl::detail::FromLGLColor(li->internalFormat);
			tm.format = ogl::detail::FromLGLinputChannels(li->externalFormat);
			texture_handle->Size(ivec2(width, height));

			//assert(header.dwFlags & DDSD_LINEARSIZE);
			unsigned char* data = (unsigned char*)malloc(size);

			//Fail if data is corrupted
			if (!data) {
				goto failure;
			}
			//Get the original texture image data
			texture_handle->Size(ivec2(width, height));
			//Read the mip maps and create textures based on them
			fread(data, 1, size, fp);
			//Read the data to start pulling out mip maps of the texture
			for (unsigned int ix = 0; ix < mipMapCount; ++ix) {

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
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipMapCount - 1);

		//Store the path of the image
		//temp_t->path = imagepath;

		//Unbind the image
		//temp_t->Unbind();
		glBindTexture(GL_TEXTURE_2D,0);

		//Close the connection
		fclose(fp);

		texture_handle->SetMeta(tm);

		return texture_handle;

		//Flag if image fails
	failure:
		//return false;
		return texture_handle;
	
	}

	ResourceBundle OpenGLTextureLoader::Load_stbi(PathHandle path_to_resource, const MetaBundle& path_to_meta)
	{
		auto& metadata = path_to_meta.metadatas[0];
		const auto texture_handle = Core::GetResourceManager().LoaderEmplaceResource<ogl::OpenGLTexture>(metadata.guid);

		auto first_meta = metadata.GetMeta<Texture>();
		if (first_meta)
			texture_handle->SetMeta(*first_meta);

		ivec2 size{};
		int channels{};

		auto data = std::unique_ptr<void, decltype(&stbi_image_free)>
		{
			stbi_load(path_to_resource.GetFullPath().data(), &size.x, &size.y, &channels, 0),
			stbi_image_free
		};

		if (data) // stbi image can fail
		{
			const auto col_format = [channels]() -> InputChannels
			{
				switch (channels)
				{
				default:
				case 1: return InputChannels::RED;
				case 2: return InputChannels::RG;
				case 3: return InputChannels::RGB;
				case 4: return InputChannels::RGBA;
				}
			}();

			texture_handle->Buffer(data.get(), size, col_format, first_meta->internal_format);

			return texture_handle;
		}

		return texture_handle;
	}

	ResourceBundle OpenGLTextureLoader::Load_DDS(PathHandle path_to_resource, const MetaBundle& path_to_meta)
	{
		using namespace ogl;
		//unsigned char header[124];
		DDS_HEADER header;

		auto& metadata = path_to_meta.metadatas[0];

		const auto texture_handle = Core::GetResourceManager().LoaderEmplaceResource<ogl::OpenGLTexture>(metadata.guid, true);

        auto v = metadata.GetMeta<OpenGLTexture>().value_or(Texture::Metadata{});

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
			v.internal_format = ogl::detail::FromLGLColor(li->internalFormat);
			v.format = ogl::detail::FromLGLinputChannels(li->externalFormat);
			texture_handle->Size(ivec2(width, height));
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
			v.internal_format = ogl::detail::FromLGLColor(li->internalFormat);
			v.format = ogl::detail::FromLGLinputChannels(li->externalFormat);
			texture_handle->Size(ivec2(width, height));

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
			texture_handle->Size(ivec2(width, height));

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

		texture_handle->SetMeta(v);
		//Unbind the image
		//temp_t->Unbind();
		glBindTexture(GL_TEXTURE_2D, 0);

		//Close the connection
		fclose(fp);

		return texture_handle;
	}

};