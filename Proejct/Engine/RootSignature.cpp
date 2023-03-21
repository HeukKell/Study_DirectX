#include "pch.h"
#include "RootSignature.h"

void RootSignature::Init(ComPtr<ID3D12Device> device)
{

	D3D12_ROOT_SIGNATURE_DESC sigDesc;	

	// 루트 시그니쳐,즉 셰이더 프로그래밍 함수의 서명 방법
	switch (ROOTSIGNATURETYPE)
	{
	case (uint32)EROOTSIGNATURE_TYPE::DEFAULT : {
		// 가장 기본적인 셰이더 함수 서명을 하는경우
		/*
		  CD3DX12_ROOT_SIGNATURE_DESC(D3D12_DEFAULT) 함수는 d3dx12.h 헤더파일에 선언되어 있으며,
		  정말로 기본적인 서명이 들어가게 된다.
		  정말로 땅만 보고올게~ 정도의의..아주 최소한의 서명

		  다른 버전도 있다. 땅을 임대할 수 있는등의..
		*/
		 sigDesc = CD3DX12_ROOT_SIGNATURE_DESC(D3D12_DEFAULT);
		 sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // 입력 조립기 단계
		break;
	}
	case (uint32)EROOTSIGNATURE_TYPE::USE_CONSTANTBUFFERVIEW: {
		// 2개의 constant buffer view 를 가진 셰이더 함수 서명을 하는 경우
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

		sigDesc = CD3DX12_ROOT_SIGNATURE_DESC(2, param);
		sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // 입력 조립기 단계
		break;
	}
	case (uint32)EROOTSIGNATURE_TYPE::USE_ROOTTABLE: {
		// table 을 이용한 루트시그니쳐 설정
		// 여러개의 레지스터 슬롯을 테이블로 묶어 하나의 인수로 설정하는 방법.
		// 활성화 에 따라 사용하는 테이블이 달라질 수 있다.
		/*
			 table1 은 b0~b4 를 사용한다 하고,
			 table2 는 b5~ b8 를 사용한다 하였을때,

			 table1 을 활성화 한다고 하면, b0~b4 슬롯의 값을 모아 하나의 인수로서 사용을 하게 된다.
			 
			 여기서, 그렇다고 해도 b5~b8 슬롯이 없던게 되고 다른 것으로 바꿀수는 없다.
			 루트시그니쳐가 b0 ~ b4 를 인수로 사용한다 하였을뿐,
			 b5~b8은 이미 설정 되었다.
			 때문에 다음 슬롯은 b9 부터 시작해야 한다.
			 즉, 무슨말이냐면, 테이블을 사용하더라도, 슬롯 할당은 그대로 되고, 어떤 값을 가져올것인지 고를 뿐이다. 라는것이다.
		*/ 

		// range, 즉 범위 단위로 설정하겠다. b0 부터 b4 까지를 CBV 인수로 설정하겠다.
		CD3DX12_DESCRIPTOR_RANGE ranges[] = {
			CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, CBV_REGISTER_COUNT, 0)// b0 ~ b4 까지 쓰겠다, CBV_REGISTER_COUNT = 5
		};

		CD3DX12_ROOT_PARAMETER param[1];
		param[0].InitAsDescriptorTable(_countof(ranges), ranges);

		sigDesc = CD3DX12_ROOT_SIGNATURE_DESC(_countof(param), param);
		sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // 입력 조립기 단계

		break;
	}
	default: {
		break;
	}
	}


	
	


	ComPtr<ID3DBlob> blobSignature;
	ComPtr<ID3DBlob> blobError;

	::D3D12SerializeRootSignature(&sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blobSignature, &blobError);
	device->CreateRootSignature(0, blobSignature->GetBufferPointer(), blobSignature->GetBufferSize(), IID_PPV_ARGS(&_signature));
}
