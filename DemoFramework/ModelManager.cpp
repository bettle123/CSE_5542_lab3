#include "ModelManager.h"
#include "Matrix.h"
using namespace Crawfis::Graphics;

bool Crawfis::Graphics::ModelManager::created = false;
Crawfis::Graphics::ModelManager* Crawfis::Graphics::ModelManager::instance = 0;
// 12/02
Matrix4 Crawfis::Graphics::ModelManager::depthMVP;