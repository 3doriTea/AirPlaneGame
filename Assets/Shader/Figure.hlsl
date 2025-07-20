/*
 * 指定の位置に単色の図形を描画するためのシェーダ
*/

/*
 * コンスタントバッファ(constant buffer)
 * DirectX から送られてくる情報(ポリゴン頂点を除く)
*/
cbuffer global
{
    float4 g_color;  // 色
    matrix g_matrixWorld;  // 頂点座標変換行列
};

/*
 * 頂点(Vertex)シェーダの出力
*/
struct VSOutput
{
    float4 position : SV_POSITION;
};

VSOutput VS(float4 position : POSITION)
{
    VSOutput output;
    output.position = mul(position, g_matrixWorld);

    // TODO: カメラ座標の計算もする
    return output;
}

float4 PS(VSOutput input) : SV_Target
{
    return g_color;
}
