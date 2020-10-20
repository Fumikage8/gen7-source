#define				vec2					float2
#define				vec3					float3
#define				vec4					float4
#define				mat4x4				float4x4
#define				mat3x3				float3x3
#define				inverse				transpose
#define				Math::Vector	vec4

// 頂点シェーダの入力データ定義
struct VS_INPUT {
	uint    vIndex    : SV_VertexID;
};

// ピクセル シェーダの入力データ定義
struct PS_INPUT {
    float4 Pos  : SV_POSITION; // 頂点座標(透視座標系)
    float4 Col  : COLOR;       // 頂点色
		float4 Uv0  : TEXCOORD0;     
};

Texture2D		g_Texture : register( t0 );
SamplerState	g_Sampler : register( s0 );

cbuffer PixcelShaderVectorConstants : register( b0 )
{
	Math::Vector cFsUvOffSet1;
	Math::Vector cFsUvOffSet2;
	Math::Vector cGaussianOffset[16];
	Math::Vector cMultiSampleOffset[21];
	Math::Vector cFsParam[2];
};

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
	
	output.Uv0.xy = texlist[ input.vIndex ].xy;
	
//	output.Uv0.x += (1.0f / 1280.0f * 0.5f);
//	output.Uv0.y += (1.0f / 720.0f * 0.5f);

	return output;
}


// ------------------------------------------------------------
// 
// ------------------------------------------------------------
vec4 GetNormalEdgeColor( vec4	normal[3] )
{
	vec4			constantColor = vec4(0.1, 0.1, 0.1, 0.6);
	vec4			edgeColorRGB = vec4(0.0, 0.0, 0.0, 0.0);
	vec4			edgeColorA = vec4(0.0, 0.0, 0.0, 0.0);

	//コンバイナ０
	edgeColorRGB = dot( normal[0].xyz, normal[1].xyz );
	edgeColorA = normal[0].w + ( 1.0 - normal[1].w );

	edgeColorRGB = saturate( edgeColorRGB );
	edgeColorA = saturate( edgeColorA );

	//コンバイナ１
	edgeColorRGB *= dot( normal[0].xyz, normal[2].xyz );
	edgeColorA *= normal[1].w + ( 1.0 - normal[0].w );

	edgeColorRGB = saturate( edgeColorRGB );
	edgeColorA = saturate( edgeColorA );
	//コンバイナ２
	edgeColorRGB *= edgeColorRGB;
	edgeColorRGB += constantColor;//飽和
	edgeColorA *= normal[0].w + ( 1.0 - normal[2].w );

	edgeColorRGB = saturate( edgeColorRGB );
	edgeColorA = saturate( edgeColorA );
	//コンバイナ３
	edgeColorRGB *= edgeColorRGB;
	edgeColorA *= normal[2].w + ( 1.0 - normal[0].w );

	edgeColorRGB = saturate( edgeColorRGB );
	edgeColorA = saturate( edgeColorA );
	//コンバイナ４
	edgeColorRGB *= edgeColorRGB;
	edgeColorA -= constantColor.w;

	edgeColorA *= 4.0f;

	edgeColorRGB = saturate( edgeColorRGB );
	edgeColorA = saturate( edgeColorA );
	//コンバイナ５
	edgeColorRGB *= edgeColorA;

	edgeColorRGB = saturate( edgeColorRGB );
	edgeColorA = saturate( edgeColorA );

	return edgeColorRGB;
}

// ------------------------------------------------------------
// 
// ------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_TARGET
{
	vec4	edgeColor;

	vec4	normal[16];
	vec4	clenterNormal;
	vec4	outLineColor;
	vec4	outLineColorFinal;
	vec4	edgeColorRGB = vec4(0.0, 0.0, 0.0, 0.0);
	float	edgeColorA = 0.0f;

	clenterNormal = g_Texture.Sample( g_Sampler, input.Uv0.xy );
	outLineColorFinal = vec4(0.0, 0.0, 0.0, 0.0);

	clenterNormal.xyz -= 0.5;
	clenterNormal.xyz *= 2.0;
	clenterNormal.xyz = normalize( clenterNormal.xyz );

	for( int i = 0; i < 16; ++i )
	{
		vec4					offSet = cGaussianOffset[i];
		vec4					uv = input.Uv0 + offSet;

		normal[i] = g_Texture.Sample( g_Sampler, uv.xy );
		normal[i].xyz -= 0.5;
		normal[i].xyz *= 2.0;
		normal[i].xyz = normalize( normal[i].xyz );

		if ( outLineColorFinal.w < outLineColor.w )
			outLineColorFinal = outLineColor;

		float			ans = dot( clenterNormal.xyz, normal[i].xyz );
		ans = max( ans, 0.0 );
		edgeColorRGB.xyz += ans;

		ans = clenterNormal.w - normal[i].w;
		ans = max( ans, 0.0 );

		edgeColorA = max( edgeColorA, ans );//もっとも差が大きい色を使用
	}

	edgeColorRGB /= 16;
	//edgeColorA /= 16;

	edgeColorRGB += cFsParam[0].x;//飽和
	edgeColorA -= cFsParam[0].x;//飽和

	edgeColorRGB = saturate( edgeColorRGB );
	edgeColorA = saturate( edgeColorA );

	edgeColorA = 1.0 - edgeColorA;

	edgeColor = edgeColorRGB * edgeColorA;
	//edgeColor = edgeColorA;

	edgeColor.xyzw = pow( edgeColor.x, cFsParam[0].y );
	edgeColor = saturate( edgeColor );
	edgeColor.w = 1.0f - edgeColor.w;

	return edgeColor;
}

