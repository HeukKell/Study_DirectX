#pragma once

/**
	정점으로 이루어진 물체
	Mesh 라는건 정점들로 이루어져 있다.
*/
class Mesh
{
public:

	void Init(vector<Vertex>& vec);
	void Render();

private:

	ComPtr<ID3D12Resource> _vertexBuffer;
	
	// View 라는 형태로 리소스를 가리키고 설명한다.
	D3D12_VERTEX_BUFFER_VIEW _vertexBufferView = {};
	
	uint32 _vertexCount = 0;
};

