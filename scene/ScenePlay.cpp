#include <random>

#include "../Game.h"
#include "../SceneManager.h"
#include "Scene.h"
#include "ScenePlay.h"
//#include "../EntityList.h"

ScenePlay::ScenePlay(SceneType sceneID, std::string path) : Scene(sceneID, path) {}

ScenePlay::~ScenePlay() {}

void ScenePlay::HandleStates()
{
}

void ScenePlay::OnKeyUp(int)
{
}

void ScenePlay::OnKeyDown(int)
{
}

void ScenePlay::LoadScene() {
	Scene::LoadScene();
}

void ScenePlay::UpdateCameraPosition()
{
}

void ScenePlay::Update(DWORD deltaTime) {
	_background->Update();
}

void ScenePlay::Render() {
	_background->BruteForceRender();
}

void ScenePlay::Release() {
	char debug[100];
	sprintf_s(debug, "[SCENE] Unloading scene with ID: %d\n", _sceneID);
	OutputDebugStringA(debug);

	if (_background != nullptr) {
		_background->Release();
		delete _background;
	}

	for (auto& texture : _textureMap) {
		texture.second->Release();
		delete texture.second;
	}
	_textureMap.clear();

	sprintf_s(debug, "[SCENE] Unloaded scene with ID: %d\n", _sceneID);
	OutputDebugStringA(debug);
}
