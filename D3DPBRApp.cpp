#include "StdAfx.h"
#include "D3DPBRApp.h"
#include "GeometryGenerator.h"
#include "d3dUtil.h"
#include "vertex.h"
#include "Effects.h"
#include "RenderStates.h"

D3DPBRApp::D3DPBRApp(HINSTANCE hInstance)
	:D3DApp(hInstance),m_pIB(0),m_pVB(0)
	,mDynamicCubeMapSRV(NULL)
	,mDynamicCubeMapDSV(NULL)
	,mLUTRTV(NULL)
	,mLUTSRV(NULL)
	,mLUTDSV(NULL)
{
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	mCam.SetPosition(0.0f, 2.0f, -15.0f);

	XMMATRIX MeshScale = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	XMMATRIX MeshOffset = XMMatrixTranslation(0.0f, 1.0f, 0.0f);
	BuildCubeFaceCamera(0.0f, 1.0f, 0.0f);
	XMStoreFloat4x4(&mMeshWorld, XMMatrixMultiply(MeshScale, MeshOffset));

	//mDirLights[0].Ambient  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[0].Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//mDirLights[0].Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLights[0].Direction = XMFLOAT3(0.f, 0.f, 1.0f);
	//mDirLights[0].Direction = XMFLOAT3(1.f, 0, 0);


	//mDirLights[1].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[1].Diffuse  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	//mDirLights[1].Specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	mDirLights[1].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	//mDirLights[2].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Diffuse  = XMFLOAT4(0.f, 0.f, 0.f, 1.0f);
	//mDirLights[2].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Direction = XMFLOAT3(0.0f, -0.707f, -0.707f);

	//mMeshMat.Ambient  = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	mMeshMat.Diffuse  = XMFLOAT4(1.0f, 0.86f, 0.57f, 1.0f);
	mMeshMat.Specular = 0.5f;
	mMeshMat.roughness = 0.5f;
	mMeshMat.metal = 0.9f;

	m_pSky = NULL;
	
}


D3DPBRApp::~D3DPBRApp(void)
{
	Effects::DestroyAll();
	InputLayouts::DestroyAll(); 
	SafeDelete(m_pSky);
	ReleaseCOM(mDynamicCubeMapDSV);
	ReleaseCOM(mDynamicCubeMapSRV);
	for(int i = 0; i < 6; ++i)
		ReleaseCOM(mDynamicCubeMapRTV[i]);
	ReleaseCOM(mLUTRTV);
	ReleaseCOM(mLUTSRV);
	ReleaseCOM(mLUTDSV);
}

void D3DPBRApp::DrawScene()
{
	//DrawEnvironment
	ID3D11RenderTargetView* rendertargets[1];
	md3dImmediateContext->RSSetViewports(1, &mCubeMapViewport);
	for (int i = 0; i < 6;i++)
	{
		md3dImmediateContext->ClearRenderTargetView(mDynamicCubeMapRTV[i], reinterpret_cast<const float*>(&Colors::Black));
		md3dImmediateContext->ClearDepthStencilView(mDynamicCubeMapDSV, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

		// Bind cube map face as render target.
		rendertargets[0] = mDynamicCubeMapRTV[i];
		md3dImmediateContext->OMSetRenderTargets(1, rendertargets, mDynamicCubeMapDSV);

		DrawEnvironment(mCubeMapCamera[i]);
	}
	md3dImmediateContext->GenerateMips(mDynamicCubeMapSRV);


	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Black));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	mCam.UpdateViewMatrix();
	DrawScene(mCam);

	HR(mSwapChain->Present(0, 0));
}

void D3DPBRApp::CreateLUT()
{
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = 512;
	texDesc.Height = 512;
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Format = DXGI_FORMAT_R16G16_FLOAT;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	ID3D11Texture2D* LUTTex = 0;
	HR(md3dDevice->CreateTexture2D(&texDesc, 0, &LUTTex));

	//
	// Create a render target view to each cube map face 
	// (i.e., each element in the texture array).
	// 

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;



	HR(md3dDevice->CreateRenderTargetView(LUTTex, &rtvDesc, &mLUTRTV));
	

	//
	// Create a shader resource view to the cube map.
	//

	//D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	//srvDesc.Format = texDesc.Format;
	//srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//srvDesc.Texture2D.MipLevels = 0;
	//srvDesc.Texture2D.MostDetailedMip = 0;


	//HR(md3dDevice->CreateShaderResourceView(LUTTex, &srvDesc, &mLUTSRV));





	D3D11_TEXTURE2D_DESC depthTexDesc;
	depthTexDesc.Width = 512;
	depthTexDesc.Height = 512;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 1;
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;

	ID3D11Texture2D* depthTex = 0;
	HR(md3dDevice->CreateTexture2D(&depthTexDesc, 0, &depthTex));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = depthTexDesc.Format;
	dsvDesc.Flags  = 0;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	HR(md3dDevice->CreateDepthStencilView(depthTex, &dsvDesc, &mLUTDSV));

	ReleaseCOM(depthTex);

	//
	// Viewport for drawing into cubemap.
	// 

	mLUTViewport.TopLeftX = 0.0f;
	mLUTViewport.TopLeftY = 0.0f;
	mLUTViewport.Width    = (float)512;
	mLUTViewport.Height   = (float)512;
	mLUTViewport.MinDepth = 0.0f;
	mLUTViewport.MaxDepth = 1.0f;



	GeometryGenerator::MeshData quad;
	GeometryGenerator geoGen;
	geoGen.CreateFullscreenQuad(quad);
	int indexNum = quad.Indices.size();
	
	std::vector<Vertex::Basic32> vertices(quad.Vertices.size());
	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i].Normal = quad.Vertices[i].Normal;
		vertices[i].Pos = quad.Vertices[i].Position;
		vertices[i].Tangent = quad.Vertices[i].TangentU;
		vertices[i].Tex = quad.Vertices[i].TexC;
	}

	D3D11_BUFFER_DESC dbd;
	dbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	dbd.ByteWidth = sizeof(Vertex::Basic32) * vertices.size();
	dbd.CPUAccessFlags = 0;
	dbd.StructureByteStride = 0;
	dbd.MiscFlags = 0;
	dbd.Usage = D3D11_USAGE_DEFAULT;
	D3D11_SUBRESOURCE_DATA vinitdata;
	vinitdata.pSysMem = &vertices[0];
	ID3D11Buffer* vb;
	HR(md3dDevice->CreateBuffer(&dbd, &vinitdata, &vb));

	std::vector<UINT> indeces(quad.Indices.begin(), quad.Indices.end());

	dbd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	dbd.ByteWidth = sizeof(UINT) * indexNum;
	vinitdata.pSysMem = &indeces[0];

	ID3D11Buffer* ib;
	HR(md3dDevice->CreateBuffer(&dbd, &vinitdata, &ib));

	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	md3dImmediateContext->ClearRenderTargetView(mLUTRTV, reinterpret_cast<const float*>(&Colors::Black));
	md3dImmediateContext->ClearDepthStencilView(mLUTDSV, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
	md3dImmediateContext->OMSetRenderTargets(1, &mLUTRTV, mLUTDSV);

	md3dImmediateContext->RSSetViewports(1, &mLUTViewport);
	md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);


	ID3DX11EffectTechnique* activeMeshTech = Effects::CommonPassFX->LUTTech;

	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;
	D3DX11_TECHNIQUE_DESC techDesc;
	activeMeshTech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		// Draw the Mesh.


		md3dImmediateContext->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);




		activeMeshTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(indexNum, 0, 0);

		//m_pSky->Draw(md3dImmediateContext, camera);

		// Restore default
		md3dImmediateContext->RSSetState(0);
	}

	ReleaseCOM(vb);
	ReleaseCOM(ib);
		
	D3DX11_IMAGE_FILE_FORMAT ift;
	
	HR(D3DX11SaveTextureToFile(md3dImmediateContext, LUTTex, D3DX11_IFF_DDS, _T("lut.dds")));
	ReleaseCOM(LUTTex);
}

void D3DPBRApp::UpdateScene(float dt)
{
	if( GetAsyncKeyState('W') & 0x8000 )
		mCam.Walk(10.0f*dt);

	if( GetAsyncKeyState('S') & 0x8000 )
		mCam.Walk(-10.0f*dt);

	if( GetAsyncKeyState('A') & 0x8000 )
		mCam.Strafe(-10.0f*dt);

	if( GetAsyncKeyState('D') & 0x8000 )
		mCam.Strafe(10.0f*dt);
}

void D3DPBRApp::BuildGeometry()
{
	GeometryGenerator::MeshData sphere;
	GeometryGenerator geoGen;
	geoGen.CreateSphere(5.f, 40, 40, sphere);
	m_indexnum = sphere.Indices.size();
	std::vector<Vertex::Basic32> vertices(sphere.Vertices.size());
	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i].Normal = sphere.Vertices[i].Normal;
		vertices[i].Pos = sphere.Vertices[i].Position;
		vertices[i].Tangent = sphere.Vertices[i].TangentU;
		vertices[i].Tex = sphere.Vertices[i].TexC;
	}

	D3D11_BUFFER_DESC dbd;
	dbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	dbd.ByteWidth = sizeof(Vertex::Basic32) * vertices.size();
	dbd.CPUAccessFlags = 0;
	dbd.StructureByteStride = 0;
	dbd.MiscFlags = 0;
	dbd.Usage = D3D11_USAGE_DEFAULT;
	D3D11_SUBRESOURCE_DATA vinitdata;
	vinitdata.pSysMem = &vertices[0];

	HR(md3dDevice->CreateBuffer(&dbd, &vinitdata, &m_pVB));

	std::vector<UINT> indeces(sphere.Indices.begin(), sphere.Indices.end());

	dbd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	dbd.ByteWidth = sizeof(UINT) * m_indexnum;
	vinitdata.pSysMem = &indeces[0];

	HR(md3dDevice->CreateBuffer(&dbd, &vinitdata, &m_pIB));
}

bool D3DPBRApp::Init()
{
	bool bRet = D3DApp::Init();
	if (bRet)
	{
		BuildGeometry();
	}
	Effects::InitAll(md3dDevice);
	InputLayouts::InitAll(md3dDevice);
	RenderStates::InitAll(md3dDevice);

	CreateLUT();

	m_pSky = new Sky(md3dDevice, L"Textures/snowcube1024.dds", 5000.0f);
	BuildDynamicCubeTexture();
	return bRet;
}

void D3DPBRApp::OnResize()
{
	D3DApp::OnResize();

	mCam.SetLens(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
}

void D3DPBRApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if( (btnState & MK_LBUTTON) != 0 )
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		mCam.Pitch(dy);
		mCam.RotateY(dx);
	}
	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void D3DPBRApp::BuildDynamicCubeTexture()
{
	//
	// Cubemap is a special texture array with 6 elements.
	//

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = CubeMapSize;
	texDesc.Height = CubeMapSize;
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 6;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;

	ID3D11Texture2D* cubeTex = 0;
	HR(md3dDevice->CreateTexture2D(&texDesc, 0, &cubeTex));

	//
	// Create a render target view to each cube map face 
	// (i.e., each element in the texture array).
	// 

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.ArraySize = 1;
	rtvDesc.Texture2DArray.MipSlice = 0;

	for(int i = 0; i < 6; ++i)
	{
		rtvDesc.Texture2DArray.FirstArraySlice = i;
		HR(md3dDevice->CreateRenderTargetView(cubeTex, &rtvDesc, &mDynamicCubeMapRTV[i]));
	}

	//
	// Create a shader resource view to the cube map.
	//

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = -1;

	HR(md3dDevice->CreateShaderResourceView(cubeTex, &srvDesc, &mDynamicCubeMapSRV));

	ReleaseCOM(cubeTex);

	//
	// We need a depth texture for rendering the scene into the cubemap
	// that has the same resolution as the cubemap faces.  
	//

	D3D11_TEXTURE2D_DESC depthTexDesc;
	depthTexDesc.Width = CubeMapSize;
	depthTexDesc.Height = CubeMapSize;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 1;
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;

	ID3D11Texture2D* depthTex = 0;
	HR(md3dDevice->CreateTexture2D(&depthTexDesc, 0, &depthTex));

	// Create the depth stencil view for the entire cube
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = depthTexDesc.Format;
	dsvDesc.Flags  = 0;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	HR(md3dDevice->CreateDepthStencilView(depthTex, &dsvDesc, &mDynamicCubeMapDSV));

	ReleaseCOM(depthTex);

	//
	// Viewport for drawing into cubemap.
	// 

	mCubeMapViewport.TopLeftX = 0.0f;
	mCubeMapViewport.TopLeftY = 0.0f;
	mCubeMapViewport.Width    = (float)CubeMapSize;
	mCubeMapViewport.Height   = (float)CubeMapSize;
	mCubeMapViewport.MinDepth = 0.0f;
	mCubeMapViewport.MaxDepth = 1.0f;


}

void D3DPBRApp::DrawEnvironment(Camera& camera)
{
	m_pSky->Draw(md3dImmediateContext, camera);
}

void D3DPBRApp::DrawScene(Camera& camera)
{
	md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);

	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;

	//

	XMMATRIX view     = camera.View();
	XMMATRIX proj     = camera.Proj();
	XMMATRIX viewProj = camera.ViewProj();




	// Set per frame constants.
	Effects::CommonPassFX->SetDirLights(mDirLights);
	Effects::CommonPassFX->SetEyePosW(mCam.GetPosition());
	Effects::CommonPassFX->SetEnvMap(mDynamicCubeMapSRV);

	ID3DX11EffectTechnique* activeMeshTech = Effects::CommonPassFX->Light1Tech;

	D3DX11_TECHNIQUE_DESC techDesc;
	activeMeshTech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		// Draw the Mesh.

		if( GetAsyncKeyState('1') & 0x8000 )
			md3dImmediateContext->RSSetState(RenderStates::WireframeRS);

		md3dImmediateContext->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);

		XMMATRIX world = XMLoadFloat4x4(&mMeshWorld);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*proj;

		Effects::CommonPassFX->SetWorld(world);
		Effects::CommonPassFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::CommonPassFX->SetWorldViewProj(worldViewProj);
		Effects::CommonPassFX->SetMaterial(mMeshMat);

		activeMeshTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(m_indexnum, 0, 0);

		//m_pSky->Draw(md3dImmediateContext, camera);

		// Restore default
		md3dImmediateContext->RSSetState(0);
	}

}

void D3DPBRApp::BuildCubeFaceCamera(float x, float y, float z)
{
	// Generate the cube map about the given position.
	XMFLOAT3 center(x, y, z);
	XMFLOAT3 worldUp(0.0f, 1.0f, 0.0f);

	// Look along each coordinate axis.
	XMFLOAT3 targets[6] = 
	{
		XMFLOAT3(x+1.0f, y, z), // +X
		XMFLOAT3(x-1.0f, y, z), // -X
		XMFLOAT3(x, y+1.0f, z), // +Y
		XMFLOAT3(x, y-1.0f, z), // -Y
		XMFLOAT3(x, y, z+1.0f), // +Z
		XMFLOAT3(x, y, z-1.0f)  // -Z
	};

	// Use world up vector (0,1,0) for all directions except +Y/-Y.  In these cases, we
	// are looking down +Y or -Y, so we need a different "up" vector.
	XMFLOAT3 ups[6] = 
	{
		XMFLOAT3(0.0f, 1.0f, 0.0f),  // +X
		XMFLOAT3(0.0f, 1.0f, 0.0f),  // -X
		XMFLOAT3(0.0f, 0.0f, -1.0f), // +Y
		XMFLOAT3(0.0f, 0.0f, +1.0f), // -Y
		XMFLOAT3(0.0f, 1.0f, 0.0f),	 // +Z
		XMFLOAT3(0.0f, 1.0f, 0.0f)	 // -Z
	};

	for(int i = 0; i < 6; ++i)
	{
		mCubeMapCamera[i].LookAt(center, targets[i], ups[i]);
		mCubeMapCamera[i].SetLens(0.5f*XM_PI, 1.0f, 0.1f, 1000.0f);
		mCubeMapCamera[i].UpdateViewMatrix();
	}
}
