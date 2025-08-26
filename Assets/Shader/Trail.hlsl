cbuffer ConstantBuffer : register(b0)
{
    float4x4 gVP;
}

struct VSIn
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct VSOut
{
    float4 sv : SV_POSITION;
    float4 color : COLOR;
};

VSOut VS(VSIn input)
{
    VSOut output;
    output.sv = mul(float4(input.position, 1), gVP);
    output.color = input.color;

    return output;
}

float4 PS(VSOut input) : SV_TARGET
{
    return input.color;
}
