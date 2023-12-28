#include "Alice.h"

#include<cassert>

void Allice::Initialize() {

	ImportSettings importS = {
		modelFile,
		meshes,
		false,
		true,
	};

	AssimpLoader loader;
	if (!loader.Load(importS)) {
		assert(false);
	}

}

void Allice::Update() {

	
	meshes;

}
