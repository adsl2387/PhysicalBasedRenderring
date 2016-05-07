#pragma once
#include "irenderlayerinterface.h"
#include "IBuffer.h"
#include "ITexture.h"
#include "IViewport.h"
#include "d3dUtil.h"

class D3dRenderLayer :
	public IRenderLayerInterface
{
public:
	D3dRenderLayer(void);
	~D3dRenderLayer(void);

	virtual bool InitDevice(void* Param, int nClientWidth, int nClientHeight);

	virtual bool OnResize(int nWidth, int nHeight);

	virtual bool SetVertexBuffer(IBuffer* pBuffer) ;

	virtual bool SetIndexBuffer(IBuffer* pBuffer) ;

	virtual VertexShader* CreateVertexShader(const char* filename);

	virtual PixelShader* CreatePixelShader(const char* filename);

	virtual bool SetVertexShader(VertexShader* shader);

	virtual bool SetPixelShader(PixelShader* shader);

	virtual void SetRenderState(RenderState* state);

	virtual ITexture* CreateTexture(const TextureInfo& textureinfo);

	virtual void Present();

	virtual void ClearRenderTarget(D3D11RenderTargetView* RenderTarget, const float* rgba);

	virtual void ClearDepthStencil(D3D11DepthStencilView* DepthStencil, float Depth, UINT8 Stencil);

	virtual void SetViewport(D3D11Viewport* D3D11Viewport);

private:
	ID3D11Device* md3dDevice;
	
	ID3D11DeviceContext* md3dImmediateContext;
	
	IDXGISwapChain* mSwapChain;
	
	ID3D11Texture2D* mDepthStencilBuffer;
	
	ID3D11RenderTargetView* mRenderTargetView;
	
	ID3D11DepthStencilView* mDepthStencilView;
	
	D3D11_VIEWPORT mScreenViewport;

	D3D_DRIVER_TYPE md3dDriverType;

	bool			mEnable4xMsaa;

	UINT			m4xMsaaQuality;

	int				mClientWidth;

	int				mClientHeight;
};

