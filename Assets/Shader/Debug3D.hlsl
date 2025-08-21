#include "3DCommon.hlsli"


VS_OUT VS(float4 position : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD)
{
    VS_OUT outData;

    outData.position = mul(position, g_matrixWVP);

    return outData;
}

float4 PS(VS_OUT inData) : SV_Target
{
    return float4(1, 1, 1, 0.3);

}
