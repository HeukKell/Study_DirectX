#include "pch.h"
#include "Mesh.h"
#include "Engine.h"

void Mesh::Init(const vector<Vertex>& vertexBuffer, const vector<uint32>& indexBuffer)
{
	CreateVertexBuffer(vertexBuffer);
	CreateIndexBuffer(indexBuffer);
}


void Mesh::CreateVertexBuffer(const vector<Vertex>& buffer)
{
	/*
		과정은 이러하다.
		1. GPU 공간 할당받고
		2. 할당받은 GPU 공간에 연결해 데이터 옮겨넣고
		3. View 를 작성한다. ( 데이터의 위치와 정보에 대한 Descriptor(View) 작성)
		4. Render 함수에서 View 를 전달하면서, 외주를 요청한다.

		렌더타겟의 경우와 유사하다
		리소스를 먼저 만들고, view 를 작성하고, view 를 통해 일감을 넘기는 방식
	*/
	_vertexCount = static_cast<uint32>(buffer.size());
	uint32 bufferSize = _vertexCount * sizeof(Vertex); // 각 점의 사이즈와 점의 개수의 곱



	/*
		GPU 쪽 메모리를 할당 받는다. 정점 데이터를 옮겨야 하니까.
		GPU 쪽에 할당해서 데이터를 넣어줄테니 잠시 창고를 빌려달라고 하는것이다.

		D3D12_HEAP_PROPERTIES 는 공간의 사용용도에 대해서 요청하는 타입을 설정한다.
		Upload 타입으로 한다면, cpu 에서 작업한것을 gpu 쪽에다가 넘겨주는 용도로만 활용하긴 해야하는데,
		데이터를 복사하는 용도로만 사용해야하는데, 지금은 계속해서 활용하고 있다.
		여러가지로 활용하는 사례들이 있다.
		정석적으로는 default 타입으로 버퍼를 만들어 주고, 데이터를 전송할때만 upload 타입으로 써야하는데,
		그냥 쓰고 있다.

		D3D12_RESOURCE_DESC 는 버퍼의 크기만큼 빌리고 싶다 하여 버퍼의 사이즈를 넣어준 것이고.
		
		CreateCommittedResource 함수는 GPU 에게 공간을 할당 받는다.
	*/
	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_vertexBuffer) // ram 에 있는 공간을 가리키는게 아니라 GPU 공간을 가리키고 있다.
	);

	// Copy the triangle data to the vertex buffer
	// GPU 공간에다가 데이터를 복사하기 위해 2단계에 걸쳐서 복사를 진행하는데,이와 같다.
	void* vertexDataBuffer = nullptr; // CPU 공간의 버퍼, 이곳에 데이터를 넣게 되면 GPU 쪽에도 데이터가 쭉 들어간다.
	CD3DX12_RANGE readRange(0, 0); // we do not intend to read from this resource on the CPU
	_vertexBuffer->Map(0, &readRange, &vertexDataBuffer); // 공간연결, vertextDataBuffer 를 통해서 데이터를 복사할 수 있게끔 연결을 해 준 것이다.
	::memcpy(vertexDataBuffer, &buffer[0], bufferSize); // 데이터 밀어넣고
	_vertexBuffer->Unmap(0, nullptr); // 공간 닫기. 
	// 이런식으로 GPU 쪽 메모리에 데이터를 복사가 되었다.


	// Initialize the vertex buffer view
	// View(Descriptor) 를 만들어줫는데, 어디에 위치해있는지, 크기가 얼마짜리인지 저장
	// View를 통해서 GPU 가 어떤 쪽을 얼마만큼인지 알아듣게 해주는 것이다.
	// 마치 GPU 에게 작업할 물건은 어디에 있으며 얼마만큼 있다. 라고 전달해주는 느낌
	_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
	_vertexBufferView.StrideInBytes = sizeof(Vertex); // StrideInBytes 바이트 단위의 보폭, 정점 1개의 크기
	_vertexBufferView.SizeInBytes = bufferSize; // 버퍼의 크기
}

void Mesh::CreateIndexBuffer(const vector<uint32>& buffer)
{

	_indexCount = static_cast<uint32>(buffer.size());
	uint32 buffersize = sizeof(uint32) * _indexCount;

	
	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC heapDesc = CD3DX12_RESOURCE_DESC::Buffer(buffersize);


	DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&heapDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_indexBuffer)
	);


	void* indexDataBuffer = nullptr;
	CD3DX12_RANGE readRange(0, 0); // we do not intend to read from this resource on the CPU
	_indexBuffer->Map(0, &readRange, &indexDataBuffer); // gpu 버퍼의 주소를 indexDataBuffer 로 받는것인가?
	memcpy(indexDataBuffer, &buffer[0], buffersize);
	_indexBuffer->Unmap(0, nullptr); //공간 닫기

	_indexBufferView.BufferLocation = _indexBuffer->GetGPUVirtualAddress();
	_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	_indexBufferView.SizeInBytes = buffersize;
}


void Mesh::Render()
{
	/*
		CommandQueue 의 RenderBegin 이 시작되고,
		RenderEnd 가 호출되기 전에
		그 사이의 순간에 외주명령을 추가해서 밀어 넣는것이다.

		CommandQueue::RenderBegin() -> Mesh::Render() -> CommandQueue::RenderEnd()

	*/
	CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 정점들이 어떤 형태로 연결되어 있는지, 삼각형 형태로 연결되어 있다고 알려준다.
	CMD_LIST->IASetVertexBuffers(0, 1, &_vertexBufferView); // Slot : (0~15) , 슬롯을 정해주는데, 지금은 0번으로
	CMD_LIST->IASetIndexBuffer(&_indexBufferView);



	switch (ROOTSIGNATURETYPE)
	{
	case (uint32)EROOTSIGNATURE_TYPE::USE_CONSTANTBUFFERVIEW: {

		/*	이경우 ConstantBuffer 클래스의 PushData 에서
			CMD_LIST->SetGraphicsRootConstantBufferView( 레지스터 슬롯 번호, 핸들) 을 통해 커밋을 해준다.
		*/

		// b0
		GEngine->GetConstantBuffer()->PushData(0, &_transform, sizeof(_transform)); // 정점 좌표 이동에 쓸것이다.
		// b1
		GEngine->GetConstantBuffer()->PushData(1, &_transform, sizeof(_transform)); //컬러 색상으로 쓸것이다

		break;
	}
	case (uint32)EROOTSIGNATURE_TYPE::USE_ROOTTABLE: {
		
		// shader 에 들어갈 인수 , 루트 테이블의 데이터 입력 필요.
		// TODO
		// 1) buffer 에다가 데이터를 세팅
		// 2) buffer 의 주소를 register 에다가 전송 (TableDescriptorHeap 에다가 CBV 전달)
		// 3) 모든 세팅이 끝났으면 TableDescHeap 커밋 


		// 해당 핸들을 register 에 전달해달라고 보낸다.

		// b0
		{
			D3D12_CPU_DESCRIPTOR_HANDLE pushedDataHandle = GEngine->GetConstantBuffer()->PushData(0, &_transform, sizeof(_transform)); // 정점 좌표 이동에 쓸것이다.
			GEngine->GetTableDescHeap()->SetCBV(pushedDataHandle, CBV_REGISTER::b0);
		}
		// b1
		{
			D3D12_CPU_DESCRIPTOR_HANDLE pushedDataHandle = GEngine->GetConstantBuffer()->PushData(1, &_transform, sizeof(_transform)); //컬러 색상으로 쓸것이다
			GEngine->GetTableDescHeap()->SetCBV(pushedDataHandle, CBV_REGISTER::b1);
		}

		// 레지스터에 데이터 커밋
		GEngine->GetTableDescHeap()->CommitTable();

		break;
	}
	default: {

		break;
	}
	}
	
	//CMD_LIST->DrawInstanced(_vertexCount, 1, 0, 0); // 그려주는 작업 예약
	CMD_LIST->DrawIndexedInstanced(_indexCount, 1, 0, 0, 0); // index buffer 를 통해서 그려준다.
	
}
