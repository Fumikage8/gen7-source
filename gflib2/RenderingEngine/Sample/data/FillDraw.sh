// ���_�V�F�[�_�̓��̓f�[�^��`
struct VS_INPUT {
	uint    vIndex    : SV_VertexID;
};

// �s�N�Z�� �V�F�[�_�̓��̓f�[�^��`
struct PS_INPUT {
    float4 Pos  : SV_POSITION; // ���_���W(�������W�n)
    float4 Col  : COLOR;       // ���_�F
	float4 Uv   : TEXCOORD0;     
};

Texture2D		g_Texture : register( t0 );
SamplerState	g_Sampler : register( s0 );

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
	
	output.Uv.xy = texlist[ input.vIndex ].xy;

	return output;
}


// �s�N�Z�� �V�F�[�_�̊֐�
float4 PS(PS_INPUT input) : SV_TARGET
{
	return g_Texture.Sample( g_Sampler, input.Uv.xy );
}
