#include "3DCommon.hlsli"

VS_OUT VS(float4 position : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD)
{
    VS_OUT outData;

    outData.position = mul(position, g_matrixWVP);

    // 法線の変形
    normal.w = 0;
    outData.normal = mul(normal, g_matrixNormalTrans);

    float4 worldPosition = mul(position, g_matrixW);
    outData.eye = normalize(g_cameraPosition - worldPosition);  // 頂点への視線
    outData.uv = uv;
    
    return outData;
}

float4 PS(VS_OUT inData) : SV_Target
{
    float4 lightDir = normalize(g_lightDir);

    inData.normal = normalize(inData.normal);
    
    float4 shade = saturate(dot(inData.normal, -lightDir));
    shade.a = 1;  // 透明度は操作したくないため、強制的にアルファ値1
    
    float4 diffuse;
    if (g_hasTexture)
    {
        //diffuse = g_diffuseColor;
        diffuse = g_texture.Sample(g_sampler, inData.uv);
    }
    else
    {
        diffuse = g_diffuseColor;
    }
    
    float4 specuer = float4(0, 0, 0, 0);
    if (g_speculerColor.a != 0)
    {
        float4 r = reflect(lightDir, inData.normal);
        specuer = pow(saturate(dot(r, inData.eye)), g_shuniness) * g_speculerColor;
    }
    
    //return float4(1, 0, 0, 1);
    //return diffuse * shade + diffuse * g_ambientColor + specuer;
    float4 color = diffuse * shade + diffuse * g_ambientColor + specuer;
    color.a = 1;
    return color;

}
