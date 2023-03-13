#include "pch.h"
#include "DescriptorHeap.h"

#include "SwapChain.h"


// cpp 에서는 class SwapChain 으로 전방선언해봤자 아무 소용이 없다. 그래서 헤더파일 참조

void DescriptorHeap::Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain)
{
	
	_swapChain = swapChain;

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
	_rtvHeapSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);


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
		_rtvHandle[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeapBegin, i * _rtvHeapSize);
		// 렌더타겟 뷰 생성, 묘사 즉, RTV 배열에다가 각각 렌더타겟을 채워주고 있다.
		device->CreateRenderTargetView(swapChain->GetRenderTarget(i).Get(), nullptr, _rtvHandle[i]);

	}

	// GPU 에게 View 를 넘겨줄 수 있는 상태가 되었다.
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetBackBufferView()
{
	
	return GetRTV(_swapChain->GetCurrentBackBufferIndex());
}
