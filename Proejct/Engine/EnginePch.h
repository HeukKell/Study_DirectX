#pragma once

// 각종 include
#include <Windows.h>
#include <tchar.h>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>

using namespace std;

#include "d3dx12.h"				// microsoft 에서 만들어주긴 한건데, 비공식적으로 딸려오는 것이라 따로 받아서 넣어줘야 한다.
#include <d3d12.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>

using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;

#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>

// 각종 lib
#pragma comment(lib,"d3d12")
#pragma comment(lib,"dxgi")
#pragma comment(lib,"dxguid")
#pragma comment(lib, "d3dcompiler")

#ifdef _DEBUG
#pragma comment(lib,"DirectXTex\\DirectXTex_Debug.lib")
#else
#pragma comment(lib,"DirectXTex\\DirectXTex_Release.lib")
#endif


// 각종 typedef

using int8		= __int8;
using int16		= __int16;
using int32		= __int32;
using int64		= __int64;
using uint8		= unsigned __int8;
using uint16	= unsigned __int16;
using uint32	= unsigned __int32;
using uint64	= unsigned __int64;
using Vec2		= XMFLOAT2;	// float 가 2개 모인것 {x,y}
using Vec3		= XMFLOAT3;	// float 가 3개 모인것 {x,y,z}
using Vec4		= XMFLOAT4; // float 가 4개 모인것 {r,g,b,a}
using Matrix	= XMMATRIX;

struct WindowInfo {
	HWND hwnd;		// 출력 윈도우
	int32 width;	// 너비
	int32 height;	// 높이
	bool windowed;	// 창모드 or 전체화면
};

struct Vertex {
	Vec3 pos;
	Vec4 color;
};

struct Transform {
	Vec4 offset ;// shader 쏙 이름이랑 같을필요 없다.
};

// ConstatntBufferView 등록 관련
enum class CBV_REGISTER {

	b0,
	b1,
	b2,
	b3,
	b4,
	END
};

/** 현재 Root Signature 를 어떤 타입을 쓰고있는지 제시하는 열거형 */
enum class EROOTSIGNATURE_TYPE {

	DEFAULT, // 기본 서명 방식
	USE_CONSTANTBUFFERVIEW, // constant buffer view 를 사용하는 방식
	USE_ROOTTABLE,	// root table 을 사용하는 방식
	END
};

enum
{
	SWAP_CHAIN_BUFFER_COUNT = 2,
	CBV_REGISTER_COUNT = CBV_REGISTER::END,
	REGISTERCOUNT  = CBV_REGISTER :: END, // 총 레지스터 개수, 추후에 값이 바뀔수 있음
	
	ROOTSIGNATURETYPE = EROOTSIGNATURE_TYPE::USE_CONSTANTBUFFERVIEW // 현재 사용할 루트 시그니쳐 타입
};



extern unique_ptr<class Engine> GEngine; //  전방선언와 외부선언

#define DEVICE GEngine->GetDevice()->GetDevice()
#define CMD_LIST GEngine->GetCmdQueue()->GetCmdList()
#define ROOT_SIGNATURE GEngine->GetRootSignature()->GetSignature()


void HelloEngine();