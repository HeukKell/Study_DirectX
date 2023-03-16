#pragma once

#include "Device.h"
#include "CommandQueue.h"
#include "CommandQueue.h"
#include "RootSignature.h"

#include "ConstantBuffer.h"

#include "Mesh.h"
#include "Shader.h"

class Engine
{

private:
	// 그려질 화면 크기 관련 변수
	WindowInfo		_window;
	D3D12_VIEWPORT	_viewport = {};
	D3D12_RECT		_scissorRect = {};

	
	shared_ptr<Device>				_device;		// 인력사무소대표
	shared_ptr<CommandQueue>		_cmdQueue;		// GPU 에 보낼요청 을 관리
	shared_ptr<SwapChain>			_swapChain;		// DoubleBuffering 용도. Buffer swap
	shared_ptr<RootSignature>		_rootSignature; // 계약서, 서명
	shared_ptr<ConstantBuffer>		_cb;			// constant buffer

public:

	void Init(const WindowInfo& window); // 출력윈도우 핸들과, 너비, 높이, 창모드인지 전체화면인지 여부를 받아야해
	void Render();

	void ResizeWindow(int32 width, int32 height); // 윈도우 크기창을 다시 설정하는 함수

	shared_ptr<Device>			GetDevice()		{ return _device; }
	shared_ptr<CommandQueue>	GetCmdQueue()	{ return _cmdQueue; }
	shared_ptr<SwapChain>		GetSwapChain()	{ return _swapChain; }
	shared_ptr<RootSignature>	GetRootSignature(){ return _rootSignature; }
	shared_ptr<ConstantBuffer>	GetConstantBuffer() { return _cb; }

public:

	/** CommandQueue 에다가 요청사항들을 적어주는 함수*/
	void RenderBegin();

	/** 쌓은 일감들을 GPU 에게 외주를 맡겨, 실질적으로 모든것을 그리는 작업 */
	void RenderEnd();

};

