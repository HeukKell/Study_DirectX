#include "pch.h"
#include "CommandQueue.h"
#include "SwapChain.h"

CommandQueue::~CommandQueue()
{

	// 이벤트 정리
	// 특별히 이것을 호출해주지 않아도 프로그램상 문제는 없지만, 이벤트를 활용했다면 해지해주는게 정석
	::CloseHandle(_fenceEvent);
}



/*
	device 를 이용해서 CommandQueue 를 만들어 주는것이니 device 를 인수로 설정

*/
void CommandQueue::Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain)
{

	_swapChain = swapChain;

	// 어떤식으로 만들어 줄것인지 우선 정한다음
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	/*
		어떻게 만들것인지 : queueDesc
		어디에다가 만들것인지 : _cmdQueue

		new 를 이용해서 만드는것이 아니라 Com 객체를 통해서,
		Com 객체가 제공하는 함수를 통해서 간접적으로 만들고 있다.
	*/
	device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_cmdQueue));

	/*
		-D3D12_COMMAND_LIST_TYPE_DIRECT : GPU 가 직접 실행하는 명령 목록
	*/ 
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAlloc));

	/*
		GPU 가 하나인 시스템에선 0 으로
		DIRECT or BUNDLE
		Allocator
		초기상태 (그리기 명령은 nullptr 로 지정)

		_cmdAlloc 를 참조하고 있는데,
		vector 처럼 clear 를 한다고 해서 메모리를 해지하는것이 아니라,
		할당한 메모리는 그대로 두고, 사이즈만 줄여서 쓰는 방식인데,
		그점이 유사하게 작동하고 이렇게 메모리 관리를 하는 객체가
		_cmdAlloc 이다.

	*/
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_cmdList));


	/*
		CommandList 는 close / open 상태가 잇는데
		Open 상태에서 Command 를 넣다가 Close 한 다음 제출하는 개념
	*/
	_cmdList->Close();

	/*
		CreateFence
		- CPU 와 GPU 의 동기화 수단으로 쓰인다

		CreateEvent 는 DX 에서 쓰는 특수한 함수가 아니라
		멀티스레드 동기화 할때 쓰는 자주쓰는 방법
		신호등 같은. 빨간불일때 꺼졌다가 파란불일때 켜지는
		특정 조건이 갖추어 지면 활성화 되는
	*/
	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
	_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);

}

void CommandQueue::WaitSync()
{
	/*
		멀티 스레드 관련부분이며
		일감에 값을 부여해 끝날때까지 기다려 주고 있는 상황이다.
		CPU 가 GPU 의 일감이 끝날때까지 대기하고 있는 방식.

		사실 게임에서는.
		CPU 가 GPU 가 일을 끝날때까지 기다리는것은 좋은 방식은 아니다.
		일을 맡겼다면 그동안 다른일을 할수도 있는 것인데, 
		일단 첫 코드이기 때문에 이렇게 작성한 것이다.
	*/

	/*
		Advance the fence value to mark commands up to this fence point

		일감을 요청한 다음 번호를 메길 것이다.
		1씩 증가 한다.
	*/
	_fenceValue++;

	/*
		Add an Instruction to the command queue to set a new fence point
		Because we are on the GPU timeline, thie new fence point won't be set until the GPU finishes
		processing all the commands prior to this Signal().

		command queue 에 다가 번호를 넘겨주고
	*/
	_cmdQueue->Signal(_fence.Get(), _fenceValue);

	// wait until the GPU has completed commands up to this fence point
	if (_fence->GetCompletedValue() < _fenceValue)
	{
		// Fire event when GPU hits current fence
		// fence 에다가 그 이벤트가 끝났다면, 이벤트를 실행시켜 달라고 한다.
		_fence->SetEventOnCompletion(_fenceValue, _fenceEvent);

		// wait until the GPU hits current fence evnet is fired
		// 기다림 
		::WaitForSingleObject(_fenceEvent, INFINITE);
	}
}

void CommandQueue::RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rect)
{
	// std::vector clear 와 유사하다 . 싹 비워주는데 메모리가 줄어들지는 않고, capacity 가 그대로 유지되는 느낌
	_cmdAlloc->Reset();
	_cmdList->Reset(_cmdAlloc.Get(), nullptr);


	/*
		CD**** 는 헬퍼 클래스다.

		화면출력에 사용되던 렌더타겟을
		GPU 작업용 렌더타겟으로 바꿔치기 해 주겠다는 의미.
		
		D3D12_RESOURCE_BARRIER는 개별적인 명령어 객체가 아닌, 
		리소스 상태 전환정보를 담은 구조체입니다. 
		리소스 전환 명령에 대한 구조체 생성함수 CD3DX12_RESOURCE_BARRIER::Transition()
		ResourceBarrier 함수의 매개 변수로 사용됩니다


	*/ 
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		_swapChain->GetBackRTBuffer().Get(),	// renderTarget
		D3D12_RESOURCE_STATE_PRESENT, // 화면출력
		D3D12_RESOURCE_STATE_RENDER_TARGET // 외주 결과물
	);

	/*
		ResourceBarrier는 ID3D12GraphicsCommandList 인터페이스에서 제공하는 함수 중 하나로, 
		바리어를 추가하여 GPU 명령 버퍼의 작업을 제어하는 데 사용됩니다. 
		이 함수는 다음과 같은 매개 변수를 사용합니다.

		NumBarriers : 배열에 포함될 배리어 개수
		pBarriers : q배리어 배열의 포인터

		함수가 호출되면, pBarriers 매개변수로 전달된 배리어 배열이 GPU 명령 버퍼에 추가됩니다.
		이를 통해 리소스 상태 전환을 동기화 하고, 작업 순서를 제어할 수 있습니다.
	*/
	_cmdList->ResourceBarrier(1, &barrier);

	// Set the viewport and scissor rect, this needs to be reset whenever the command list is reset.
	_cmdList->RSSetViewports(1, vp);
	_cmdList->RSSetScissorRects(1, rect);

	// Specify the buffers we are going to render to
	// 어떤 버퍼에다가 그림을 그릴지를 다시한번 명시해 주어야 한다.
	// 백버퍼를 들고와서
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferView = _swapChain->GetBackRTV();

	// 백버퍼 에다가 그림을 그려달라고 하는것이다.
	_cmdList->ClearRenderTargetView(backBufferView, Colors::LightSteelBlue, 0, nullptr);
	_cmdList->OMSetRenderTargets(1, &backBufferView, FALSE, nullptr);

}

void CommandQueue::RenderEnd()
{
	/*
		GPU 작업용에 사용되던 렌더타겟을
		화면 출력 렌더타겟으로 바꿔치기 해 주겠다는 의미.
	*/
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		_swapChain->GetBackRTBuffer().Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, // 외주 결과물
		D3D12_RESOURCE_STATE_PRESENT // 화면출력
	);

	_cmdList->ResourceBarrier(1, &barrier);
	_cmdList->Close();

	// 커맨드 리스트 수행
	ID3D12CommandList* cmdListArr[] = { _cmdList.Get() };
	_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	_swapChain->Present();

	/*
		Wait until frame commands are complete.
		this Whating is inefficient and is done for simplicity.
		Later we will show how to organize our renderting code
		so we do not have to wait per frame.
	*/
	WaitSync();

	_swapChain->SwapIndex();
}
