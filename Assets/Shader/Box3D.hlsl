#include "3DCommon.hlsli"

VS_OUT VS(float4 position : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD)
{
    VS_OUT outData;

    outData.position = mul(position, g_matrixWVP);

    // 法線の変形
    normal.w = 0;
    outData.normal = mul(normal, g_matrixNormalTrans);

    float4 worldPosition = mul(position, g_matrixW);
    // 視線ベクトル
    outData.eye = normalize(g_cameraPosition - worldPosition);
    
    // 法線の絶対値を取得
    float3 absNormal = abs(outData.normal.xyz);
    
    // 最も影響度の高い軸を判別して、対応するスケール値を選択
    float2 selectedScale;
    
    if (absNormal.z > absNormal.x && absNormal.z > absNormal.y)
    {
        // XY平面に垂直 → XY軸のスケールを使用
        selectedScale.x = g_textureScale.x;
        selectedScale.y = g_textureScale.y;
      
    }
    else if (absNormal.y > absNormal.x)
    {
        // XZ平面に垂直 → XZ軸のスケールを使用
        selectedScale.x = g_textureScale.x;
        selectedScale.y = g_textureScale.z;
    }
    else
    {
        // XY平面に垂直 → ZY軸のスケールを使用
        selectedScale.x = g_textureScale.z;
        selectedScale.y = g_textureScale.y;
    }
    
    // UV座標
    outData.uv = uv * selectedScale.xy;
    
    return outData;
}

float4 PS(VS_OUT inData) : SV_Target
{
    // 光源方向
    float4 lightDir = normalize(g_lightDir);
    
    // 法線
    inData.normal = normalize(inData.normal);
    
    // 
    float4 shade = saturate(dot(inData.normal, -lightDir));
    shade.a = 1;
    
    
    float4 diffuse;
    if (g_hasTexture == true)
    {
        diffuse = g_texture.Sample(g_sampler, inData.uv);
    }
    else
    {
        diffuse = g_diffuseColor;
    }
    
    // 環境光
    float4 ambient = float4(1, 1, 1, 1);
    
    // 鏡面反射成分
    float4 specuer = float4(0, 0, 0, 0);
    if (g_speculerColor.a != 0)
    {
        float4 r = reflect(lightDir, inData.normal);
        specuer = pow(saturate(dot(r, inData.eye)), g_shuniness) * g_speculerColor;
    }
    
    // 最終的な色
    float4 color = diffuse * shade + diffuse * ambient + specuer;
    return color;
}