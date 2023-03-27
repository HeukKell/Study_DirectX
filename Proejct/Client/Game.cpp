#include "pch.h"
#include "Game.h"
#include "Engine.h"
#include "Client.h"

shared_ptr<Mesh> mesh = make_shared<Mesh>();
shared_ptr<Shader> shader = make_shared<Shader>();

// index buffer 의 사용여부
#define UseIndexBuffer 


void Game::Init(const WindowInfo& window)
{
	GEngine->Init(window);



// 삼각형 만들기 :: vertex buffer & index buffer 를 사용하는 방법과 vertex buffer 만 사용하는 방법
#ifdef UseIndexBuffer //vertex buffer & index buffer 를 사용하는 방법

	/*
		정점 정보와 indexBuffer 를 사용하는 방법으로
		정점을 중복해서 쓰지 않는다.
		삼각형이 어떻게 연결되어 있는지는 index buffer 로 표현해주게 된다.
	*/

	vector<Vertex> vec(4);
	vec[0].pos = Vec3(-0.5f, 0.5f, 0.5f);
	vec[0].color = Vec4(1.f, 0.f, 0.f, 1.f);
	vec[1].pos = Vec3(0.5f, 0.5f, 0.5f);
	vec[1].color = Vec4(0.f, 1.f, 0.f, 1.f);
	vec[2].pos = Vec3(0.5f, -0.5f, 0.5f);
	vec[2].color = Vec4(0.f, 0.f, 1.f, 1.f);
	vec[3].pos = Vec3(-0.5f, -0.5f, 0.5f);
	vec[3].color = Vec4(0.f, 1.f, 0.f, 1.f);

	/* 삼각형이 그려져야 하는 순서대로 입력해주자
		16 또는 32 비트 정도면 충분하다.
		삼각형 하나의 의 경우 12바이트 정도면 그릴수 있다.
	*/
	vector<uint32> indexVec;
	{
		indexVec.push_back(0);
		indexVec.push_back(1);
		indexVec.push_back(2);
	}
	{
		indexVec.push_back(0);
		indexVec.push_back(2);
		indexVec.push_back(3);
	}


	mesh->Init(vec, indexVec);
	
#else // vertex buffer 만 사용하는 방법
	/*
		똑같은 정점 정보를 중복해서 사용하고 있다.
		인덱스 버퍼를 사용할때보다 50% 더 데이터양이 많아진다.
	*/
	vector<Vertex> vec(6);
	vec[0].pos = Vec3(-0.5f, 0.5f, 0.5f);
	vec[0].color = Vec4(1.f, 0.f, 0.f, 1.f);
	vec[1].pos = Vec3(0.5f, 0.5f, 0.5f);
	vec[1].color = Vec4(0.f, 1.f, 0.f, 1.f);
	vec[2].pos = Vec3(0.5f, -0.5f, 0.5f);
	vec[2].color = Vec4(0.f, 0.f, 1.f, 1.f);

	vec[3].pos = Vec3(0.5f, -0.5f, 0.5f);
	vec[3].color = Vec4(0.f, 0.f, 1.f, 1.f);
	vec[4].pos = Vec3(-0.5f, -0.5f, 0.5f);
	vec[4].color = Vec4(0.f, 1.f, 0.f, 1.f);
	vec[5].pos = Vec3(-0.5f, 0.5f, 0.5f);
	vec[5].color = Vec4(1.f, 0.f, 0.f, 1.f);

	mesh->Init(vec);

#endif // UseIndexBuffer

	shader->Init(L"..\\Resources\\Shader\\default.hlsli");

	//혹시 동기화가 안됬다면 기다려 주는것
	GEngine->GetCmdQueue()->WaitSync();
}

void Game::Update()
{
	bool isTest = true;

	if (isTest)
	{
		GEngine->RenderBegin();

		shader->Update();
		/*mesh->Render();*/

		{
			Transform t;
			t.offset = Vec4(0.0f, 0.f, 0.f, 0.f);
			mesh->SetTransform(t);

			mesh->Render();
		}
		//{
		//	Transform t;
		//	t.offset = Vec4(0.f, 0.75f, 0.f, 0.f);
		//	mesh->SetTransform(t);

		//	mesh->Render();
		//}

		GEngine->RenderEnd();
	}
	else {

		GEngine->Render();
	}

	
}
