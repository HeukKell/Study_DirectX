#pragma once

/**
	정점으로 이루어진 물체
	Mesh 라는건 정점들로 이루어져 있다.
*/
class Mesh
{
public:

	/** 
		Mesh 의 초기화 함수
		@param vertexBuffer 정점 정보를 받습니다.
		@param indexBuffer 삼각형이 그려져야하는 순서대로 인덱스 번호를 받습니다.
	*/
	void Init(const vector<Vertex>& vertexBuffer, const vector<uint32>& indexBuffer);
	void Render();

	void SetTransform(const Transform& t) { _transform = t; }

private:

	// 정점 버퍼 뷰

	ComPtr<ID3D12Resource> _vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW _vertexBufferView = {};
	uint32 _vertexCount = 0;

	// 인덱스 버퍼 뷰 ( 삼각형이 그려져야할 순서를 저장하는 버퍼 )

	ComPtr<ID3D12Resource> _indexBuffer;
	D3D12_INDEX_BUFFER_VIEW _indexBufferView;
	uint32 _indexCount = 0;


	Transform _transform = {};

private:

	void CreateVertexBuffer(const vector<Vertex>& buffer);
	void CreateIndexBuffer(const vector<uint32>& buffer);
};

