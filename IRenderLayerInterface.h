#pragma once
#include "IBuffer.h"
class VertexShader;
class RenderState;
class PixelShader;
typedef unsigned int UINT;

class IRenderLayerInterface: public PBObject
{
public:
	IRenderLayerInterface(void);
	~IRenderLayerInterface(void);

	virtual bool InitDevice(void* Param) = 0;

	virtual bool SetVertexBuffer(IBuffer* pBuffer) = 0;

	virtual bool SetIndexBuffer(IBuffer* pBuffer) = 0;

	virtual VertexShader* CreateVertexShader(const char* filename) = 0;

	virtual PixelShader* CreatePixelShader(const char* filename) = 0;

	virtual bool SetVertexShader(VertexShader* shader) = 0;

	virtual bool SetPixelShader(VertexShader* shader) = 0;

	virtual void SetRenderState(RenderState* state) = 0;

};

