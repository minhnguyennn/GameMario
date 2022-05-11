#include <fstream>

#include "GlobalUtil.h"
#include "Game.h"

HWND Game::_hWND = nullptr;
HWND Game::_contentHWND = nullptr;

Game* Game::_gameInstance = nullptr;

unsigned int Game::windowAdjustX = 0;
unsigned int Game::windowAdjustY = 0;

LRESULT Game::_WinProc(HWND hWND, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) 
	{
	case WM_SIZE:
	{
		const SIZE ar = { GlobalUtil::SCREEN_WIDTH, GlobalUtil::SCREEN_HEIGHT };

		int clientWidth = LOWORD(lParam);
		int clientHeight = HIWORD(lParam);

		int contentWidth = MulDiv(clientHeight, ar.cx, ar.cy);
		int contentHeight = MulDiv(clientWidth, ar.cy, ar.cx);

		if (contentWidth > clientWidth) {
			contentWidth = clientWidth;
			contentHeight = MulDiv(contentWidth, ar.cy, ar.cx);
		}
		else {
			contentHeight = clientHeight;
			contentWidth = MulDiv(contentHeight, ar.cx, ar.cy);
		}

		//Caculate offsets to center content
		int offsetX = (clientWidth - contentWidth) / 2;
		int offsetY = (clientHeight - contentHeight) / 2;

		//Adjust content window position
		SetWindowPos(
			_contentHWND,
			nullptr,
			offsetX,
			offsetY,
			contentWidth,
			contentHeight,
			SWP_NOACTIVATE|SWP_NOOWNERZORDER|SWP_NOZORDER	
		);
	}
	break;
	case WM_SIZING:
		_ResizeWindow(static_cast<int>(wParam), *reinterpret_cast<LPRECT>(lParam));
		break;
	case WM_GETMINMAXINFO:
	{
		MINMAXINFO* info = reinterpret_cast<MINMAXINFO*>(lParam);
		info->ptMinTrackSize.y =
			((info->ptMinTrackSize.x - Game::windowAdjustX) *
				_ASPECT_RATIO_Y) / _ASPECT_RATIO_X + Game::windowAdjustY;
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CHAR:
		switch (wParam)
		{
		case VK_TAB:
			//GlobalUtil::debugMode = !GlobalUtil::debugMode;
			break;
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
	}

	return DefWindowProc(hWND, message, wParam, lParam);
}

LRESULT Game::_WinProcContent(HWND hWND, UINT message, WPARAM wParam, LPARAM lParam) {
	return DefWindowProc(hWND, message, wParam, lParam);
}

void Game::_ResizeWindow(int edge, RECT& rect) {
	int size_x_desired = (rect.right - rect.left) - windowAdjustX;
	int size_y_desired = (rect.bottom - rect.top) - windowAdjustY;

	switch (edge) 
	{
	case WMSZ_BOTTOM:
	case WMSZ_TOP:
	{
		int size_x = windowAdjustX + (size_y_desired * _ASPECT_RATIO_X) / _ASPECT_RATIO_Y;
		rect.left = (rect.left + rect.right) / 2 - size_x / 2;
		rect.right = rect.left + size_x;
	}
	break;
	case WMSZ_BOTTOMLEFT:
	{
		int size_x, size_y;

		if (size_x_desired * _ASPECT_RATIO_Y > size_y_desired * _ASPECT_RATIO_X) {
			size_x = rect.right - rect.left;
			size_y = windowAdjustY + ((size_x - windowAdjustX) * _ASPECT_RATIO_Y) / _ASPECT_RATIO_X;
		}
		else {
			size_y = rect.bottom - rect.top;
			size_x = windowAdjustX + ((size_y - windowAdjustY) * _ASPECT_RATIO_X) / _ASPECT_RATIO_Y;
		}

		rect.left = rect.right - size_x;
		rect.bottom = rect.top + size_y;
	}
	break;
	case WMSZ_BOTTOMRIGHT:
	{
		int size_x, size_y;

		if (size_x_desired * _ASPECT_RATIO_Y > size_y_desired * _ASPECT_RATIO_X) {
			size_x = rect.right - rect.left;
			size_y = windowAdjustY + ((size_x - windowAdjustX) * _ASPECT_RATIO_Y) / _ASPECT_RATIO_X;
		}
		else {
			size_y = rect.bottom - rect.top;
			size_x = windowAdjustX + ((size_y - windowAdjustY) * _ASPECT_RATIO_X) / _ASPECT_RATIO_Y;
		}

		rect.right = rect.left + size_x;
		rect.bottom = rect.top + size_y;
	}
	break;
	case WMSZ_LEFT:
	case WMSZ_RIGHT:
	{
		int size_y = windowAdjustY + (size_x_desired * _ASPECT_RATIO_Y) / _ASPECT_RATIO_Y;
		rect.top = (rect.top + rect.bottom) / 2 - size_y / 2;
		rect.bottom = rect.top + size_y;
	}
	break;
	case WMSZ_TOPLEFT:
	{
		int size_x, size_y;
		if (size_x_desired * _ASPECT_RATIO_Y > size_y_desired * _ASPECT_RATIO_X) {
			size_x = rect.right - rect.left;
			size_y = windowAdjustY + ((size_x - windowAdjustX) * _ASPECT_RATIO_Y) / _ASPECT_RATIO_X;
		}
		else {
			size_y = rect.bottom - rect.top;
			size_x = windowAdjustX + ((size_y - windowAdjustY) * _ASPECT_RATIO_X) / _ASPECT_RATIO_Y;
		}
		rect.left = rect.right - size_x;
		rect.top = rect.bottom - size_y;
	}
	break;
	case WMSZ_TOPRIGHT:
	{
		int size_x, size_y;

		if (size_x_desired * _ASPECT_RATIO_Y > size_y_desired * _ASPECT_RATIO_X)
		{
			size_x = rect.right - rect.left;
			size_y = windowAdjustY + ((size_x - windowAdjustX) * _ASPECT_RATIO_Y) / _ASPECT_RATIO_X;
		}
		else
		{
			size_y = rect.bottom - rect.top;
			size_x = windowAdjustX + ((size_y - windowAdjustY) * _ASPECT_RATIO_X) / _ASPECT_RATIO_Y;
		}

		rect.right = rect.left + size_x;
		rect.top = rect.bottom - size_y;
	}
	break;
	}
}

void Game::_CreateContentWindow(HINSTANCE hInstance) {
	WNDCLASSEX wndClass{};
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = reinterpret_cast<WNDPROC>(_WinProcContent);
	wndClass.hIcon = reinterpret_cast<HICON>(LoadImage(hInstance, _GAME_ICON, IMAGE_ICON, 0, 0, LR_LOADFROMFILE));
	wndClass.hIconSm = reinterpret_cast<HICON>(LoadImage(hInstance, _GAME_ICON, IMAGE_ICON, 0, 0, LR_LOADFROMFILE));
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wndClass.lpszClassName = L"Content_Class";
	RegisterClassEx(&wndClass);

	_contentHWND = CreateWindow(
		L"Content_Class",
		nullptr,
		WS_CHILD|WS_VISIBLE,
		0,
		0,
		0,
		0,
		_hWND,
		nullptr,
		hInstance,
		nullptr
	);

	if (_contentHWND == nullptr) {
		OutputDebugStringA("[GAME] _contentHWND was nullptr\n");
		return;
	}
}

void Game::_ParseSettings(std::string line) {
	std::vector<std::string> tokens = GlobalUtil::SplitStr(line);
	if (tokens.size() < 2) {
		return;
	}
	if (tokens.at(0) == "default") {
		_defaultSceneID = std::stoul(tokens.at(1));
	}
	else {
		OutputDebugStringA("[GAME] Unknown game settings\n");
	}
}

void Game::_Update(DWORD deltaTime) {
	/*const int UP_KEY = Device::GetInstance()->GetControllerKey("Up");
	const int LEFT_KEY = Device::GetInstance()->GetControllerKey("LEFT");
	const int DOWN_KEY = Device::GetInstance()->GetControllerKey("DOWN");
	const int RIGHT_KEY = Device::GetInstance()->GetControllerKey("RIGHT");
	const int SELECT_KEY = Device::GetInstance()->GetControllerKey("SELECT");
	const int START_KEY = Device::GetInstance()->GetControllerKey("START");
	const int B_KEY = Device::GetInstance()->GetControllerKey("B");
	const int A_KEY = Device::GetInstance()->GetControllerKey("A");*/

	_managerInstance->GetCurrentScene()->Update(deltaTime);
}

void Game::_Render() {
	auto device = GlobalUtil::directDevice;
	auto spriteHandler = GlobalUtil::spriteHandler;
	auto currentScene = _managerInstance->GetCurrentScene();

	auto swapChain = _swapChain;
	auto renderTargetView = _renderTargetView;
	auto blendState = _blendState;

	device->ClearRenderTargetView(renderTargetView, currentScene->GetBGColor());
	spriteHandler->Begin(D3DX10_SPRITE_SORT_TEXTURE | D3DX10_SPRITE_SAVE_STATE);

	//RGBA
	float newBlendFactor[4] = { 0.0f };
	device->OMSetBlendState(blendState, newBlendFactor, 0xffffffff);

	currentScene->Render();
	spriteHandler->End();
	swapChain->Present(0, 0);
}

Game::Game() {
	_managerInstance = SceneManager::GetInstance();
}

Game::~Game() {
	/*if (_pipelineInstance != nullptr) {
		_pipelineInstance->Release();
	}

	if (_deviceInstance != nullptr) {
		_deviceInstance->Release();
	}*/

	if (_managerInstance != nullptr) {
		_managerInstance->Release();
	}
}

Game* Game::GetInstance() {
	if (_gameInstance == nullptr) {
		_gameInstance = new Game;
	}
	return _gameInstance;
}

const unsigned int Game::GetBackBufferWidth() {
	return _backBufferWidth;
}

const unsigned int Game::GetBackBufferHeight() {
	return _backBufferHeight;
}

const unsigned int Game::GetWindowWidth() {
	return _windowWidth;
}

const unsigned int Game::GetWindowHeight() {
	return _windowHeight;
}

HWND Game::CreateGameWindow(HINSTANCE hInstance, int nCmdShow, int width, int height) {
	WNDCLASSEX wndClass;
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = reinterpret_cast<WNDPROC>(_WinProc);
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hIcon = reinterpret_cast<HICON>(LoadImage(hInstance, _GAME_ICON, IMAGE_ICON, 0, 0, LR_LOADFROMFILE));
	wndClass.hIconSm = reinterpret_cast<HICON>(LoadImage(hInstance, _GAME_ICON, IMAGE_ICON, 0, 0, LR_LOADFROMFILE));
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = _CLASS_NAME;
	RegisterClassEx(&wndClass);

	_hWND = CreateWindow(
		_CLASS_NAME,
		_GAME_TITLE,
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		height,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	if (_hWND == nullptr) {
		OutputDebugStringA("[GAME] _hWND was nullptr\n");
		return FALSE;
	}

	_CreateContentWindow(hInstance);

	ShowWindow(_hWND, nCmdShow);
	UpdateWindow(_hWND);

	return _hWND;
}

bool Game::InitGame(HWND hWND) {
	_isRunning = true;

	RECT window;
	GetClientRect(hWND, &window);

	_backBufferWidth = window.right + 1;
	_backBufferHeight = window.bottom + 1;

	_windowWidth = window.right + 1;
	_windowHeight = window.bottom + 1;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = _backBufferWidth;
	swapChainDesc.BufferDesc.Height = _backBufferHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = _FRAME_RATE;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = _contentHWND;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = true;

	HRESULT hResult = D3D10CreateDeviceAndSwapChain(
		nullptr,
		D3D10_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		D3D10_SDK_VERSION,
		&swapChainDesc,
		&_swapChain,
		&GlobalUtil::directDevice
	);

	if (FAILED(hResult)) {
		OutputDebugStringA("[PIPELINE] Failed to create device and swap chain\n");
		return false;
	}

	ID3D10Texture2D* backBuffer;
	hResult = _swapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), reinterpret_cast<LPVOID*>(&backBuffer));
	if (FAILED(hResult)) {
		OutputDebugStringA("[PIPELINE] Failed to get back buffer from the swap chain\n");
		return false;
	}

	hResult = GlobalUtil::directDevice->CreateRenderTargetView(backBuffer, nullptr, &_renderTargetView);
	backBuffer->Release();
	if (FAILED(hResult)) {
		OutputDebugStringA("[PIPELINE] Failed to create render target view\n");
		return false;
	}
	GlobalUtil::directDevice->OMSetRenderTargets(1, &_renderTargetView, nullptr);

	D3D10_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D10_RASTERIZER_DESC));
	rasterizerDesc.FillMode = D3D10_FILL_SOLID;
	rasterizerDesc.CullMode = D3D10_CULL_NONE;
	rasterizerDesc.DepthClipEnable = true;

	hResult = GlobalUtil::directDevice->CreateRasterizerState(&rasterizerDesc, &_rasterizerState);
	if (FAILED(hResult)) {
		OutputDebugStringA("[PIPELINE] Failed to create rasterizer state\n");
		return false;
	}
	GlobalUtil::directDevice->RSSetState(_rasterizerState);

	D3D10_VIEWPORT viewPort;
	viewPort.Width = _backBufferWidth;
	viewPort.Height = _backBufferHeight;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	GlobalUtil::directDevice->RSSetViewports(1, &viewPort);

	hResult = D3DX10CreateSprite(GlobalUtil::directDevice, 0, &GlobalUtil::spriteHandler);
	if (FAILED(hResult)) {
		OutputDebugStringA("[PIPELINE] Failed to create sprite handler\n");
		return false;
	}

	//Creates the projection matrix using the values in the viewport
	D3DXMATRIX projectionMatrix;
	D3DXMatrixOrthoOffCenterLH(
		&projectionMatrix,
		static_cast<float>(viewPort.TopLeftX),
		static_cast<float>(viewPort.Width),
		static_cast<float>(viewPort.TopLeftY),
		static_cast<float>(viewPort.Height),
		0.1f,
		10.0f
	);
	hResult = GlobalUtil::spriteHandler->SetProjectionTransform(&projectionMatrix);
	if (FAILED(hResult)) {
		OutputDebugStringA("[PIPELINE] Failed to create the projection matrix\n");
		return false;
	}

	D3D10_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D10_BLEND_DESC));
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.BlendEnable[0] = true;
	blendDesc.SrcBlend = D3D10_BLEND_SRC_ALPHA;
	blendDesc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
	blendDesc.BlendOp = D3D10_BLEND_OP_ADD;
	blendDesc.SrcBlendAlpha = D3D10_BLEND_ONE;
	blendDesc.DestBlendAlpha = D3D10_BLEND_ZERO;
	blendDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
	blendDesc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;

	hResult = GlobalUtil::directDevice->CreateBlendState(&blendDesc, &_blendState);
	if (FAILED(hResult)) {
		OutputDebugStringA("[PIPELINE] Failed to create blend state\n");
		return false;
	}

	return true;
}

bool Game::InitDevice() {
	/*if (!_deviceInstance->InitKeyboard(_hWND)) {
		return false;
	}*/

	return true;
}

void Game::LoadSettings(std::string filePath) {
	std::ifstream readFile;
	readFile.open(filePath, std::ios::in);
	
	if (!readFile.is_open()) {
		OutputDebugStringA("[GAME] Failed to read file\n");
		return;
	}

	_GameFileSection section = _GameFileSection::GAMEFILE_SECTION_UNKNOWN;

	char str[GlobalUtil::MAX_FILE_LINE];
	while (readFile.getline(str, GlobalUtil::MAX_FILE_LINE)) {
		std::string line(str);

		if (line.empty() || line.front() == '#') {
			continue;
		}

		if (line == "[/]") {
			section = _GameFileSection::GAMEFILE_SECTION_UNKNOWN;
			continue;
		}

		if (line == "[SETTINGS]") {
			section = _GameFileSection::GAMEFILE_SECTION_SETTINGS;
			continue;
		}

		if (line == "[SCENES]") {
			section = _GameFileSection::GAMEFILE_SECTION_SCENES;
			continue;
		}

		switch (section) {
		case _GameFileSection::GAMEFILE_SECTION_SETTINGS:
			_ParseSettings(line);
			break;
		case _GameFileSection::GAMEFILE_SECTION_SCENES:
			_managerInstance->ParseScenes(line);
			break;
		}
	}

	readFile.close();
	_managerInstance->ChangeScene(_defaultSceneID);
}

void Game::GameRun() {
	MSG msg;
	DWORD frameStart = static_cast<DWORD>(GetTickCount64());
	DWORD ticksPerFrame = 1000 / _FRAME_RATE;

	while (_isRunning) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				_isRunning = false;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			DWORD now = static_cast<DWORD>(GetTickCount64());
			DWORD deltaTime = now - frameStart;

			if (deltaTime >= ticksPerFrame) {
				frameStart = now;

				//_deviceInstance->ProcessKeyboardInputs();

				_Update(deltaTime);
				_Render();

				Sleep(0);
			}
			else {
				//The time has come and so have I
				//Sleep(ticksPerFrame - deltaTime);
			}
		}
	}
}





