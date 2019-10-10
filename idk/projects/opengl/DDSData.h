#pragma once
/******************************************************************************/
/*!
\project GAM200
\file    DDSData.h
\author  Chong Wei Xiang
\brief
	All content (C) 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
	Reproduction or disclosure of this file or its contents without the prior
	 written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#pragma once
#ifndef DDSDATA_H
#define DDSDATA_H
#include "windows.h"
#include <glad/glad.h>
#endif

namespace idk::ogl
{

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3

	struct DDS_PIXELFORMAT  // DDPIXELFORMAT
	{
		DWORD dwSize;
		DWORD dwFlags;
		DWORD dwFourCC;
		DWORD dwRGBBitCount;
		DWORD dwRBitMask;
		DWORD dwGBitMask;
		DWORD dwBBitMask;
		DWORD dwABitMask;
	};

	typedef struct DDS_HEADER {
		DWORD           dwSize;
		DWORD           dwFlags;
		DWORD           dwHeight;
		DWORD           dwWidth;
		DWORD           dwPitchOrLinearSize;
		DWORD           dwDepth;
		DWORD           dwMipMapCount;
		DWORD           dwReserved1[11];
		DDS_PIXELFORMAT  ddspf;
		DWORD           dwCaps;
		DWORD           dwCaps2;
		DWORD           dwCaps3;
		DWORD           dwCaps4;
		DWORD           dwReserved2;
	} DDS_HEADER;

	struct DdsLoadInfo {
		bool compressed;
		bool swap;
		bool palette;
		unsigned int divSize;
		unsigned int blockBytes;
		GLenum internalFormat;
		GLenum externalFormat;
		GLenum type;
	};

	DdsLoadInfo loadInfoDXT1 = {
	  true, false, false, 4, 8, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
	};
	DdsLoadInfo loadInfoDXT2 = {
	  true, false, false, 4, 16, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
	};
	DdsLoadInfo loadInfoDXT3 = {
	  true, false, false, 4, 16, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
	};
	DdsLoadInfo loadInfoDXT4 = {
	  true, false, false, 4, 16, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
	};
	DdsLoadInfo loadInfoDXT5 = {
	  true, false, false, 4, 16, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
	};
	DdsLoadInfo loadInfoBGRA8 = {
	  false, false, false, 1, 4, GL_RGBA8, GL_BGRA, GL_UNSIGNED_BYTE
	};
	DdsLoadInfo loadInfoBGR8 = {
	  false, false, false, 1, 3, GL_RGB8, GL_BGR, GL_UNSIGNED_BYTE
	};
	DdsLoadInfo loadInfoBGR5A1 = {
	  false, true, false, 1, 2, GL_RGB5_A1, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV
	};
	DdsLoadInfo loadInfoBGR565 = {
	  false, true, false, 1, 2, GL_RGB5, GL_RGB, GL_UNSIGNED_SHORT_5_6_5
	};
	DdsLoadInfo loadInfoIndex8 = {
	  false, false, true, 1, 1, GL_RGB8, GL_BGRA, GL_UNSIGNED_BYTE
	};

	//  DDS_header.dwFlags
#define DDSD_CAPS                   0x00000001 
#define DDSD_HEIGHT                 0x00000002 
#define DDSD_WIDTH                  0x00000004 
#define DDSD_PITCH                  0x00000008 
#define DDSD_PIXELFORMAT            0x00001000 
#define DDSD_MIPMAPCOUNT            0x00020000 
#define DDSD_LINEARSIZE             0x00080000 
#define DDSD_DEPTH                  0x00800000 

//  DDS_header.sPixelFormat.dwFlags
#define DDPF_ALPHAPIXELS            0x00000001 
#define DDPF_FOURCC                 0x00000004 
#define DDPF_INDEXED                0x00000020 
#define DDPF_RGB                    0x00000040 

//  DDS_header.sCaps.dwCaps1
#define DDSCAPS_COMPLEX             0x00000008 
#define DDSCAPS_TEXTURE             0x00001000 
#define DDSCAPS_MIPMAP              0x00400000 

//  DDS_header.sCaps.dwCaps2
#define DDSCAPS2_CUBEMAP            0x00000200 
#define DDSCAPS2_CUBEMAP_POSITIVEX  0x00000400 
#define DDSCAPS2_CUBEMAP_NEGATIVEX  0x00000800 
#define DDSCAPS2_CUBEMAP_POSITIVEY  0x00001000 
#define DDSCAPS2_CUBEMAP_NEGATIVEY  0x00002000 
#define DDSCAPS2_CUBEMAP_POSITIVEZ  0x00004000 
#define DDSCAPS2_CUBEMAP_NEGATIVEZ  0x00008000 
#define DDSCAPS2_VOLUME             0x00200000 

#define D3DFMT_DXT1     '1TXD'    //  DXT1 compression texture format 
#define D3DFMT_DXT2     '2TXD'    //  DXT2 compression texture format 
#define D3DFMT_DXT3     '3TXD'    //  DXT3 compression texture format 
#define D3DFMT_DXT4     '4TXD'    //  DXT4 compression texture format 
#define D3DFMT_DXT5     '5TXD'    //  DXT5 compression texture format 


#define PF_IS_DXT1(pf) \
  ((pf.dwFlags & DDPF_FOURCC) && \
   (pf.dwFourCC == D3DFMT_DXT1))

#define PF_IS_DXT2(pf) \
  ((pf.dwFlags & DDPF_FOURCC) && \
   (pf.dwFourCC == PF_IS_DXT2))

#define PF_IS_DXT3(pf) \
  ((pf.dwFlags & DDPF_FOURCC) && \
   (pf.dwFourCC == D3DFMT_DXT3))

#define PF_IS_DXT4(pf) \
  ((pf.dwFlags & DDPF_FOURCC) && \
   (pf.dwFourCC == PF_IS_DXT4))

#define PF_IS_DXT5(pf) \
  ((pf.dwFlags & DDPF_FOURCC) && \
   (pf.dwFourCC == D3DFMT_DXT5))

#define PF_IS_BGRA8(pf) \
  ((pf.dwFlags & DDPF_RGB) && \
   (pf.dwFlags & DDPF_ALPHAPIXELS) && \
   (pf.dwRGBBitCount == 32) && \
   (pf.dwRBitMask == 0xff0000) && \
   (pf.dwGBitMask == 0xff00) && \
   (pf.dwBBitMask == 0xff) && \
   (pf.dwAlphaBitMask == 0xff000000U))

#define PF_IS_BGR8(pf) \
  ((pf.dwFlags & DDPF_ALPHAPIXELS) && \
  !(pf.dwFlags & DDPF_ALPHAPIXELS) && \
   (pf.dwRGBBitCount == 24) && \
   (pf.dwRBitMask == 0xff0000) && \
   (pf.dwGBitMask == 0xff00) && \
   (pf.dwBBitMask == 0xff))

#define PF_IS_BGR5A1(pf) \
  ((pf.dwFlags & DDPF_RGB) && \
   (pf.dwFlags & DDPF_ALPHAPIXELS) && \
   (pf.dwRGBBitCount == 16) && \
   (pf.dwRBitMask == 0x00007c00) && \
   (pf.dwGBitMask == 0x000003e0) && \
   (pf.dwBBitMask == 0x0000001f) && \
   (pf.dwAlphaBitMask == 0x00008000))

#define PF_IS_BGR565(pf) \
  ((pf.dwFlags & DDPF_RGB) && \
  !(pf.dwFlags & DDPF_ALPHAPIXELS) && \
   (pf.dwRGBBitCount == 16) && \
   (pf.dwRBitMask == 0x0000f800) && \
   (pf.dwGBitMask == 0x000007e0) && \
   (pf.dwBBitMask == 0x0000001f))

#define PF_IS_INDEX8(pf) \
  ((pf.dwFlags & DDPF_INDEXED) && \
   (pf.dwRGBBitCount == 8))
};