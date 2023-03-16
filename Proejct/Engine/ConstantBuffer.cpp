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

void ConstantBuffer::Clear()
{
	_currentIndex = 0;
}

void ConstantBuffer::PushData(int32 rootParamIndex, void* buffer, uint32 size)
{
	assert(_currentIndex < _elementSize);

	::memcpy(&_mappedBuffer[_currentIndex * _elementSize], buffer, size);

	D3D12_GPU_VIRTUAL_ADDRESS address = GetGpuVirtualAddress(_currentIndex);
	CMD_LIST->SetGraphicsRootConstantBufferView(rootParamIndex, address); // 각 슬롯에 버퍼의 주소를 넣는다.
	_currentIndex++;

}

D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetGpuVirtualAddress(uint32 index)
{
	D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = _cdvBuffer->GetGPUVirtualAddress();
	objCBAddress += index * _elementSize;
	return objCBAddress;
}

