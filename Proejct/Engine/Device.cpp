#include "pch.h"
#include "Device.h"

void Device::Init()
{
	// IIP_PPV_ARGS()
	// 아이디와 포인터를 어떻게든 꺼내주는 역할. Com 객체에 대한 포인터를 초기화 한다.

	
	/* D3D12 디버그층 활성화
		추가적인 정보를 디버그에 넣어준다
		- VC++ 출력상에 상세한 디버깅 메시지 출력
		- riid : 디바이스의 COM ID
		- ppDevice : 생성된 장치가 매개변수에 설정
	*/
#ifdef _DEBUG
	::D3D12GetDebugInterface(IID_PPV_ARGS(&_debugController));
	_debugController->EnableDebugLayer();
#endif

	/*
		DXGI (DirectX Graphic Infrastructure)
		device 에 모든 기능이 들어가 있는것은 아니기에, 화면 디스플레이와 관련된 기능은 DXGI 에 들어가 있다.
		
		Direct3D 와 함께 쓰이는 API
		- 전체화면 모드 전환
		- 지원되는 디스플레이 모드 열거 등
		CreateDXGIFactory
		- riid : 디바이스의 COM ID
		- ppDevice : 생성된 장치가 매개변수에 설정
	*/
	::CreateDXGIFactory(IID_PPV_ARGS(&_dxgi));


	/*
		CreateDevice
		- 디스플레이 어댑터 (그래픽카드) 를 나타내는 객체 ( 대장,boss )
		- pAdapter : nullptr 지정하면 시스템 기본 디스플레이 어댑터
		- MinimumFeatureLevel : 응용프로그램이 요구하는 최소 기능 수준 ( 오래된 모델 걸러낸다 ). 최소사양 설정하는것이다. direct 11 을 최소한으로 들고 있어야 하도록 설정
		- riid : 디바이스 COM ID
		- ppDevice : 생성된 장치가 매개변수에 설정
	*/
	::D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&_device));
}
