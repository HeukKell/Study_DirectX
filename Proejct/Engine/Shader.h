#pragma once

/**
	[일감 기술서]
	외주 인력들이 뭘 해야 할지 기술
*/
class Shader
{
public:
	void Init(const wstring& path);
	void Update();

private:
	void CreateShader(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob, D3D12_SHADER_BYTECODE& shaderByteCode);
	void CreateVertexShader(const wstring& path, const string& name, const string& version);
	void CreatePixelShader(const wstring& path, const string& name, const string& version);

private:

	ComPtr<ID3DBlob> _vsBlob;	// 셰이더 파일을 읽기 성공하면 여기에 리소스가 만들어진다?
	ComPtr<ID3DBlob> _psBlob;	// 셰이더 파일을 읽기 성공하면 여기에 리소스가 만들어진다?
	ComPtr<ID3DBlob> _errBlob;

	ComPtr<ID3D12PipelineState> _pipelineState;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC _pipelineDesc = {};
};

