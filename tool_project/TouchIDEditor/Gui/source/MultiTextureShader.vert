// -------------------------------------------------------------
// 入力宣言
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
// 出力宣言
// -------------------------------------------------------------
varying vec4 outUv;

// ------------------------------------------------------------
// 頂点シェーダプログラム
// ------------------------------------------------------------
void main(void)
{
	gl_Position = inPosition;
	outUv = inUv0;
}
