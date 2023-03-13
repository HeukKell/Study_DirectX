#include "pch.h"
#include "Engine.h"

#include "Device.h"
#include "CommandQueue.h"
#include "SwapChain.h"



void Engine::Init(const WindowInfo& window)
{
	_window = window;
	ResizeWindow(window.width, window.height);

	// 그려질 화면 크기를 설정
	_viewport = { 0,0, static_cast<FLOAT>(window.width), static_cast<FLOAT>(window.height), 0.0f, 1.0f };
	_scissorRect = CD3DX12_RECT(0, 0, window.width, window.height);

	// Heap 영역에 클래스 할당후 포인터 설정
	_device = make_shared<Device>();
	_cmdQueue = make_shared<CommandQueue>();
	_swapChain = make_shared<SwapChain>();

	// 초기화 부분
	_device->Init();
	_cmdQueue->Init(_device->GetDevice(), _swapChain);
	_swapChain->Init(window,_device->GetDevice(), _device->GetDXGI(), _cmdQueue->GetCmdQue());
}


void Engine::ResizeWindow(int32 width, int32 height)
{
	_window.width = width;
	_window.height = height;

	RECT rect = { 0,0,width, height };
	::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false); // 메뉴바 없이, 사이즈 재계산해서 알려줘
	::SetWindowPos(_window.hwnd, 0, 100, 100, rect.right - rect.left, rect.bottom - rect.top, 0);

	// :: 을 안붙여도 되지만, 글로벌 네임스페이스 등,  윈도우에서 제공하는 표준라이브러리 의 함수를 쓰고 있다는 표시를 출 수 있다.

}

void Engine::Render()
{
	RenderBegin();

	// TODO : 나머지 물체들을 그려준다
	// 이구간에서 command list 에 요청을 밀어넣으면 딸려서 실행된다.

	RenderEnd();
}



void Engine::RenderBegin()
{
	_cmdQueue->RenderBegin(&_viewport, &_scissorRect);
}

void Engine::RenderEnd()
{
	_cmdQueue->RenderEnd();
}
