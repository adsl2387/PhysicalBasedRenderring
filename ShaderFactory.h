#pragma once
#include "PBObject.h"
#include "Shader.h"
#include <map>
using namespace std;

class ShaderFactory: public PBObject
{
public:
	ShaderFactory(void);
	~ShaderFactory(void);

	static ShaderFactory* GetSingleton();

	Shader* CreateShaderFromFile(const char* filename);

	void ReleaseShader(Shader* shader);

private:


	map<char*, Shader*> ShaderMap;
};

