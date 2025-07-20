//グローバル
cbuffer global
{
	matrix g_mWVP; //ワールドから射影までの変換行列
};

//バーテックスバッファー出力構造体
struct VS_OUTPUT
{	
    float4 Pos : SV_POSITION;
};
//
//バーテックスシェーダー
//
VS_OUTPUT VS( float4 Pos : POSITION )
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.Pos = mul( Pos, g_mWVP );

    return output;
}

//
//ピクセルシェーダー
//
float4 PS( VS_OUTPUT input ) : SV_Target
{
	return float4( 1.0f, 0.0f, 0.0f, 1.0f );
}

//テクニックは不要。　どのシェーダーを使うかアプリ側で指定するので。