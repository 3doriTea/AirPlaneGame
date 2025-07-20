Texture2D g_texture : register(t0);  // テクスチャ
SamplerState g_sampler : register(s0);  // テクスチャのサンプラ

/*
 * コンスタントバッファ(constant buffer)
 * DirectX から送られてくる情報(ポリゴン頂点を除く)
*/
cbuffer global
{
    matrix g_matrixCameraTranslate; // カメラ変換行列
    matrix g_matrixCameraRotation;  // カメラ回転行列
    matrix g_matrixWorldTranslate;  // ワールド変換行列
    matrix g_matrixWorldRotation;   // ワールド回転行列
    matrix g_matrixTexture;         // テクスチャ座標変換行列
    float4 g_color;                 // テクスチャの合成色
    float2 g_angle;                 // テクスチャの角度
};

/*
 * 頂点(Vertex)シェーダの出力
*/
struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float2 g_position : TEXCOORD1;
};
