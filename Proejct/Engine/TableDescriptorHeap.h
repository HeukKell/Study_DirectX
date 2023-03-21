#pragma once
class TableDescriptorHeap
{
private:

	ComPtr<ID3D12DescriptorHeap>	_descHeap;
	uint64							_handleSize = 0;
	uint64							_groupSize = 0;
	uint64							_groupCount = 0;

	uint32							_currentGroupIndex = 0;

public:
	void Init(uint32 count);

private:
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32 reg);

public:

	void Clear();
	void SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, CBV_REGISTER reg);
	/**
		레지스터 메모리의 루트 테이블 쪽으로 데이터 보내는함수	
		commit 이라는게 레지스터 메모리 쪽으로 올려보내는것을 의미하는 경우가 있나보다. 아무튼
	*/
	void CommitTable();

	ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() { return _descHeap; }

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(CBV_REGISTER reg);

};

