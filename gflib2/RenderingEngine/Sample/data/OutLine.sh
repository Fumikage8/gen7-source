#define				vec2					float2
#define				vec3					float3
#define				vec4					float4
#define				mat4x4				float4x4
#define				mat3x3				float3x3
#define				inverse				transpose
#define				Math::Vector	vec4

// ���_�V�F�[�_�̓��̓f�[�^��`
struct VS_INPUT {
	uint    vIndex    : SV_VertexID;
};

// �s�N�Z�� �V�F�[�_�̓��̓f�[�^��`
struct PS_INPUT {
    float4 Pos  : SV_POSITION; // ���_���W(�������W�n)
    float4 Col  : COLOR;       // ���_�F
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

// ���_�V�F�[�_�̊֐�
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

	//�R���o�C�i�O
	edgeColorRGB = dot( normal[0].xyz, normal[1].xyz );
	edgeColorA = normal[0].w + ( 1.0 - normal[1].w );

	edgeColorRGB = saturate( edgeColorRGB );
	edgeColorA = saturate( edgeColorA );

	//�R���o�C�i�P
	edgeColorRGB *= dot( normal[0].xyz, normal[2].xyz );
	edgeColorA *= normal[1].w + ( 1.0 - normal[0].w );

	edgeColorRGB = saturate( edgeColorRGB );
	edgeColorA = saturate( edgeColorA );
	//�R���o�C�i�Q
	edgeColorRGB *= edgeColorRGB;
	edgeColorRGB += constantColor;//�O�a
	edgeColorA *= normal[0].w + ( 1.0 - normal[2].w );

	edgeColorRGB = saturate( edgeColorRGB );
	edgeColorA = saturate( edgeColorA );
	//�R���o�C�i�R
	edgeColorRGB *= edgeColorRGB;
	edgeColorA *= normal[2].w + ( 1.0 - normal[0].w );

	edgeColorRGB = saturate( edgeColorRGB );
	edgeColorA = saturate( edgeColorA );
	//�R���o�C�i�S
	edgeColorRGB *= edgeColorRGB;
	edgeColorA -= constantColor.w;

	edgeColorA *= 4.0f;

	edgeColorRGB = saturate( edgeColorRGB );
	edgeColorA = saturate( edgeColorA );
	//�R���o�C�i�T
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

		edgeColorA = max( edgeColorA, ans );//�����Ƃ������傫���F���g�p
	}

	edgeColorRGB /= 16;
	//edgeColorA /= 16;

	edgeColorRGB += cFsParam[0].x;//�O�a
	edgeColorA -= cFsParam[0].x;//�O�a

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

