Texture2D g_texture : register(t0);  // テクスチャ
SamplerState g_sampler : register(s0);  // テクスチャのサンプラ

cbuffer global
{
    matrix g_matrixWVP;  // ワールド・ビュー・プロジェクションの合成行列
    matrix g_matrixNormalTrans;  // 法線の変換行列 (回転行列)
    matrix g_matrixW;    // ワールド変換行列
    float4 g_lightDir;  // 環境光線
    float4 g_diffuseColor;  // マテリアルの色
    float4 g_ambientColor;  // 影の色
    float4 g_speculerColor;  // スペキュラーからの色
    float4 g_cameraPosition;  // 視点 (カメラの座標)
    float g_shuniness;  // ハイライトの強さ
    bool g_hasTexture;  // テクスチャを持っているか
};

struct VS_OUT
{
    float4 position : SV_POSITION;  // 位置
    float4 normal : NORMAL0;  // 法線
    float2 uv : TEXCOORD; // uv座標
    float4 eye : NORMAL1;
};
