#pragma once

#define DIRECTINPUT_VERSION 0x0800

#include<vector>
#include<string>
#include<codecvt>

#include<d3d10.h>
#include<D3DX10.h>

#include<dinput.h>
#include<Windows.h>

namespace GlobalUtil
{
	const unsigned int SCREEN_WIDTH = 256;
	const unsigned int SCREEN_HEIGHT = 224;

	const unsigned int MAX_FILE_LINE = 5000;

	extern ID3D10Device* directDevice;
	extern ID3DX10Sprite* spriteHandler;

	extern ID3DX10Font* font;

	extern void ToUpper(std::string&);
	extern void ToLower(std::string&);

	extern void GetFontRect(LPCSTR, RECT*);
	extern void WriteTextToScreen(LPCSTR, RECT*, UINT, D3DXCOLOR);

	extern std::vector<std::string>SplitStr(std::string, std::string = "\t");

	extern std::vector<unsigned int> SplitDigit(unsigned int);

	extern LPCWSTR ToLPCWSTR(std::string);

	extern std::string ToStr(std::string&);
};

//typedef struct RectFloat                                                                                              

