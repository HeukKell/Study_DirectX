// 단계별로 뭘 해야할지 일 내용을 기술합니다.

struct VS_IN
{
    float3 pos : POSITION;
    float4 color : COLOR;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 color : COLOR;
};


// vertex shader 에서 뭘 해야할지
/*
    일반 CPP 문법과 비슷하다
    이름을 바꿔줄 수도 있다.
*/
VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    output.pos = float4(input.pos, 1.f);
    output.color = input.color;

    return output;
}

// pixel shaer 에서 뭘 해야할지
float4 PS_Main(VS_OUT input) : SV_Target
{
    return input.color;
}