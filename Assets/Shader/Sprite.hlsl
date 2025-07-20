#include "2DCommon.hlsli"

/*
* 頂点シェーダ
*/
VSOutput VS(float4 position : POSITION, float4 uv : TEXCOORD)
{
    VSOutput output;
    output.position = mul(position, g_matrixWorldRotation);
    output.position = mul(output.position, g_matrixCameraRotation);
    output.position = mul(output.position, g_matrixWorldTranslate);
    output.position = mul(output.position, g_matrixCameraTranslate);
    output.uv = mul(uv, g_matrixTexture);
    //output.g_position = position.xy;

    return output;
}

/*
* ピクセルシェーダ
*/
float4 PS(VSOutput input) : SV_Target
{
    float4 color = g_color * g_texture.Sample(g_sampler, input.uv);

    return color;
}
