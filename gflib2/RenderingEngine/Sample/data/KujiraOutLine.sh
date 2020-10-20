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
Texture2D		g_IDTexture : register( t1 );
SamplerState	g_IDSampler : register( s1 );

cbuffer PixcelShaderVectorConstants : register( b0 )
{
	Math::Vector cFsUvOffSet1;
	Math::Vector cFsUvOffSet2;
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
vec4 GetIDEdgeColor( vec4	idColor[3] )
{
	vec4			constantColor = vec4(0.1, 0.1, 0.1, 0.6);
	vec4			edgeColorRGB = vec4(0.0, 0.0, 0.0, 0.0);
	vec4			edgeColorA = vec4(0.0, 0.0, 0.0, 0.0);

	//�R���o�C�i�O
	edgeColorRGB = ( 1.0 - idColor[0].x ) - ( 1.0 - idColor[1].x );
	edgeColorA = ( 1.0 - idColor[0].x ) - ( 1.0 - idColor[2].x );

	edgeColorRGB *= 4.0f;
	edgeColorA *= 4.0f;

	edgeColorRGB = saturate( edgeColorRGB );
	edgeColorA = saturate( edgeColorA );

	//�R���o�C�i�P
	edgeColorRGB = edgeColorRGB + edgeColorA;
	edgeColorA = edgeColorA;

	edgeColorRGB *= 4.0f;

	edgeColorRGB = saturate( edgeColorRGB );
	edgeColorA = saturate( edgeColorA );
	//�R���o�C�i�Q
	edgeColorRGB = edgeColorRGB + edgeColorRGB;
	edgeColorA = edgeColorA;

	edgeColorRGB *= 4.0f;

	edgeColorRGB = saturate( edgeColorRGB );
	edgeColorA = saturate( edgeColorA );
	//�R���o�C�i�R
	edgeColorRGB = (edgeColorRGB + edgeColorRGB) * (1.0 - idColor[0].x);
	edgeColorA = edgeColorA;

	edgeColorRGB = saturate( edgeColorRGB );
	edgeColorA = saturate( edgeColorA );
	//�R���o�C�i�S
	edgeColorRGB = (1.0 - edgeColorRGB);
	edgeColorA = edgeColorA;

	edgeColorRGB = saturate( edgeColorRGB );
	edgeColorA = saturate( edgeColorA );

	edgeColorRGB.w = edgeColorA.x;

	return edgeColorRGB;
}

// ------------------------------------------------------------
// 
// ------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_TARGET
{
	vec4	edgeColor;

	vec4	normal[3];

	normal[0] = g_Texture.Sample( g_Sampler, input.Uv0.xy );
	normal[1] = g_Texture.Sample( g_Sampler, input.Uv0.xy + cFsUvOffSet1.xy );
	normal[2] = g_Texture.Sample( g_Sampler, input.Uv0.xy + cFsUvOffSet2.xy );

	for( int i = 0; i < 3; ++i )
	{
		normal[i].xyz -= 0.5;
		normal[i].xyz *= 2.0;
		normal[i].xyz = normalize( normal[i].xyz );
	}
	
	edgeColor = GetNormalEdgeColor( normal );
	
	vec4	idEdgeColor[2];
	vec4	idColor0[3];
	vec4	idColor1[3];
	
	idColor0[0] = g_IDTexture.Sample( g_IDSampler, input.Uv0.xy );
	idColor0[1] = g_IDTexture.Sample( g_IDSampler, input.Uv0.xy + cFsUvOffSet1.xy );
	idColor0[2] = g_IDTexture.Sample( g_IDSampler, input.Uv0.xy + cFsUvOffSet2.xy );
	idColor1[0] = g_IDTexture.Sample( g_IDSampler, input.Uv0.xy );
	idColor1[1] = g_IDTexture.Sample( g_IDSampler, input.Uv0.xy + cFsUvOffSet1.xy );
	idColor1[2] = g_IDTexture.Sample( g_IDSampler, input.Uv0.xy + cFsUvOffSet2.xy );
	
	idEdgeColor[0] = GetIDEdgeColor( idColor0 );
	idEdgeColor[1] = GetIDEdgeColor( idColor1 );
	idEdgeColor[0] = min( idEdgeColor[0], idEdgeColor[1] );
	
	edgeColor.xyz *= idEdgeColor[0].xyz;
	
	edgeColor = saturate( edgeColor );
	edgeColor.w = 1.0f - edgeColor.x;

	return edgeColor;
	//return g_IDTexture.Sample( g_IDSampler, input.Uv0.xy );
	//return g_Texture.Sample( g_Sampler, input.Uv0.xy );
}
