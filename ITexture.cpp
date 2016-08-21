#include "StdAfx.h"
#include "ITexture.h"


ITexture::ITexture(void)
{
}


ITexture::~ITexture(void)
{
}

D3D11Texture::D3D11Texture(ID3D11Texture2D* Texture2D)
{

}

D3D11Texture::D3D11Texture()
{

}

D3D11Texture::~D3D11Texture()
{

}

ID3D11Texture2D* D3D11Texture::GetTexture2D()
{
	return NULL;
}

ID3D11RenderTargetView* D3D11RenderTargetView::GetRenderTargetView()
{
	return NULL;
}

ID3D11DepthStencilView* D3D11DepthStencilView::GetDepthStencilView()
{
	return NULL;
}

ID3D11ShaderResourceView* D3D11ShaderResourceView::GetShaderResourceView()
{
	return NULL;
}
