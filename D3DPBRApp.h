#pragma once
#include "d3dapp.h"
#include "Camera.h"
#include "Sky.h"
class D3DPBRApp :
	public D3DApp
{
public:
	D3DPBRApp(HINSTANCE hInstance);
	virtual ~D3DPBRApp(void);

	virtual void UpdateScene(float dt);
	virtual void DrawScene(); 
	virtual bool Init();
	virtual void OnResize();
	//virtual void OnMouseDown(WPARAM btnState, int x, int y);
	//virtual void OnMouseUp(WPARAM btnState, int x, int y) ;
	virtual void OnMouseMove(WPARAM btnState, int x, int y);
protected:
	void BuildDynamicCubeTexture();

	void BuildGeometry();

	void DrawEnvironment(Camera& camera);

	void BuildCubeFaceCamera(float x, float y, float z);

	void DrawScene(Camera& camera);

	void CreateLUT();
private:
	PBRDirectionalLight mDirLights[3];
	PBRMaterial mMeshMat;
	XMFLOAT4X4 mMeshWorld;
	POINT mLastMousePos;

	Sky* m_pSky;
	ID3D11RenderTargetView*	mDynamicCubeMapRTV[6];
	ID3D11ShaderResourceView* mDynamicCubeMapSRV;
	ID3D11DepthStencilView* mDynamicCubeMapDSV;
	ID3D11RenderTargetView* mLUTRTV;
	ID3D11ShaderResourceView* mLUTSRV;
	ID3D11DepthStencilView* mLUTDSV;
	ID3D11Buffer* m_pVB;
	ID3D11Buffer* m_pIB;
	int m_indexnum;
	D3D11_VIEWPORT mCubeMapViewport;
	D3D11_VIEWPORT mLUTViewport;
	Camera mCubeMapCamera[6];
	Camera mCam;

	static const int CubeMapSize = 512;
};

