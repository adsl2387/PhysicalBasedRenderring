#pragma once
#include "pbobject.h"
typedef enum PB_Format
{
	PB_FORMAT_UNKNOWN	                    = 0,
	PB_FORMAT_R32G32B32A32_TYPELESS       = 1,
	PB_FORMAT_R32G32B32A32_FLOAT          = 2,
	PB_FORMAT_R32G32B32A32_UINT           = 3,
	PB_FORMAT_R32G32B32A32_SINT           = 4,
	PB_FORMAT_R32G32B32_TYPELESS          = 5,
	PB_FORMAT_R32G32B32_FLOAT             = 6,
	PB_FORMAT_R32G32B32_UINT              = 7,
	PB_FORMAT_R32G32B32_SINT              = 8,
	PB_FORMAT_R16G16B16A16_TYPELESS       = 9,
	PB_FORMAT_R16G16B16A16_FLOAT          = 10,
	PB_FORMAT_R16G16B16A16_UNORM          = 11,
	PB_FORMAT_R16G16B16A16_UINT           = 12,
	PB_FORMAT_R16G16B16A16_SNORM          = 13,
	PB_FORMAT_R16G16B16A16_SINT           = 14,
	PB_FORMAT_R32G32_TYPELESS             = 15,
	PB_FORMAT_R32G32_FLOAT                = 16,
	PB_FORMAT_R32G32_UINT                 = 17,
	PB_FORMAT_R32G32_SINT                 = 18,
	PB_FORMAT_R32G8X24_TYPELESS           = 19,
	PB_FORMAT_D32_FLOAT_S8X24_UINT        = 20,
	PB_FORMAT_R32_FLOAT_X8X24_TYPELESS    = 21,
	PB_FORMAT_X32_TYPELESS_G8X24_UINT     = 22,
	PB_FORMAT_R10G10B10A2_TYPELESS        = 23,
	PB_FORMAT_R10G10B10A2_UNORM           = 24,
	PB_FORMAT_R10G10B10A2_UINT            = 25,
	PB_FORMAT_R11G11B10_FLOAT             = 26,
	PB_FORMAT_R8G8B8A8_TYPELESS           = 27,
	PB_FORMAT_R8G8B8A8_UNORM              = 28,
	PB_FORMAT_R8G8B8A8_UNORM_SRGB         = 29,
	PB_FORMAT_R8G8B8A8_UINT               = 30,
	PB_FORMAT_R8G8B8A8_SNORM              = 31,
	PB_FORMAT_R8G8B8A8_SINT               = 32,
	PB_FORMAT_R16G16_TYPELESS             = 33,
	PB_FORMAT_R16G16_FLOAT                = 34,
	PB_FORMAT_R16G16_UNORM                = 35,
	PB_FORMAT_R16G16_UINT                 = 36,
	PB_FORMAT_R16G16_SNORM                = 37,
	PB_FORMAT_R16G16_SINT                 = 38,
	PB_FORMAT_R32_TYPELESS                = 39,
	PB_FORMAT_D32_FLOAT                   = 40,
	PB_FORMAT_R32_FLOAT                   = 41,
	PB_FORMAT_R32_UINT                    = 42,
	PB_FORMAT_R32_SINT                    = 43,
	PB_FORMAT_R24G8_TYPELESS              = 44,
	PB_FORMAT_D24_UNORM_S8_UINT           = 45,
	PB_FORMAT_R24_UNORM_X8_TYPELESS       = 46,
	PB_FORMAT_X24_TYPELESS_G8_UINT        = 47,
	PB_FORMAT_R8G8_TYPELESS               = 48,
	PB_FORMAT_R8G8_UNORM                  = 49,
	PB_FORMAT_R8G8_UINT                   = 50,
	PB_FORMAT_R8G8_SNORM                  = 51,
	PB_FORMAT_R8G8_SINT                   = 52,
	PB_FORMAT_R16_TYPELESS                = 53,
	PB_FORMAT_R16_FLOAT                   = 54,
	PB_FORMAT_D16_UNORM                   = 55,
	PB_FORMAT_R16_UNORM                   = 56,
	PB_FORMAT_R16_UINT                    = 57,
	PB_FORMAT_R16_SNORM                   = 58,
	PB_FORMAT_R16_SINT                    = 59,
	PB_FORMAT_R8_TYPELESS                 = 60,
	PB_FORMAT_R8_UNORM                    = 61,
	PB_FORMAT_R8_UINT                     = 62,
	PB_FORMAT_R8_SNORM                    = 63,
	PB_FORMAT_R8_SINT                     = 64,
	PB_FORMAT_A8_UNORM                    = 65,
	PB_FORMAT_R1_UNORM                    = 66,
	PB_FORMAT_R9G9B9E5_SHAREDEXP          = 67,
	PB_FORMAT_R8G8_B8G8_UNORM             = 68,
	PB_FORMAT_G8R8_G8B8_UNORM             = 69,
	PB_FORMAT_BC1_TYPELESS                = 70,
	PB_FORMAT_BC1_UNORM                   = 71,
	PB_FORMAT_BC1_UNORM_SRGB              = 72,
	PB_FORMAT_BC2_TYPELESS                = 73,
	PB_FORMAT_BC2_UNORM                   = 74,
	PB_FORMAT_BC2_UNORM_SRGB              = 75,
	PB_FORMAT_BC3_TYPELESS                = 76,
	PB_FORMAT_BC3_UNORM                   = 77,
	PB_FORMAT_BC3_UNORM_SRGB              = 78,
	PB_FORMAT_BC4_TYPELESS                = 79,
	PB_FORMAT_BC4_UNORM                   = 80,
	PB_FORMAT_BC4_SNORM                   = 81,
	PB_FORMAT_BC5_TYPELESS                = 82,
	PB_FORMAT_BC5_UNORM                   = 83,
	PB_FORMAT_BC5_SNORM                   = 84,
	PB_FORMAT_B5G6R5_UNORM                = 85,
	PB_FORMAT_B5G5R5A1_UNORM              = 86,
	PB_FORMAT_B8G8R8A8_UNORM              = 87,
	PB_FORMAT_B8G8R8X8_UNORM              = 88,
	PB_FORMAT_R10G10B10_XR_BIAS_A2_UNORM  = 89,
	PB_FORMAT_B8G8R8A8_TYPELESS           = 90,
	PB_FORMAT_B8G8R8A8_UNORM_SRGB         = 91,
	PB_FORMAT_B8G8R8X8_TYPELESS           = 92,
	PB_FORMAT_B8G8R8X8_UNORM_SRGB         = 93,
	PB_FORMAT_BC6H_TYPELESS               = 94,
	PB_FORMAT_BC6H_UF16                   = 95,
	PB_FORMAT_BC6H_SF16                   = 96,
	PB_FORMAT_BC7_TYPELESS                = 97,
	PB_FORMAT_BC7_UNORM                   = 98,
	PB_FORMAT_BC7_UNORM_SRGB              = 99,
	PB_FORMAT_FORCE_UINT                  = 0xffffffff
};

typedef enum PB_Usage
{
	PB_USAGE_DEFAULT	= 0,
	PB_USAGE_IMMUTABLE	= 1,
	PB_USAGE_DYNAMIC	= 2,
	PB_USAGE_STAGING	= 3
};

typedef enum PB_BindType
{
	PB_BIND_VERTEX_BUFFER	= 0x1L,
	PB_BIND_INDEX_BUFFER	= 0x2L,
	PB_BIND_CONSTANT_BUFFER	= 0x4L,
	PB_BIND_SHADER_RESOURCE	= 0x8L,
	PB_BIND_STREAM_OUTPUT	= 0x10L,
	PB_BIND_RENDER_TARGET	= 0x20L,
	PB_BIND_DEPTH_STENCIL	= 0x40L,
	PB_BIND_UNORDERED_ACCESS	= 0x80L
};

typedef enum PB_MiscType
{
	PB_RESOURCE_MISC_GENERATE_MIPS	= 0x1L,
	PB_RESOURCE_MISC_SHARED	= 0x2L,
	PB_RESOURCE_MISC_TEXTURECUBE	= 0x4L,
	PB_RESOURCE_MISC_DRAWINDIRECT_ARGS	= 0x10L,
	PB_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS	= 0x20L,
	PB_RESOURCE_MISC_BUFFER_STRUCTURED	= 0x40L,
	PB_RESOURCE_MISC_RESOURCE_CLAMP	= 0x80L,
	PB_RESOURCE_MISC_SHARED_KEYEDMUTEX	= 0x100L,
	PB_RESOURCE_MISC_GDI_COMPATIBLE	= 0x200L
};

struct TextureInfo
{
	int Width;
	int Height;
	PB_Format Format;
	UINT Usage;
	UINT BindFlag;
	UINT MiscFlag;
};

class ITexture :
	public PBObject
{
public:
	ITexture(void);
	~ITexture(void);
};

class D3D11Texture : public ITexture
{
public:
	D3D11Texture();

	D3D11Texture(ID3D11Texture2D* Texture2D);

	virtual ~D3D11Texture();

	ID3D11Texture2D* GetTexture2D();
protected:
	ID3D11Texture2D* D3D11Texture2D;
};


class D3D11RenderTargetView:public D3D11Texture
{
public:
	D3D11RenderTargetView();

	virtual ~D3D11RenderTargetView();

	ID3D11RenderTargetView* GetRenderTargetView();

protected:
	ID3D11RenderTargetView*	RenderTargetView;
};

class D3D11DepthStencilView:public D3D11Texture
{
public:
	D3D11DepthStencilView();

	virtual ~D3D11DepthStencilView();

	ID3D11DepthStencilView* GetDepthStencilView();

protected:

	ID3D11DepthStencilView* DepthStencilView;
};

class D3D11ShaderResourceView:public D3D11Texture
{
public:

	D3D11ShaderResourceView();

	virtual ~D3D11ShaderResourceView();


	ID3D11ShaderResourceView* GetShaderResourceView();
protected:
	ID3D11ShaderResourceView* ShaderResourceView;
};