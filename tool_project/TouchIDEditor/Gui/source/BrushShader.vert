// -------------------------------------------------------------
// ���͐錾
// -------------------------------------------------------------
attribute vec4 inPosition;
attribute vec4 inColor;
attribute vec4 inUv0;
attribute vec4 inUv1;
attribute vec4 inUv2;
attribute vec4 inNormal;
attribute vec4 inTangent0;
attribute vec4 inTangent1;
attribute vec4 inTangent2;
attribute vec4 inJointIndex;
attribute vec4 inJointWeight;
attribute vec4 inOrigNormal;
attribute vec4 inOrigTangent;

// -------------------------------------------------------------
// �o�͐錾
// -------------------------------------------------------------
varying vec4 outUv;

// -------------------------------------------------------------
// �O���[�o���ϐ�
// -------------------------------------------------------------
uniform vec4 cVsScreanSize;
uniform vec4 cVsBrushSize;
uniform vec4 cVsBrushPos;

// ------------------------------------------------------------
// ���_�V�F�[�_�v���O����
// ------------------------------------------------------------
void main(void)
{
	vec4			pos  = inPosition;

	pos.x *= cVsBrushSize.x / cVsScreanSize.x;
	pos.y *= cVsBrushSize.y / cVsScreanSize.y;

	pos = pos + cVsBrushPos;
	pos.z = 0;
	pos.w = 1;
	gl_Position = pos;

	outUv = inUv0;
}
