// �ܰ躰�� �� �ؾ����� �� ������ ����մϴ�.

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


// vertex shader ���� �� �ؾ�����
/*
    �Ϲ� CPP ������ ����ϴ�
    �̸��� �ٲ��� ���� �ִ�.
*/
VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    output.pos = float4(input.pos, 1.f);
    output.color = input.color;

    return output;
}

// pixel shaer ���� �� �ؾ�����
float4 PS_Main(VS_OUT input) : SV_Target
{
    return input.color;
}