#pragma once

/**
	[계약서 / 결재]
	CPU [    ] -> GPU [    ]
	사용하고 있는 공간이 완전 다르다.
	시스템 메모리가 완전 다르다
	RAM 에 있는 것을 CPU 에서 꺼내 쓰고, 즉 CPU 랑 RAM 은 굉장히 친밀한 사이이다.
	하지만 GPU 의 경우 RAM 에 있는것을 꺼내 쓰기에는 안맞다. 굉장히 멀리 있다.

	결국 외주를 주고 싶을떄는 CPU 에 있는 데이터를 어떻게든 GPU 에게 전달한 다음에 그곳에 있는 데이터를 토대로 GPU 가 작업을 진행 해야 한다.

	예시1 )
	한국 과 베트남이 있다.
	한국의 원석을 베트남이 가공을 하길 원한다면,
	한국의 원석을 베트남에 보낸다음 베트남은 받은 원석을 가공을 진행한후, 한국에게 보내야 한다.
	
	결국에는 GPU 에도 공간이 있는데, GPU 의 어느 공간을 임대해서 쓰겠다고 계약을 해야한다.
	이를 확실하게 서명을 한다.
	어떠어떠한 레지스터, 어떠한 버퍼, 어떠한 정책, 어떠한 땅을 임대할 것인지 계약을 쓰는 느낌이다.
	GPU 메모리와 관련된 부분

	ComPtr 로 되어 있는 거의 대부분의 것들은
	GPU 에게 요청을 하는 것이고, GPU 내부에서 실질적인 객체가 만들어지는 것이다.
	
	ComPtr 로 생성한 변수, 그러니까 포인터 변수는 CPU 메모리에 할당되어 있으며,
	해당 객체가 나중에 GPU 명령을 생성하거나, 그래픽 리소스에 액세스 하는 등의 동작을 수행할 때 GPU 메모리에 접근하게 됩니다.

	ComPtr을 사용하여 생성한 포인터 변수는 CPU 메모리에 할당되며,
	해당 포인터 변수가 가리키는 COM 객체는 GPU 내부 또는 CPU와 공유하는 메모리에 할당됩니다.
	예를 들어, ComPtr<ID3D12RootSignature>을 생성하면 
	포인터 변수는 CPU 메모리에 할당되지만,
	ID3D12RootSignature 객체는 GPU 내부의 메모리에 할당됩니다.
	이는 DirectX 12의 경우 CPU와 GPU 간의 메모리 공유가 가능하기 때문입니다.
	따라서, ComPtr을 사용하여 포인터 변수를 생성하면
	실제 객체는 GPU 내부 또는 공유 메모리에 할당되며,
	CPU에서는 해당 객체를 포인터 변수를 통해 접근할 수 있습니다.
*/ 
class RootSignature
{
public:

private:
	// 서명의 정보가 _signature 에 들어있다.
	ComPtr<ID3D12RootSignature> _signature;

public:

	void Init(ComPtr<ID3D12Device> device);
	
	ComPtr<ID3D12RootSignature> GetSignature() { return _signature; }

};

