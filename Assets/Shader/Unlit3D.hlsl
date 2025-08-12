#include "3DCommon.hlsli"


VS_OUT VS(float4 position : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD)
{
    VS_OUT outData;

    outData.position = mul(position, g_matrixWVP);

    // ñ@ê¸ÇÃïœå`
    normal.w = 0;
    outData.normal = mul(normal, g_matrixNormalTrans);

    float4 worldPosition = mul(position, g_matrixW);
    outData.eye = normalize(g_cameraPosition - worldPosition); // í∏ì_Ç÷ÇÃéãê¸
    outData.uv = uv;
    
    return outData;
}

float4 PS(VS_OUT inData) : SV_Target
{
    float4 color;
    if (g_hasTexture == true)
    {
        color = g_texture.Sample(g_sampler, inData.uv);

    }
    else
    {
        color = float4(1, 1, 1, 0.3);
    }
    
    return color;

}
