#pragma once


// 전방선언
class SwapChain;
class DescriptorHeap;

/*
	예전에는 device 를 통해서 수행하였으나 . (DX9 이전에는)
	일감을 그냥 던져주면 되었으나.
	DX12 에 오면서 일감을 던져주는 방식이 좀더 복잡하게 바뀌었다.

	일감들을 모아서 실행하는 부분
*/ 
class CommandQueue
{
public:
	~CommandQueue();

private:

	/*
		CommandQueue : DX12 에 등장
		GPU 에게 외주를 요청할 때, 하나씩 요청하면 비효율적
		[외주목록] 에 일감을 차곡차곡 기록햇다가 한 방에 요청하도록 설계

	*/

	/*
		ID3D12CommandQueue 인터페이스를 포인터 형태로 관리하기 위해 사용됩니다.
		DirectX 12에서 사용되는 명령 큐 인터페이스입니다. 
		CPU 측에서 명령을 발행하고, GPU에서 명령을 수행하는 데 사용됩니다. 
		
		이 인터페이스는 다음과 같은 주요 함수를 제공합니다.

		ExecuteCommandLists: GPU 명령 리스트를 실행합니다.
		Signal: GPU에서 이벤트를 시그널합니다.
		Wait: GPU에서 이벤트를 대기합니다.
		
		QueryInterface: 인터페이스에 대한 포인터를 얻습니다.
		AddRef: 참조 횟수를 증가시킵니다.
		Release: 참조 횟수를 감소시킵니다.
	*/

	ComPtr<ID3D12CommandQueue>			_cmdQueue;
	
	
	/*
		ID3D12CommandAllocator는 DirectX 12에서 사용되는 명령 할당자 인터페이스입니다. 명령 할당자는 GPU 명령 버퍼를 생성하는 데 사용됩니다. GPU 명령 버퍼는 GPU에서 실행할 명령을 저장하는 버퍼입니다.
		이 인터페이스는 다음과 같은 주요 함수를 제공합니다.

		Reset: 명령 할당자를 재설정합니다.
		
		QueryInterface: 인터페이스에 대한 포인터를 얻습니다.
		AddRef: 참조 횟수를 증가시킵니다.
		Release: 참조 횟수를 감소시킵니다.
	*/
	ComPtr<ID3D12CommandAllocator>		_cmdAlloc; // 일감자체를 넣어주기 위한 메모리 공간을 할당하는 애

	/*
		일감 리스트. 
		ID3D12GraphicsCommandList는 GPU 명령을 기록하고 실행하는 인터페이스입니다.

		ID3D12GraphicsCommandList는 DirectX 12에서 사용되는 그래픽 명령 리스트 인터페이스입니다. 그래픽 명령 리스트는 GPU에서 실행할 그래픽 명령을 저장하는 버퍼입니다. 이 인터페이스는 다음과 같은 주요 함수를 제공합니다.

		Close: 명령 리스트를 종료합니다.
		Reset: 명령 리스트를 재설정합니다.
		ResourceBarrier: 자원에 대한 배리어를 설정합니다.
		SetGraphicsRootSignature: 그래픽 루트 시그니처를 설정합니다.
		SetPipelineState: 파이프라인 상태 개체를 설정합니다.
		DrawInstanced: 인스턴스를 지정하여 그리기 작업을 수행합니다.
		DrawIndexedInstanced: 인덱스를 사용하여 인스턴스를 지정하여 그리기 작업을 수행합니다.
		
		QueryInterface: 인터페이스에 대한 포인터를 얻습니다.
		AddRef: 참조 횟수를 증가시킵니다.
		Release: 참조 횟수를 감소시킵니다.
	*/
	ComPtr<ID3D12GraphicsCommandList>	_cmdList;  

	/*
		Fence : 울타리(?)
		CPU / GPU 동기화를 위한 간단한 도구

		일감을 만들어 놨다면 던져 줄건데,
		결과물이 와야 보낼 수 있는 일감이 있을때,
		울타리를 쳐놓고,
		외주 작업이 끝날때 까지 기다리겠다. 할때 fence 를 활용한다.

		만약 일감을 던졌을때, 그리고 일감 목록을 105 번 이라 하였을때,
		105 번 이 다 끝날때 까지 기다리기 위해.
		fence 를 활용한다.

		ID3D12Fence는 DirectX 12에서 사용되는 플래그 값 기반 동기화 기술을 제공합니다. 이 인터페이스를 사용하여 GPU가 처리한 명령의 완료 여부를 확인하고, CPU와 GPU 간의 동기화를 수행할 수 있습니다. 이 인터페이스는 다음과 같은 주요 함수를 제공합니다.

		GetCompletedValue: 이전에 완료된 명령의 시퀀스 값을 반환합니다.
		SetEventOnCompletion: 지정된 값이 되었을 때 이벤트를 설정합니다.
		Signal: 지정된 값을 시퀀스 값으로 설정합니다.
		QueryInterface: 인터페이스에 대한 포인터를 얻습니다.
		AddRef: 참조 횟수를 증가시킵니다.
		Release: 참조 횟수를 감소시킵니다.
	*/
	ComPtr<ID3D12Fence>					_fence;
	uint32								_fenceValue = 0;
	HANDLE								_fenceEvent = INVALID_HANDLE_VALUE;

	shared_ptr<SwapChain>				_swapChain;
	

public:

	// 초기화 함수
	void Init(
		ComPtr<ID3D12Device> device,
		shared_ptr<SwapChain> swapChain
	);


	/*
		Fence 를 이용해서 기다려 주는 함수
	*/
	void WaitSync();

	
	void RenderBegin(
		const D3D12_VIEWPORT* vp,
		const D3D12_RECT* rect
	);

	void RenderEnd();

	ComPtr<ID3D12CommandQueue> GetCmdQue() { return _cmdQueue; }
	
};

