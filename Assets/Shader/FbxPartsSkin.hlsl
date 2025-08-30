#include "3DCommon.hlsli"

//定義
#define MAX_BONE_MATRICES 128




cbuffer BoneMatrices : register(b1) //ボーンのポーズ行列が入る
{
    matrix g_boneMatrices[MAX_BONE_MATRICES];
    bool g_hasSkinnedMesh;
    float padding[3];
};

//スキニング後の頂点・法線が入る
struct Skin
{
    float4 position;
    float3 normal;
};
//バーテックスバッファーの入力
struct VSSkinIn
{
    float3 position : POSITION; //位置   
    float3 normal : NORMAL; //頂点法線
    float2 uv : TEXCOORD; //テクスチャー座標
    uint4 boneIndex : BONE_INDEX; //ボーンのインデックス
    float4 boneWeight : BONE_WEIGHT; //ボーンの重み
};
//ピクセルシェーダーの入力（バーテックスバッファーの出力）　
struct PSSkinIn
{
    float4 position : SV_Position; //位置
    float3 normal : NORMAL; //頂点法線
    float2 uv : TEXCOORD; //テクスチャー座標
    float4 Color : COLOR0; //最終カラー（頂点シェーダーにおいての）
};


//頂点をスキニング（ボーンにより移動）する。サブ関数（バーテックスシェーダーで使用）
Skin SkinVert(VSSkinIn input)
{
    Skin Output = (Skin) 0;
    float4 pos = float4(input.position, 1.0f);
    float3 normal = input.normal;
    
    if(g_hasSkinnedMesh && any(input.boneWeight.xyz > 0.0f))
    {
        float4 skinnedPos = float4(0, 0, 0, 0);
        float3 skinnedNormal = float3(0, 0, 0);
        
        [unroll]
        for (int i = 0; i < 4; i++)
        {
            if(input.boneWeight[i] > 0.0f)
            {
                // i番目のボーンの変換行列を取得
                matrix boneMatrix = g_boneMatrices[input.boneIndex[i]];
                
                // 頂点座標を変換行列、ウェイトで変換
                Output.position += mul(boneMatrix, pos) * input.boneWeight[i];
                
                // 法線も移動成分を取り除いて変換
                Output.normal += mul((float3x3) boneMatrix, normal) * input.boneWeight[i];
            }
        }
    }
    
    ////ボーン0
    //uint iBone = input.boneIndex.x;
    //float fWeight = input.boneWeight.x;
    //matrix m = FetchBoneMatrix(iBone);
    //Output.position += fWeight * mul(position, m);
    //Output.normal += fWeight * mul(normal, (float3x3) m);
    ////ボーン1
    //iBone = input.boneIndex.y;
    //fWeight = input.boneWeight.y;
    //m = FetchBoneMatrix(iBone);
    //Output.position += fWeight * mul(position, m);
    //Output.normal += fWeight * mul(normal, (float3x3) m);
    ////ボーン2
    //iBone = input.boneIndex.z;
    //fWeight = input.boneWeight.z;
    //m = FetchBoneMatrix(iBone);
    //Output.position += fWeight * mul(position, m);
    //Output.normal += fWeight * mul(normal, (float3x3) m);
    ////ボーン3
    //iBone = input.boneIndex.w;
    //fWeight = input.boneWeight.w;
    //m = FetchBoneMatrix(iBone);
    //Output.position += fWeight * mul(position, m);
    //Output.normal += fWeight * mul(normal, (float3x3) m);
    
    return Output;
}

//
//PSSkinIn VSSkin(VSSkinIn input )
//バーテックスシェーダー
PSSkinIn VS(VSSkinIn input)
{
    PSSkinIn output;
    
    Skin vSkinned = SkinVert(input);

    output.position = mul(vSkinned.position, g_matrixWVP);
    output.normal = normalize(mul(vSkinned.normal, (float3x3) g_matrixW));
    output.uv = input.uv;
    float3 LightDir = normalize(g_lightDir);
    float3 PosWorld = mul(vSkinned.position, g_matrixW);
    float3 ViewDir = normalize(g_cameraPosition - PosWorld);
    float3 Normal = normalize(output.normal);
    float4 NL = saturate(dot(Normal, LightDir));

    float3 Reflect = normalize(2 * NL * Normal - LightDir);
    float4 specular = pow(saturate(dot(Reflect, ViewDir)), g_shuniness);

    output.Color = g_diffuseColor * NL + specular * g_speculerColor;
    
    return output;
}

//
// float4 PSSkin(PSSkinIn input) : SV_Target
//ピクセルシェーダー
float4 PS(PSSkinIn input) : SV_Target
{
    float4 TexDiffuse = g_texture.Sample(g_sampler, input.uv);

    return input.Color + TexDiffuse;
}