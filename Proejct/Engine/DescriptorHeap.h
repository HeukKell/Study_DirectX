#pragma once


/**

기안서
- 외주를 맡길 때 이런 저런 정보들을 같이 넘겨줘야 하는데, 아무런 형태로나 요청하면 알아듣지 못한다.
- 각종 리소스를 어떤 용도로 사용해야 하는지 꼼꼼하게 적어서 넘겨주는 용도
- 규격, 양식
- DX12 에서 추가된 개념
- DX11 이전에서는 View 라는 용어로 활용했지만, DX12 에서는 서술자Heap 이라고 부른다
- 리소스를 한번더 랩핑해서, 그 리소스가 무엇인지 적은 기안서를 제출하는 방식

- GPU 한테 일을 맡길때 GPU 가 알수 있게 RenderTarget을 서술을추가하여 랩핑하는 클래스
*/
class DescriptorHeap
{
public:


private:
	 
	// rtv : Render Target View
	
	ComPtr<ID3D12DescriptorHeap>	_rtvHeap;
	uint32							_rtvHeapSize = 0;
	D3D12_CPU_DESCRIPTOR_HANDLE		_rtvHandle[SWAP_CHAIN_BUFFER_COUNT];

	shared_ptr<class SwapChain>		_swapChain;
	  
	
public:

	void Init(
		ComPtr<ID3D12Device> device,
		shared_ptr<class SwapChain> swapChain
	);

	// idx 에 따라 RTV 를 꺼내주는 함수
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTV(int32 idx) { return _rtvHandle[idx];}

	D3D12_CPU_DESCRIPTOR_HANDLE GetBackBufferView();


};

