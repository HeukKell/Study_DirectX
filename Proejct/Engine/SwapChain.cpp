#include "pch.h"
#include "SwapChain.h"


/*
	다른것들은 device 를 통해서 만들어 줬는데,
	이것은 dxgi 를 통해 만들어주는것을 볼 수 있다.
	dxgi 가 디바이스의 출력과 관련된 부분을 담당한다.
*/
void SwapChain::Init(const WindowInfo& window, ComPtr<ID3D12Device> device, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue)
{
	

	CreateSwapChain(window, dxgi, cmdQueue);
	CreateRTV(device);
}

void SwapChain::Present()
{
	//Present the frame.
	_swapChain->Present(0, 0); // 현재 화면에다가 어떤 종이가 출력되어야 하는지 정해졌다면, 화면에다가 그려달라는 부분.
}

void SwapChain::SwapIndex()
{
	_backBufferIdx = (_backBufferIdx + 1) & SWAP_CHAIN_BUFFER_COUNT;
	// ([0|1] + 1) %2 . 나머지 연산을 통해 계속해서 0 과 1 을 반복하게 될것이다.
}



void SwapChain::CreateSwapChain(const WindowInfo& window, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue)
{
	// 없어도 된다. 혹시나 Init 을 두번했을까봐 이전의 만든 정보를 달린다.
	_swapChain.Reset();

	// swapChain 을 어떻게 만들것인지에 대한 묘사. 종이를 어떻게 만들어줄것인지.
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = static_cast<uint32>(window.width);		// 버퍼의 해상도 너비
	sd.BufferDesc.Height = static_cast<uint32>(window.height);		// 버퍼의 해상도 높이
	sd.BufferDesc.RefreshRate.Numerator = 60;	// 화면 갱신비율
	sd.BufferDesc.RefreshRate.Denominator = 1;	// 화면 갱신 비율
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 버퍼의 디스플레이 형식
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1; // 멀티샘플링 off
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //  후면 버퍼에 렌더링할것
	sd.BufferCount = SWAP_CHAIN_BUFFER_COUNT; // 전면 + 후면 버퍼
	sd.OutputWindow = window.hwnd;
	sd.Windowed = window.windowed;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // 전면 후면 버퍼 교체시 이전 프레임 정보 버림
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	dxgi->CreateSwapChain(cmdQueue.Get(), &sd, &_swapChain);  // dxgi 에, 우리가 만들어준 정보를 토대로 swapChain 에 넣어달라고 하는것이다.

	// swapchain 이 만들어지고, 버퍼 또한 같이 만들어 졌다.
	// 그래서 다시 꺼내서, renderTarget 이라는 변수에 다시 넣어주는것을 볼 수 있다.
	for (int32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
	{
		_swapChain->GetBuffer(i, IID_PPV_ARGS(&_rtBuffer[i]));
	}
}

void SwapChain::CreateRTV(ComPtr<ID3D12Device> device)
{

	/*
		descriptor (DX12) = View(~DX11)
		'서술자 힙' 으로 RTV 생성

		RTV(RenderTargetView)	// DX11
		DSV(DepthStencilView)	// DX11
		CBV(ConstantBufferView)
		SRV(ShaderResourceView)
		UAV(UnorderedAccessView)
		를 전부

		예전에는 View 마다 따로 서술해 줫다면,
		DX12 부터는 Descriptor Heap 에다가 모든 View 를 서술하여 관리.
	*/

	// 렌더타겟뷰의 사이즈 계산
	int32 rtvHeapSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);


	// 우리는 RenderTarget 을 갖고 있으니, RenderTargetView 를 서술해주어야 한다.
	D3D12_DESCRIPTOR_HEAP_DESC rtvDesc;
	rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;		// RenderTargetView 타입
	rtvDesc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT;	// render target view 개수
	rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	// 
	rtvDesc.NodeMask = 0;

	/*
	  같은 종류의 데이터끼리 배열로 관리 한다.
	  RTV 목록 : [] []			// 우리의 경우 렌더타겟 2개 관리중..
	  DSC 목록 : [] [] [] ...   // 이건 아직 쓰지않아

	  DescriptorHeap 은 일종의 배열이라고 볼 수 있다.
	  배열은 배열인데, 섞에서 관리하는것이 아니라. 같은 View 들을 배열로 묶어 관리하는 Descirptor

	  아래 CreateDescriptorHeap 의 결과
	  RTV 2개를 관리할 수 있는 배열이 만들어진다.
	*/
	device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&_rtvHeap));


	/*
		RTV [] [] 2개를 만들어 줬으니 내용을 채워야해.
		GetCPUDescriptrHandleForHeapStart() 함수를 통해 시작주소를 받아낸다.
		SIZT_T 타입의 ptr 이라는 주소를 관리하고 있다.
	*/
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapBegin = _rtvHeap->GetCPUDescriptorHandleForHeapStart();

	// RTV 2개의 내용 채워주기
	for (int i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
	{
		_rtvHandle[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeapBegin, i * rtvHeapSize);
		// 렌더타겟 뷰 생성, 묘사 즉, RTV 배열에다가 각각 렌더타겟뷰를 채워주고 있다.
		device->CreateRenderTargetView(_rtBuffer[i].Get(), nullptr, _rtvHandle[i]);

	}

	// GPU 에게 View 를 넘겨줄 수 있는 상태가 되었다.
}
