cbuffer ConstantBuffer
{
    float4x4 gVP;
}

struct VSOut
{
    float4 sv : SV_POSITION;
    float4 color : COLOR;
};

VSOut VS(float4 position : POSITION, float4 color : COLOR)
{
    VSOut output;
    output.sv = mul(position, gVP);
    output.color = color;

    return output;
}

float4 PS(VSOut input) : SV_TARGET
{
    return input.color;
    //return float4(0, 1, 0, 1);
}
