#include "pch.h"
#include "ConstantBuffer.h"
#include "Engine.h"

ConstantBuffer::ConstantBuffer()
{
}

ConstantBuffer::~ConstantBuffer()
{
	if (_cdvBuffer)
	{
		if (_cdvBuffer != nullptr)
		{
			_cdvBuffer->Unmap(0, nullptr);
		}
		_cdvBuffer = nullptr;
	}
}

void ConstantBuffer::Init(uint32 size, uint32 count)
{
	// 상수 버퍼는 256 바이트 배수로 만들어야 한다.
	// 0 256, 512, 768 , ...
	/*
		256 배수로 만들어지게끔 비트연산 0000 0000 1111 1111 -> 255 인데, 
		~255는 1111 1111 0000 0000 이고
		무조건 256 의 배수가 되는것이다.

		그리고 만약 128 을 입력했을때 최소 256 을 세팅해야 하니 +255 를 통해
		입력한 값보다 부족하지는 않게, 256 의 배수로 맞추었다.
	*/ 
	
	_elementSize = (size + 255) & ~255; 
	_elementCount = count;

	CreateBuffer();
	CreateView();
}



void ConstantBuffer::CreateBuffer()
{
	// GPU 메모리 공간 확보
	uint32 bufferSize = _elementSize * _elementCount;
	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_cdvBuffer)
	);

	_cdvBuffer->Map(0, nullptr, reinterpret_cast<void**>(&_mappedBuffer));
	// gpu 가 건드리고 있을때는 버퍼를 건드리면 문제가 생기니 주의해야한다.
	// gpu 가 버퍼를 읽고 있는데 버퍼를 쓰려고 한다면 문제가 생길수 있다.
	// 동기화 생각해봐야한다.
}

D3D12_CPU_DESCRIPTOR_HANDLE ConstantBuffer::GetCpuHandle(uint32 index)
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(_cpuHandleBegin, index * _handleIncrementSize);
	/* CD3DX12_CPU_DESCRIPTOR_HEANDLE 은 아래의 식으로 계산할수도 있다.
		_cpuHandleBegine.ptr += index * _handleIncrementSize;

		결국 D3D12_CPU_DESCRIPTOR_HANDLE 을 받아서 offset 을 적용한
		새로운 D3D12_CPU_DESCRIPTOR_HNADLE 을 만드는 함수이다.

		CPU 가 찾아갈때는 CPU handle
		GPU 가 찾아갈때는 GPU handle
		
	*/
}

void ConstantBuffer::CreateView()
{
	// 저장소 만들기
	D3D12_DESCRIPTOR_HEAP_DESC cbvDesc = {};
	cbvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	cbvDesc.NodeMask = 0;
	cbvDesc.NumDescriptors = _elementCount;
	cbvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	DEVICE->CreateDescriptorHeap(&cbvDesc, IID_PPV_ARGS(&_cbvHeap));


	// 원하는 위치에 view 를 넣어주기 위해 주소 계산을 윟나 변수들
	_cpuHandleBegin = _cbvHeap->GetCPUDescriptorHandleForHeapStart();
	_handleIncrementSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV); // GPU 사양마다 조금씩 다르기 때문에 이런식으로 들고온다고 한다.

	for (uint32 i = 0; i < _elementCount; ++i)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE cbvHandle = GetCpuHandle(i);

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = _cdvBuffer->GetGPUVirtualAddress() + static_cast<uint64>(_elementSize) * i;
		cbvDesc.SizeInBytes = _elementSize; // CB size is required to be 256 byte aligned

		DEVICE->CreateConstantBufferView(&cbvDesc, cbvHandle); // cbvHandle 위치에다가 view 를 만들어주세요.
		// 각각의 view 는 GPU 의 어떠한 지점을 가리키고 있고, descriptor heap 은 이들을 CPU 주소로 접근한다.

		// CPUAddr1-DescriptorHANDLE1, CPUAddr2-DescriptorHANDLE2 ===> GPUAddr1-ConstantBufferView1, GPUAddr2-ConstantbufferView2, ...
	}
	//책갈피 <- 여기 까지 햇다. 
	/*
		일단.
		constant buffer view 용 descriptor heap 을 만들었고,
		constant buffer view 를 생성해 채워 넣었다.
	*/
}

void ConstantBuffer::Clear()
{
	_currentIndex = 0;
}

D3D12_CPU_DESCRIPTOR_HANDLE ConstantBuffer::PushData(int32 rootParamIndex, void* buffer, uint32 size)
{
	assert(_currentIndex < _elementSize);

	::memcpy(&_mappedBuffer[_currentIndex * _elementSize], buffer, size);

	// 데이터를 넣은 핸들
	D3D12_CPU_DESCRIPTOR_HANDLE pushedCPUHandle = GetCpuHandle(_currentIndex);

	// RootSignature 의 타입에 따라 달리합니다.
	switch (ROOTSIGNATURETYPE)
	{
	case (uint32)ROOTSIGNATURE_TYPE::USE_CONSTANTBUFFERVIEW: {

		D3D12_GPU_VIRTUAL_ADDRESS address = GetGpuVirtualAddress(_currentIndex);
		CMD_LIST->SetGraphicsRootConstantBufferView(rootParamIndex, address); // 각 슬롯에 버퍼의 주소를 넣는다.

		break;
	}
	case (uint32)ROOTSIGNATURE_TYPE::USE_ROOTTABLE: {

		// 넣은 주소를 table Descriptor heap 에게 전달해 주어야 한다.
		// 데이터를 넣은 위치를 뽑아.
		
		break;
	}
	default: {

		break;
	}
	}

	
	_currentIndex++;
	return pushedCPUHandle;

}

D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetGpuVirtualAddress(uint32 index)
{
	D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = _cdvBuffer->GetGPUVirtualAddress();
	objCBAddress += index * _elementSize;
	return objCBAddress;
}

