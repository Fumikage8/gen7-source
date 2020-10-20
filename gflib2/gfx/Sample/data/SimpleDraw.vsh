cbuffer cbNeverChanges : register(b0) {
    matrix Projection;
};

cbuffer cbNeverChanges : register(b1) {
    matrix View;
};

cbuffer cbNeverChanges : register(b2) {
    matrix World;
};

// ���_�V�F�[�_�̓��̓f�[�^��`
struct VS_INPUT {
    float4 Pos : POSITION0;   // ���_���W(���f�����W�n)
		float4 Uv  : TEXCOORD0;     
    float4 Col : COLOR0;      // ���_�F
};

// �s�N�Z�� �V�F�[�_�̓��̓f�[�^��`
struct PS_INPUT {
    float4 Pos  : SV_POSITION; // ���_���W(�������W�n)
    float3 Norm : NORMAL;      // �@���x�N�g��(���[���h���W�n)
    float4 Col  : COLOR;       // ���_�F
	float4 Uv   : TEXCOORD0;     
};

Texture2D		g_Texture : register( t0 );
SamplerState	g_Sampler : register( s0 );

// ���_�V�F�[�_�̊֐�
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;

	output.Pos = mul(input.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	// ���_�F
	output.Col = input.Col;

	output.Uv = input.Uv;
	// �o��
	return output;
}


// �s�N�Z�� �V�F�[�_�̊֐�
float4 PS(PS_INPUT input) : SV_TARGET
{
	return g_Texture.Sample( g_Sampler, input.Uv.xy );
}
