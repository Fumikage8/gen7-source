// �萔�o�b�t�@�̃f�[�^��`�@�@�ύX���Ȃ��f�[�^
cbuffer cbNeverChanges : register(b0) { // ��ɃX���b�g�u0�v���g��
    matrix Projection;   // �����ϊ��s��
};

// �萔�o�b�t�@�̃f�[�^��`�A�@�ύX�p�x�̒Ⴂ�f�[�^
cbuffer cbChangesEveryFrame : register(b1) { // ��ɃX���b�g�u1�v���g��
    matrix View;   // �r���[�ϊ��s��
};

// �萔�o�b�t�@�̃f�[�^��`�B�@�ύX�p�x�̍����f�[�^
cbuffer cbChangesEveryObject : register(b2) { // ��ɃX���b�g�u2�v���g��
    matrix World;      // ���[���h�ϊ��s��
};

// ���_�V�F�[�_�̓��̓f�[�^��`
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

// �s�N�Z�� �V�F�[�_�̓��̓f�[�^��`
struct PS_INPUT {
    float4 Pos  : SV_POSITION; // ���_���W(�������W�n)
    float4 Norm : NORMAL;      // �@���x�N�g��(���[���h���W�n)
    float4 Col  : COLOR;       // ���_�F
		float4 Uv   : TEXCOORD0;
};

Texture2D		g_Texture : register( t0 );
SamplerState	g_Sampler : register( s0 );

// ���_�V�F�[�_�̊֐�
PS_INPUT VS(VS_INPUT input) 
{
	PS_INPUT output;

	// ���_���W�@���f�����W�n���r���[���W�n
	output.Pos = mul(input.Pos, World);
	output.Pos = mul(output.Pos, View);
	// ���_���W�@�r���[���W�n���ˉe���W�n
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
