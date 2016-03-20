#pragma once
#include "pbobject.h"
typedef enum PB_Format
{
	PBGI_FORMAT_UNKNOWN	                    = 0,
	PBGI_FORMAT_R32G32B32A32_TYPELESS       = 1,
	PBGI_FORMAT_R32G32B32A32_FLOAT          = 2,
	PBGI_FORMAT_R32G32B32A32_UINT           = 3,
	PBGI_FORMAT_R32G32B32A32_SINT           = 4,
	PBGI_FORMAT_R32G32B32_TYPELESS          = 5,
	PBGI_FORMAT_R32G32B32_FLOAT             = 6,
	PBGI_FORMAT_R32G32B32_UINT              = 7,
	PBGI_FORMAT_R32G32B32_SINT              = 8,
	PBGI_FORMAT_R16G16B16A16_TYPELESS       = 9,
	PBGI_FORMAT_R16G16B16A16_FLOAT          = 10,
	PBGI_FORMAT_R16G16B16A16_UNORM          = 11,
	PBGI_FORMAT_R16G16B16A16_UINT           = 12,
	PBGI_FORMAT_R16G16B16A16_SNORM          = 13,
	PBGI_FORMAT_R16G16B16A16_SINT           = 14,
	PBGI_FORMAT_R32G32_TYPELESS             = 15,
	PBGI_FORMAT_R32G32_FLOAT                = 16,
	PBGI_FORMAT_R32G32_UINT                 = 17,
	PBGI_FORMAT_R32G32_SINT                 = 18,
	PBGI_FORMAT_R32G8X24_TYPELESS           = 19,
	PBGI_FORMAT_D32_FLOAT_S8X24_UINT        = 20,
	PBGI_FORMAT_R32_FLOAT_X8X24_TYPELESS    = 21,
	PBGI_FORMAT_X32_TYPELESS_G8X24_UINT     = 22,
	PBGI_FORMAT_R10G10B10A2_TYPELESS        = 23,
	PBGI_FORMAT_R10G10B10A2_UNORM           = 24,
	PBGI_FORMAT_R10G10B10A2_UINT            = 25,
	PBGI_FORMAT_R11G11B10_FLOAT             = 26,
	PBGI_FORMAT_R8G8B8A8_TYPELESS           = 27,
	PBGI_FORMAT_R8G8B8A8_UNORM              = 28,
	PBGI_FORMAT_R8G8B8A8_UNORM_SRGB         = 29,
	PBGI_FORMAT_R8G8B8A8_UINT               = 30,
	PBGI_FORMAT_R8G8B8A8_SNORM              = 31,
	PBGI_FORMAT_R8G8B8A8_SINT               = 32,
	PBGI_FORMAT_R16G16_TYPELESS             = 33,
	PBGI_FORMAT_R16G16_FLOAT                = 34,
	PBGI_FORMAT_R16G16_UNORM                = 35,
	PBGI_FORMAT_R16G16_UINT                 = 36,
	PBGI_FORMAT_R16G16_SNORM                = 37,
	PBGI_FORMAT_R16G16_SINT                 = 38,
	PBGI_FORMAT_R32_TYPELESS                = 39,
	PBGI_FORMAT_D32_FLOAT                   = 40,
	PBGI_FORMAT_R32_FLOAT                   = 41,
	PBGI_FORMAT_R32_UINT                    = 42,
	PBGI_FORMAT_R32_SINT                    = 43,
	PBGI_FORMAT_R24G8_TYPELESS              = 44,
	PBGI_FORMAT_D24_UNORM_S8_UINT           = 45,
	PBGI_FORMAT_R24_UNORM_X8_TYPELESS       = 46,
	PBGI_FORMAT_X24_TYPELESS_G8_UINT        = 47,
	PBGI_FORMAT_R8G8_TYPELESS               = 48,
	PBGI_FORMAT_R8G8_UNORM                  = 49,
	PBGI_FORMAT_R8G8_UINT                   = 50,
	PBGI_FORMAT_R8G8_SNORM                  = 51,
	PBGI_FORMAT_R8G8_SINT                   = 52,
	PBGI_FORMAT_R16_TYPELESS                = 53,
	PBGI_FORMAT_R16_FLOAT                   = 54,
	PBGI_FORMAT_D16_UNORM                   = 55,
	PBGI_FORMAT_R16_UNORM                   = 56,
	PBGI_FORMAT_R16_UINT                    = 57,
	PBGI_FORMAT_R16_SNORM                   = 58,
	PBGI_FORMAT_R16_SINT                    = 59,
	PBGI_FORMAT_R8_TYPELESS                 = 60,
	PBGI_FORMAT_R8_UNORM                    = 61,
	PBGI_FORMAT_R8_UINT                     = 62,
	PBGI_FORMAT_R8_SNORM                    = 63,
	PBGI_FORMAT_R8_SINT                     = 64,
	PBGI_FORMAT_A8_UNORM                    = 65,
	PBGI_FORMAT_R1_UNORM                    = 66,
	PBGI_FORMAT_R9G9B9E5_SHAREDEXP          = 67,
	PBGI_FORMAT_R8G8_B8G8_UNORM             = 68,
	PBGI_FORMAT_G8R8_G8B8_UNORM             = 69,
	PBGI_FORMAT_BC1_TYPELESS                = 70,
	PBGI_FORMAT_BC1_UNORM                   = 71,
	PBGI_FORMAT_BC1_UNORM_SRGB              = 72,
	PBGI_FORMAT_BC2_TYPELESS                = 73,
	PBGI_FORMAT_BC2_UNORM                   = 74,
	PBGI_FORMAT_BC2_UNORM_SRGB              = 75,
	PBGI_FORMAT_BC3_TYPELESS                = 76,
	PBGI_FORMAT_BC3_UNORM                   = 77,
	PBGI_FORMAT_BC3_UNORM_SRGB              = 78,
	PBGI_FORMAT_BC4_TYPELESS                = 79,
	PBGI_FORMAT_BC4_UNORM                   = 80,
	PBGI_FORMAT_BC4_SNORM                   = 81,
	PBGI_FORMAT_BC5_TYPELESS                = 82,
	PBGI_FORMAT_BC5_UNORM                   = 83,
	PBGI_FORMAT_BC5_SNORM                   = 84,
	PBGI_FORMAT_B5G6R5_UNORM                = 85,
	PBGI_FORMAT_B5G5R5A1_UNORM              = 86,
	PBGI_FORMAT_B8G8R8A8_UNORM              = 87,
	PBGI_FORMAT_B8G8R8X8_UNORM              = 88,
	PBGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM  = 89,
	PBGI_FORMAT_B8G8R8A8_TYPELESS           = 90,
	PBGI_FORMAT_B8G8R8A8_UNORM_SRGB         = 91,
	PBGI_FORMAT_B8G8R8X8_TYPELESS           = 92,
	PBGI_FORMAT_B8G8R8X8_UNORM_SRGB         = 93,
	PBGI_FORMAT_BC6H_TYPELESS               = 94,
	PBGI_FORMAT_BC6H_UF16                   = 95,
	PBGI_FORMAT_BC6H_SF16                   = 96,
	PBGI_FORMAT_BC7_TYPELESS                = 97,
	PBGI_FORMAT_BC7_UNORM                   = 98,
	PBGI_FORMAT_BC7_UNORM_SRGB              = 99,
	PBGI_FORMAT_FORCE_UINT                  = 0xffffffff
};

typedef enum PB_Usage
{

};

struct TextureInfo
{
	int Width;
	int Height;
	PB_Format Format;

};

class ITexture :
	public PBObject
{
public:
	ITexture(void);
	~ITexture(void);
};

