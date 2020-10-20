// 頂点シェーダの入力データ定義
struct VS_INPUT {
	uint    vIndex    : SV_VertexID;
};

// ピクセル シェーダの入力データ定義
struct PS_INPUT {
    float4 Pos  : SV_POSITION; // 頂点座標(透視座標系)
    float4 Col  : COLOR;       // 頂点色
	float4 Uv   : TEXCOORD0;     
};

Texture2D		g_Texture : register( t0 );
SamplerState	g_Sampler : register( s0 );

// 頂点シェーダの関数
PS_INPUT VS(VS_INPUT input) 
{
	PS_INPUT output = (PS_INPUT)0;
	
	float4  poslist[4]= {
		{ -1.0f,  1.0f, 0.0f, 1.0f },
		{  1.0f,  1.0f, 0.0f, 1.0f },
		{ -1.0f, -1.0f, 0.0f, 1.0f },
		{  1.0f, -1.0f, 0.0f, 1.0f },
	};
	
	output.Pos = poslist[ input.vIndex ];
	
	float2  texlist[4]= {
		{    0.0f,  0.0f    },
		{    1.0f,  0.0f    },
		{    0.0f,  1.0f    },
		{    1.0f,  1.0f    },
	};
	
	output.Uv.xy = texlist[ input.vIndex ].xy;

	return output;
}


// ピクセル シェーダの関数
float4 PS(PS_INPUT input) : SV_TARGET
{
	return g_Texture.Sample( g_Sampler, input.Uv.xy );
}
