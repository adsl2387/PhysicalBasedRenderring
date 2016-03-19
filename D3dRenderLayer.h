#pragma once
#include "irenderlayerinterface.h"

class D3dRenderLayer :
	public IRenderLayerInterface
{
public:
	D3dRenderLayer(void);
	~D3dRenderLayer(void);

	virtual bool InitDevice();

	virtual bool SetVertexBuffer(void* pBuffer) ;

	virtual bool SetIndexBuffer(void* pBuffer) ;

	virtual Shader* CreateAndCompileShader(const char* filename);

	virtual bool SetVertexShader(Shader* shader);

	virtual bool SetPixelShader(Shader* shader);

	virtual void SetRenderState(RenderState* state);
};

