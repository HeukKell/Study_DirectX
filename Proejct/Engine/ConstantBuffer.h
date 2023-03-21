#pragma once


//{ {constantBufferView},{constantBufferView},{constantBufferView},... }
class ConstantBuffer
{

private:
	// 여러개의 ConstatntBufferView 관리할 시작주소를 저장하고 있을꺼야.
	ComPtr<ID3D12Resource>	_cdvBuffer; // GPU 쪽 GDDR4 쪽 메모리에 생길것이다. 
	BYTE*					_mappedBuffer = nullptr;
	uint32					_elementSize = 0;	// constant buffer view 의 각 사이즈를 저장한다. 256 byte 의 배수
	uint32					_elementCount = 0;	// 몇개의 constantBufferView 를 만들것인지,


	// RootSignature Table 관련
	ComPtr<ID3D12DescriptorHeap>	_cbvHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE		_cpuHandleBegin = {};		// 시작 주소
	uint32							_handleIncrementSize = 0;	// 몇칸을 띄어야 하는지

	uint32					_currentIndex = 0;

public:

	ConstantBuffer();
	~ConstantBuffer();

	void Init(uint32 size, uint32 count);

	void Clear();

	/**
		@param rootPAramIdex 레지스터 슬롯 번호
		@param buffer 버퍼주소
		@param size 크기
	*/
	D3D12_CPU_DESCRIPTOR_HANDLE PushData(int32 rootParamIndex, void* buffer, uint32 size);

	D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress(uint32 index);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(uint32 index);

private:

	void CreateBuffer();

	void CreateView();


};

