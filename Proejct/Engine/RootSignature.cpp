#include "pch.h"
#include "RootSignature.h"

void RootSignature::Init(ComPtr<ID3D12Device> device)
{
	// case 1
	/*
	  CD3DX12_ROOT_SIGNATURE_DESC(D3D12_DEFAULT) 함수는 d3dx12.h 헤더파일에 선언되어 있으며,
	  정말로 기본적인 서명이 들어가게 된다.
	  정말로 땅만 보고올게~ 정도의의..아주 최소한의 서명

	  다른 버전도 있다. 땅을 임대할 수 있는등의..
	*/
	// D3D12_ROOT_SIGNATURE_DESC sigDesc = CD3DX12_ROOT_SIGNATURE_DESC(D3D12_DEFAULT);
	// sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // 입력 조립기 단계

	// case 2
	/*
		InitAsConstantBufferView(
			UINT shaderRegister, // 레지스터 슬롯 지정, shader 프로그램은 이 레지스터 슬롯을 찾아 값을 가져간다.
			UINT ragisterSpace,  // 같은 레지스터 슬롯이라도 채널을 달리 줄 수 있다.
			D3D12_Shader_Visibility visibility // 특정 렌더링파이프라인 단계에서만 보이게만 할수도 있지만,기본적으로 모든 단계에서 활용할 수 있게끔 D3D12_SHADER_VISIBILITY_ALL 로 지정되어 있다.
	*/
	CD3DX12_ROOT_PARAMETER param[2]; // shader 프로그램 함수 에 들어갈 인수 는 2개
	param[0].InitAsConstantBufferView(0); // -> 레시즈터 슬롯은 b0, 0번에는 ConstantBufferView 로 초기화
	param[1].InitAsConstantBufferView(1); // -> 레지스터 슬롯 b1, 1번도 ConstantBufferView 로 초기화
	// 함수 시그니쳐로 표현하자면 ( 본인생각 )
	// void shaderProgramFunction( constantBufferView b0, constantBufferView b1 ) 이런느낌일까?
	// 레지스터의 b0 에서 , b1 에서 각각의 값을 찾아 셰이더 프로그램에서 갖다 쓸 수 있도록한다.
	// 루트 테이블 ( 인수 테이블 )
	// [0] constantBufferView [b0] <- shader 에서는 레지스터 슬롯 b0 에 있는 값을 갖다 쓸것이다.
	// [1] constantBufferView [b1] <- shader 에서는 레지스터 슬롯 b1 에 있는 값을 갖다 쓸것이다.

	D3D12_ROOT_SIGNATURE_DESC sigDesc = CD3DX12_ROOT_SIGNATURE_DESC(2,param);
	sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // 입력 조립기 단계


	ComPtr<ID3DBlob> blobSignature;
	ComPtr<ID3DBlob> blobError;

	::D3D12SerializeRootSignature(&sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blobSignature, &blobError);
	device->CreateRootSignature(0, blobSignature->GetBufferPointer(), blobSignature->GetBufferSize(), IID_PPV_ARGS(&_signature));
}
