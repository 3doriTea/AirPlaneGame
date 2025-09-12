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
    outData.eye = normalize(g_cameraPosition - worldPosition);  // 頂点への視線
    // UV座標
    outData.uv = uv;
    
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
    shade.a = 1;  // 透明度は操作したくないため、強制的にアルファ値1
    
    
    float4 diffuse;
    if (g_hasTexture == true)
    {
        //diffuse = g_diffuseColor;
        // テクスチャ
        diffuse = g_texture.Sample(g_sampler, inData.uv);

    }
    else
    {
        // 拡散反射成分
        diffuse = g_diffuseColor;
        //diffuse = float4(255, 1, 1, 0.5);
    }
    
    // 環境光
    //float4 ambient = g_ambientColor;
    float4 ambient = float4(1,1,1,1);
    
    // 鏡面反射成分 (いったん0に)
    float4 specuer = float4(0, 0, 0, 0);
    //g_speculerColor //float4(255, 0, 0, 0);
    if (g_speculerColor.a != 0)
    {
        // 正反射ベクトル
        float4 r = reflect(lightDir, inData.normal);
        // 鏡面反射成分計算
        specuer = pow(saturate(dot(r, inData.eye)), g_shuniness) * g_speculerColor;
    }
    
    //return float4(1, 0, 0, 1);
    //return diffuse * shade + diffuse * g_ambientColor + specuer;
    //float4 color = diffuse * shade + g_ambientColor + specuer;
    //color.a = 1;
    
    // 最終的な色
    float4 color = diffuse * shade + diffuse * ambient + specuer;
    return color;

}
