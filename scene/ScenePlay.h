#pragma once

class ScenePlay : public Scene {
private:
	std::vector<Entity*> _removedEntities;

public:
	ScenePlay(SceneType, std::string);
	~ScenePlay();

	void HandleStates() override;
	void OnKeyUp(int) override;
	void OnKeyDown(int) override;

	void LoadScene() override;

	void UpdateCameraPosition() override;

	void Update(DWORD) override;
	void Render() override;

	void Release() override;
};
