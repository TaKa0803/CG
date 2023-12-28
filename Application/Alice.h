#pragma once

#include<vector>
#include"AssimpManager/AssimpManager.h"

class Allice {

public:

	void Initialize();

	void Update();

private:

	const wchar_t* modelFile = L"resources/Alicia/FBX/Alicia_solid_Unity.FBX";

	std::vector<Mesh>meshes;

};