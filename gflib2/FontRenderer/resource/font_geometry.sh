// �萔�o�b�t�@�̃f�[�^��`�@ �t�r���[�|�[�g�s��
cbuffer cbNeverChanges : register(b0) { // ��ɃX���b�g�u0�v���g��
    matrix InvViewPort;
};

// ���_�V�F�[�_�̓��̓f�[�^��`
struct VS_INPUT {
    float4 Pos : POSITION0;   // ���_���W(���f�����W�n)
	float4 Uv  : TEXCOORD0;
    float4 Col : COLOR0;      // ���_�F
};

// �W�I���g���V�F�[�_�̓��̓f�[�^��`
struct GS_INPUT {
    float4 Pos : POSITION0;   // ���_���W(���f�����W�n)
	float4 Uv  : TEXCOORD0;
    float4 Col : COLOR0;      // ���_�F
};

// �s�N�Z�� �V�F�[�_�̓��̓f�[�^��`
struct PS_INPUT {
    float4 Pos  : SV_POSITION; // ���_���W(�X�N���[�����W���W)
	float4 Uv   : TEXCOORD0;
    float4 Col  : COLOR;       // ���_�F
};

// �W�I���g���V�F�[�_�̓��́E�o�̓f�[�^��`

// FIXME: �Œ蒷�ɂ���̂͊댯
Texture2D		g_Texture[5] : register( t0 );
SamplerState	g_Sampler[5] : register( s0 );

// ���_�V�F�[�_�̊֐�
GS_INPUT VS(VS_INPUT input) {
    GS_INPUT output;

    // ���_���W�@���f�����W�n���r���[���W�n
    output.Pos = input.Pos;
	//output.Pos = mul(input.Pos, InvViewPort);
	        
    // ���_�F
    output.Col = input.Col;

	output.Uv = input.Uv;
    // �o��
    return output;
}

[maxvertexcount(4)]	// �W�I���g���V�F�[�_�ŏo�͂���ő咸�_��
void GS(point GS_INPUT input[1],
		  inout TriangleStream<PS_INPUT> stream
		  )
{
	PS_INPUT output;

	float4 position = {input[0].Pos.x, input[0].Pos.y, 0.0f, 1.0f};
	float2 diff = {input[0].Pos.z * InvViewPort[0][0], input[0].Pos.w * InvViewPort[1][1]};
	position = mul(position, InvViewPort);

	// 0�Ԗ�: ��ƂȂ钸�_
	output.Pos = position;
	output.Uv = input[0].Uv;
	output.Col = input[0].Col;
	stream.Append(output);

	// 1�Ԗ�: �E�ɐL�΂������_
	output.Pos.x += diff.x;
	output.Uv.x += output.Uv.z;
	stream.Append(output);

	// 2�Ԗ�: ���ɐL�΂������_
	output.Pos = position;
	output.Pos.y += diff.y;
	output.Uv = input[0].Uv;
	output.Uv.y += output.Uv.w;
	stream.Append(output);

	// 3�Ԗ�: �E���ɐL�΂������_
	output.Pos.x += diff.x;
	output.Uv.x += output.Uv.z;
	stream.Append(output);

	stream.RestartStrip();
}

// �s�N�Z�� �V�F�[�_�̊֐�
float4 PS(PS_INPUT input) : SV_TARGET
{
	float4 color;

	// UV���W��v�̗v�f��, �e�N�X�`����5����������,
	// 0.0f - 5.0f�͈̔͂Ŏ󂯎��̂�, ���̂悤�ɂ��Ē[����e��,
	// �e�N�X�`���̃C���f�b�N�X�ԍ���0.0f - 1.0f�͈͓���v���W���󂯎��
	int index = (int)floor(input.Uv.y);
	input.Uv.y -= (float)index;

	// color = index * g_Texture[index].Sample( g_Sampler[index], input.Uv.xy) * input.Col;
	// ���{���͂�����������������, ���I�z��A�N�Z�X���T�|�[�g���Ă��Ȃ��̂�,
	// if���̃I���p���[�h�ɂȂ��Ă���.
	// TODO : �{�����[���e�N�X�`���ɑΉ�������؂�ւ���
	if(index == 0)
	{
		color = g_Texture[0].Sample( g_Sampler[0], input.Uv.xy) * input.Col;
	}
	else if(index == 1)
	{
		color = g_Texture[1].Sample( g_Sampler[1], input.Uv.xy) * input.Col;
	}
	else if(index == 2)
	{
		color = g_Texture[2].Sample( g_Sampler[2], input.Uv.xy) * input.Col;
	}
	else if(index == 3)
	{
		color = g_Texture[3].Sample( g_Sampler[3], input.Uv.xy) * input.Col;
	}
	else if(index == 4)
	{
		color = g_Texture[4].Sample( g_Sampler[4], input.Uv.xy) * input.Col;
	}
	return color;
}
