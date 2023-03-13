#pragma once






class Engine
{

private:
	// 그려질 화면 크기 관련 변수
	WindowInfo		_window;
	D3D12_VIEWPORT	_viewport = {};
	D3D12_RECT		_scissorRect = {};

	
	shared_ptr<class Device>			_device;
	shared_ptr<class CommandQueue>		_cmdQueue;
	shared_ptr<class SwapChain>			_swapChain;
	shared_ptr<class DescriptorHeap>	_descHeap;


public:

	void Init(const WindowInfo& window); // 출력윈도우 핸들과, 너비, 높이, 창모드인지 전체화면인지 여부를 받아야해
	void Render();

	void ResizeWindow(int32 width, int32 height); // 윈도우 크기창을 다시 설정하는 함수

public:

	/** CommandQueue 에다가 요청사항들을 적어주는 함수*/
	void RenderBegin();

	/** 쌓은 일감들을 GPU 에게 외주를 맡겨, 실질적으로 모든것을 그리는 작업 */
	void RenderEnd();

};

