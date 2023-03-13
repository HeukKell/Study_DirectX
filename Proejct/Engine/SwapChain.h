#pragma once

/*
교환 사슬

# 외주과정
- 현재 게임 세상에 있는 상황을 묘사
- 어떤 공식으로 어떻게 계산할지 던져준다
- GPU 가 열심히 계산(외주)
- 결과물을 받아서 화면에 그려준다


# [외주 결과물] 을 어디에 받지?
- 어떤 종이 (Buffer) 에 그려서 건내달라고 부탁해보자
- 특수 종이를 만들어서 -> 처음에 건내주고 -> 결과물을 해당 종이에 받는다 ok
- 우리 화면에 특수 종이(외주 결과물) 출력해준다

# 고려해야 할점
- 그런데 화면에 현재 결과물을 출력하는 와중에, 다음 화면도 외주를 맡겨야 한다
- 현재 화면 결과물은 이미 화면 출력에 사용중이다.
- 특수종이를 2개 만들어서, 하나는 현재 화면을 그려주고, 하나는 외주를 맡기고
- Double Buffering

# 결론
- 현재 화면 작업중 : (종이 0번) <-> (종이 1번) : GPU 작업중'backBuffer'
- 종이를 계속 바꿔서 현재 화면에 출력하고, 뒤로는 GPU 에 맡기고
- 버퍼를 여러개 만들어서 왔다갔다 바꿔가면서 화면에 출력하는것이 swap chain 의 핵심
*/
class SwapChain
{

public:


private:

	/*
		render Target
		그릴 대상, 특수종이		 
	*/ 
	ComPtr<IDXGISwapChain>	_swapChain;


	/*
		리소스를 서술하는 Descriptor ( 기안서 )
		
		- 외주를 맡길 때 이런 저런 정보들을 같이 넘겨줘야 하는데, 아무런 형태로나 요청하면 알아듣지 못한다.
		- 각종 리소스를 어떤 용도로 사용해야 하는지 꼼꼼하게 적어서 넘겨주는 용도
		- 규격, 양식
		- DX12 에서 추가된 개념
		- DX11 이전에서는 View 라는 용어로 활용했지만, DX12 에서는 서술자Heap 이라고 부른다
		- 리소스를 한번더 랩핑해서, 그 리소스가 무엇인지 적은 기안서를 제출하는 방식

		- GPU 한테 일을 맡길때 GPU 가 알수 있게 RenderTarget을 서술을추가하여 랩핑하기위한 인터페이스 Descriptor Heap
		
		rtv : Render Target View
		Descriptor 는 리소스를 설명하는 역할을 한다.
		_rtvHeap [ [RTV]  [RTV] ]
	*/
	
	ComPtr<ID3D12Resource>			_rtBuffer[SWAP_CHAIN_BUFFER_COUNT]; 
	ComPtr<ID3D12DescriptorHeap>	_rtvHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE		_rtvHandle[SWAP_CHAIN_BUFFER_COUNT];

	uint32					_backBufferIdx = 0;

private:

	/**
	  SwapChain 생성 함수
	*/
	void CreateSwapChain(
		const WindowInfo& window, 
		ComPtr<IDXGIFactory> dxgi, 
		ComPtr<ID3D12CommandQueue> cmdQueue
	);

	/**
		RenderTargetView 생성 함수
		RenderTarget 의 Descriptor를 작성하는 함수
		본래 DescripterHeap 클래스에 있던 함수였지만, 옮겨왔다.
	*/
	void CreateRTV(ComPtr<ID3D12Device> device);


public:
	void Init(
		const WindowInfo& window,
		ComPtr<ID3D12Device> device,
		ComPtr<IDXGIFactory> dxgi,
		ComPtr<ID3D12CommandQueue> cmdQueue
	);

	void Present();

	void SwapIndex();

	/** swapChain 을 반환합니다. */
	ComPtr<IDXGISwapChain> getSwapChain() { return _swapChain; }
	
	/** index 에 해당하는 렌더타겟을 반환합니다. */
	ComPtr<ID3D12Resource> GetRenderTarget(int32 index) { return _rtBuffer[index]; }
	
	/** GPU 가 작업중인 renderTarget 을 요청합니다.*/
	ComPtr<ID3D12Resource> GetBackRTBuffer() { return _rtBuffer[_backBufferIdx]; };

	D3D12_CPU_DESCRIPTOR_HANDLE GetBackRTV() { return _rtvHandle[_backBufferIdx]; }

};

