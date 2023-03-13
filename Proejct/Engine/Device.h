#pragma once

// 인력사무소
// GPU 와 상호작용할 역할을 담당
// 각종 객체 생성 등..
class Device
{

public:


private:

	/*
		Direct X 는 
		GPU 를 제어하고
		프로그래밍 하는데 쓰이는 저수준 그래픽 API 다.

		우리는 Direct 만 신경쓰고,
		마이크로소프트와 GPU 협력사들이 
		Direct 기준에 따라 제조를 한다.

		그것을 가능하게 하는 기술중 하나가
		COM 인데.

		COM (Component Object Model)
		- DX 의 프로그래밍 언어 독립성과 하위 호환성을 가능하게 하는 기술
		- COM 객체 (COM 인터페이스) 를 사용. 세부사항은 우리에게 숨겨짐
		- ComPtr 은 일종의 스마트 포인터다.
		- 일종의 스마트 포인터로 COM 객체를 관리하고 있다.

		
		COM 객체를 통해
		GPU 의 세부적인 부분은 알지 못하지만
		GPU에 접근을 하고 무언가를 시킬 수 있다는 장점을 가진다.

		ComPtr 은 COM객체를 받아주는 일종의 스마트 포인터역할을 한다. 
		Reference 를 관리하고 있다.
	*/ 
	ComPtr<ID3D12Debug>		_debugController;
	ComPtr<IDXGIFactory>	_dxgi; // 화면관련 기능들
	ComPtr<ID3D12Device>	_device; // 각종 객체 생성

public:

	void Init();

	ComPtr<IDXGIFactory> GetDXGI() { return _dxgi; }
	ComPtr<ID3D12Device> GetDevice() { return _device; }
};

