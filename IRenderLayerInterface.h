#pragma once

class Shader;
class RenderState;
typedef unsigned int UINT;

class IRenderLayerInterface
{
public:
	IRenderLayerInterface(void);
	~IRenderLayerInterface(void);

	virtual bool InitDevice() = 0;

	virtual bool SetVertexBuffer(void* pBuffer) = 0;

	virtual bool SetIndexBuffer(void* pBuffer) = 0;

	virtual Shader* CreateAndCompileShader(const char* filename) = 0;

	virtual bool SetVertexShader(Shader* shader) = 0;

	virtual bool SetPixelShader(Shader* shader) = 0;

	virtual void SetRenderState(RenderState* state) = 0;

};

