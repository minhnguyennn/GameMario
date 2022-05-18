#include <random>

#include "../Game.h"
#include "../SceneManager.h"
#include "Scene.h"
#include "ScenePlay.h"

ScenePlay::ScenePlay(SceneType sceneID, std::string path) : Scene(sceneID, path) {}

ScenePlay::~ScenePlay() {}

void ScenePlay::HandleStates() {
	_player->HandleStates();
}

void ScenePlay::OnKeyUp(int keyCode) {
	_player->OnKeyUpGame(keyCode);
}

void ScenePlay::OnKeyDown(int keyCode) {
	_player->OnKeyDownGame(keyCode);
}

void ScenePlay::LoadScene() {
	Scene::LoadScene();
}

void ScenePlay::UpdateCameraPosition() {}

void ScenePlay::Update(DWORD deltaTime) {
	for (size_t i = 0; i < _entities.size(); ++i) {
		Entity* entity = _entities.at(i);
		entity->Update(deltaTime);
	}
	
	_background->Update();
}

void ScenePlay::Render() {
	for (size_t i = 0; i < _entities.size(); ++i) {
		Entity* entity = _entities.at(i);
		entity->Render();
	}

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

	for (auto& tile : _tiles) {
		tile->Release();
		delete tile;
	}
	_tiles.clear();

	for (unsigned int i = 0; i < _entities.size(); ++i) {
		_entities.at(i)->Release();
		delete _entities.at(i);
	}
	_entities.clear();

	for (auto& texture : _textureMap) {
		texture.second->Release();
		delete texture.second;
	}
	_textureMap.clear();

	if (_cameraInstance != nullptr) {
		_cameraInstance->Release();
	}

	sprintf_s(debug, "[SCENE] Unloaded scene with ID: %d\n", _sceneID);
	OutputDebugStringA(debug);
}
