#include "pch.h"
#include "RootSignature.h"

void RootSignature::Init(ComPtr<ID3D12Device> device)
{
	/*
	  CD3DX12_ROOT_SIGNATURE_DESC(D3D12_DEFAULT) 함수는 d3dx12.h 헤더파일에 선언되어 있으며,
	  정말로 기본적인 서명이 들어가게 된다.
	  정말로 땅만 보고올게~ 정도의의..아주 최소한의 서명

	  다른 버전도 있다. 땅을 임대할 수 있는등의..
	*/
	D3D12_ROOT_SIGNATURE_DESC sigDesc = CD3DX12_ROOT_SIGNATURE_DESC(D3D12_DEFAULT);
	sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // 입력 조립기 단계

	ComPtr<ID3DBlob> blobSignature;
	ComPtr<ID3DBlob> blobError;

	::D3D12SerializeRootSignature(&sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blobSignature, &blobError);
	device->CreateRootSignature(0, blobSignature->GetBufferPointer(), blobSignature->GetBufferSize(), IID_PPV_ARGS(&_signature));
}
