#include "pch.h"
#include "TableDescriptorHeap.h"
#include "Engine.h"


/**
	@param count 그룹의 개수가 됩니다.
*/
void TableDescriptorHeap::Init(uint32 count)
{

	_groupCount = count;

	// descriptor heap 생성
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	/** SHADER_VISIBLE 로 해야 GPU 메모리 상주를 하며, 
		Register 슬롯에 맞춰 등록도 가능하다고 하니 꼭, NONE 이 아니라 SHADER_VISIBLE 모드로 적용한다.
	*/ 
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; 
	desc.NodeMask = 0;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = count * REGISTERCOUNT; // 각 그룹마다 슬롯이 몇개 있는지.

	DEVICE->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_descHeap));

	_handleSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	_groupSize = _handleSize * REGISTERCOUNT;

}



void TableDescriptorHeap::Clear()
{
	_currentGroupIndex = 0;
}

void TableDescriptorHeap::SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, CBV_REGISTER reg)
{

	D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCPUHandle(reg);

	uint32 destRange = 1;
	uint32 srcRange = 1;

	// 목표핸들에 소스핸들로부터 복사. 서술자 힙
	DEVICE->CopyDescriptors(1, &destHandle, &destRange, 1, &srcHandle, &srcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

D3D12_CPU_DESCRIPTOR_HANDLE TableDescriptorHeap::GetCPUHandle(uint32 reg)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = _descHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += _currentGroupIndex * _groupSize; // 그룹을 찾아서.
	handle.ptr += reg * _handleSize; // 해당하는 레지스터의 위치를 찾는다.

	return handle;
	
}



D3D12_CPU_DESCRIPTOR_HANDLE TableDescriptorHeap::GetCPUHandle(CBV_REGISTER reg)
{
	return GetCPUHandle(static_cast<uint32>(reg));
}
	

void TableDescriptorHeap::CommitTable()
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = _descHeap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += _currentGroupIndex * _groupSize;
	CMD_LIST->SetGraphicsRootDescriptorTable(0, handle); // 루트 시그니쳐의 인수 0번에 _descHeap 의 주소 설정??

	_currentGroupIndex++;
}