#pragma once
//
#include "../GlobalUtil.h"
#include "../Background.h"
#include "../Camera.h"

#include"vector"
#include"string"
#include"fstream"
#include"unordered_map"

class Scene
{
public:
	enum class SceneType {
		SCENE_TYPE_INTRO = 0,
		SCENE_TYPE_SECRET = 1,
		SCENE_TYPE_MAP = 10,
		SCENE_TYPE_STAGE_ONE = 11,
		SCENE_TYPE_STAGE_FOUR = 14,
		SCENE_TYPE_STAGE_FORTRESS = 18
	};
protected:
	enum class _SceneFileSection {
		SCENEFILE_SECTION_UNKNOWN,
		SCENEFILE_SECTION_MAINTHEMES,
		SCENEFILE_SECTION_SCENESIZE,
		SCENEFILE_SECTION_SCENETIME,
		SCENEFILE_SECTION_CAMERALOCKVALUE,
		SCENEFILE_SECTION_CAMERABOUNDS,
		SCENEFILE_SECTION_BGCOLOR,
		SCENEFILE_SECTION_TEXTURES,
		SCENEFILE_SECTION_ENTITYDATA,
		SCENEFILE_SECTION_TILEDATA,
		SCENEFILE_SECTION_GRID,
		SCENEFILE_SECTION_HUD,
		SCENEFILE_SECTION_MAINEFFECT,
		SCENEFILE_SECTION_BACKGROUND
	};

	SceneType _sceneID;
	unsigned int _sceneWidth;
	unsigned int _sceneHeight;
	unsigned int _currentThemeID;

	float _lockValue;

	std::vector<unsigned int> _mainThemeIDs;

	std::string _filePath;
	/*std::vector<Entity*> _entities;
	std::vector<Entity*> _tiles;*/

	std::unordered_map<unsigned int, Texture*> _textureMap;

	D3DXCOLOR _backgroundColor;
	DWORD _sceneTime;
	DWORD _toSceneStart;
	DWORD _toSceneTime;

	/*Camera* _cameraInstance*/;

	Background* _background;

	/*HUD* _hud;
	ScorePopUp* _scorePopUp;

	Player* _player;
	PropPlayer* _propMario;
	PropPlayer* _propLuigi;

	SelectText* _selectText;

	Grid* _grid;*/

	//bool _IsEntityInViewport(Entity*, RECTF) const;
	//bool _IsEntityAliveAndIB(Entity*) const;
	unsigned int _GetNextThemeID();

	Texture* _LoadTexture(LPCWSTR);

	void _ParseMainThemes(std::string);
	void _ParseSceneSize(std::string);
	void _ParseSceneTime(std::string);
	void _ParseCameraLockValue(std::string);
	void _ParseCameraBounds(std::string);
	void _ParseBackgroundColor(std::string);
	void _ParseTextures(std::string);
	void _ParseEntityData(std::string);
	void _ParseTileData(std::string);
	void _ParseGrid(std::string);
	void _ParseHUD(std::string);
	void _ParseMainEffect(std::string);
	void _ParseBackground(std::string);

public:
	Scene(SceneType, std::string);
	virtual ~Scene();

	bool IsTransitioningToScene() const;
	void StartToSceneTimer();

	SceneType GetSceneID() const;
	unsigned int GetSceneWidth() const;
	unsigned int GetSceneHeight() const;

	D3DXCOLOR GetBGColor() const;

	Texture* GetTexture(unsigned int) const;

	virtual void HandleStates();
	virtual void OnKeyUp(int);
	virtual void OnKeyDown(int);

	//void AddEntityToScene(Entity*);
	//void RemoveEntityFromScene(Entity*);

	//Entity* CreateEntityFromData(std::string, std::string, std::string);

	virtual void LoadScene();
	virtual void UpdateCameraPosition();
	virtual void Update(DWORD);
	virtual void Render();
	virtual void Release();
};

