#pragma once
#include"Sprite.h"
#include"string"
#include<unorderd_map>
class AnimatedSprite
{
private:
	std::unordered_map<std::string, Sprite*> _sprite;
	bool _HasAnimation(std::string) const;
public:
	void ParseSprites(std::string, Texture*&);
	void PlaySpriteAnimation(std::string, D3DXVECTOR2, D3DXVECTOR2 = D3DXVECTOR2(1.0f, 1.0f), unsigned int = 255);
	void Release();
};

