Texture2D g_texture : register(t0); // テクスチャ
SamplerState g_sampler : register(s0); // テクスチャのサンプラ


cbuffer global
{
    matrix g_matrixWVP;
    matrix g_matrixNormalTrans;
    float4 g_lightDirection;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL0;
    float2 uv : TEXCOORD;
};

VS_OUTPUT VS(float4 position : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD)
{
    VS_OUTPUT output;
    output.position = mul(position, g_matrixWVP);
    normal.w = 0;
    output.normal = mul(normal, g_matrixNormalTrans);
    output.uv = uv;
    
    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float3 n = normalize(input.normal.xyz);
    float3 l = normalize(g_lightDirection.xyz);
    
    float diffuse = saturate(dot(n, -l));
    
    float4 texColor = g_texture.Sample(g_sampler, input.uv);
    
    
    return texColor * diffuse;
    
}