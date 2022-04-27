#pragma once
//#include "Camera.h"
#include"vector"
class Sprite
{
private:
	unsigned int _totalFrames, _currentFrame;
	int _animationSpeed;
	std::vector<RECT> _bounds;
	//Camera* _cameraInstance;
	DWORD _lastFrameTime;
	Texture* _texture;
	D3DXMATRIX _scaleMatrix;
	D3DX10_SPRITE _sprite;
	void _ScaleSprite(const RECT&, D3DXVECTOR2, unsigned int);
public:
	Sprite(Texture*&, RECT, unsigned int, int);
	~Sprite();
	void AddSpriteBound(RECT);
	void DrawSprite(D3DXVECTOR2, D3DXVECTOR2, unsigned int);
	void Release();
};

