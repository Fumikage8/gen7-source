cbuffer cbNeverChanges : register(b0) {
    matrix Projection;
};

cbuffer cbNeverChanges : register(b1) {
    matrix View;
};

cbuffer cbNeverChanges : register(b2) {
    matrix World;
};

// 頂点シェーダの入力データ定義
struct VS_INPUT {
    float4 Pos : POSITION0;   // 頂点座標(モデル座標系)
		float4 Uv  : TEXCOORD0;     
    float4 Col : COLOR0;      // 頂点色
};

// ピクセル シェーダの入力データ定義
struct PS_INPUT {
    float4 Pos  : SV_POSITION; // 頂点座標(透視座標系)
    float3 Norm : NORMAL;      // 法線ベクトル(ワールド座標系)
    float4 Col  : COLOR;       // 頂点色
	float4 Uv   : TEXCOORD0;     
};

Texture2D		g_Texture : register( t0 );
SamplerState	g_Sampler : register( s0 );

// 頂点シェーダの関数
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;

	output.Pos = mul(input.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	// 頂点色
	output.Col = input.Col;

	output.Uv = input.Uv;
	// 出力
	return output;
}


// ピクセル シェーダの関数
float4 PS(PS_INPUT input) : SV_TARGET
{
	return g_Texture.Sample( g_Sampler, input.Uv.xy );
}
