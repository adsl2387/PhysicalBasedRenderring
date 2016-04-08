#pragma once
#include "PBObject.h"
#include "VertexShader.h"
#include <map>
using namespace std;

class ShaderFactory: public PBObject
{
public:
	ShaderFactory(void);
	~ShaderFactory(void);

	static ShaderFactory* GetSingleton();

	VertexShader* CreateShaderFromFile(const char* filename);

	void ReleaseShader(VertexShader* shader);

private:


	map<char*, VertexShader*> ShaderMap;
};

