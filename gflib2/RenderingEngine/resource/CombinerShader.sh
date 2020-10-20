// 定数バッファのデータ定義①　変更しないデータ
cbuffer cbNeverChanges : register(b0) { // 常にスロット「0」を使う
    matrix Projection;   // 透視変換行列
};

// 定数バッファのデータ定義②　変更頻度の低いデータ
cbuffer cbChangesEveryFrame : register(b1) { // 常にスロット「1」を使う
    matrix View;   // ビュー変換行列
};

// 定数バッファのデータ定義③　変更頻度の高いデータ
cbuffer cbChangesEveryObject : register(b2) { // 常にスロット「2」を使う
    matrix World;      // ワールド変換行列
};

// 頂点シェーダの入力データ定義
struct VS_INPUT {
		float4 Pos  : POSITION0;
		float4 Norm : NORMAL;
		float4 BiNorm : BINORMAL;
		float4 Tangent : TANGENT;
		float4 Uv   : TEXCOORD0;
		float4 Col					: COLOR;
		float4 BlendIdx			: BLENDINDICES;
		float4 BlendWeight	: BLENDWEIGHT;
};

// ピクセル シェーダの入力データ定義
struct PS_INPUT {
    float4 Pos  : SV_POSITION; // 頂点座標(透視座標系)
    float4 Norm : NORMAL;      // 法線ベクトル(ワールド座標系)
    float4 Col  : COLOR;       // 頂点色
		float4 Uv   : TEXCOORD0;
};

Texture2D		g_Texture : register( t0 );
SamplerState	g_Sampler : register( s0 );

// 頂点シェーダの関数
PS_INPUT VS(VS_INPUT input) 
{
	PS_INPUT output;

	// 頂点座標　モデル座標系→ビュー座標系
	output.Pos = mul(input.Pos, World);
	output.Pos = mul(output.Pos, View);
	// 頂点座標　ビュー座標系→射影座標系
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
