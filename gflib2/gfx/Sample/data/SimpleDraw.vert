#version 430
//invariant gl_Position;
// -------------------------------------------------------------
// ���͐錾
// -------------------------------------------------------------
layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec4 inUv;
layout (location = 2) in vec4 inColor;

struct VsInput {
  vec4 inPosition;
	vec4 inUv;
	vec4 inColor;
};

// -------------------------------------------------------------
// �o�͐錾
// -------------------------------------------------------------
out vec4 outUv;
out vec4 outColor;

out gl_PerVertex {
  vec4 gl_Position;
};

// -------------------------------------------------------------
// �O���[�o���ϐ�
// -------------------------------------------------------------
layout(std140,column_major,binding=0) uniform ConstantBuffer0{
	mat4	cVsProjectionMatrix;
};

layout(std140,column_major,binding=1) uniform ConstantBuffer1{
	mat4	cVsModelViewMatrix;
};

layout(std140,column_major,binding=2) uniform ConstantBuffer2{
	mat4	cVsModelMatrix;
};

vec4 mul( vec4 vec, mat4 mat )
{
	vec4		outVec = vec4(0, 0, 0, 1);
	
	outVec.x = dot( vec, mat[0] );
	outVec.y = dot( vec, mat[1] );
	outVec.z = dot( vec, mat[2] );
	outVec.w = dot( vec, mat[3] );
	
	return outVec;
}

// ------------------------------------------------------------
// ���_�V�F�[�_�v���O����
// ------------------------------------------------------------
void main(void)
{
	VsInput					input;
	
	input.inPosition = inPosition;
	input.inUv = inUv;
	input.inColor = inColor;
	
	vec4		pos = vec4(0, 0, 0, 1);
	vec4		view = vec4(0, 0, 0, 1);

	pos = input.inPosition;
	pos = mul(pos, cVsModelMatrix);
	view = mul(pos, cVsModelViewMatrix);
	gl_Position = mul(view, cVsProjectionMatrix);

	outUv = input.inUv;
	outColor = input.inColor;
}
