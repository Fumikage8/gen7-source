//invariant gl_Position;
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
varying vec4 outColor;
varying vec3 outObjNormal;
varying vec3 outObjLight;
varying vec3 outWorldViewNormal;
varying vec3 outWorldViewLight;
varying vec3 outWorldViewHalfVec;
varying vec3 outBiNormalVec;
varying vec4 outDebugVec;

// -------------------------------------------------------------
// グローバル変数
// -------------------------------------------------------------
uniform vec4		cVsWorldLightVec;
uniform mat4x4	cVsModelMatrix;
uniform mat4x4	cVsModelViewMatrix;
uniform mat4		cVsProjectionMatrix;
uniform vec4		cVsMaterialColor;
uniform vec4		cVsUvOffSet;
uniform vec4		cVsRimParam;
uniform vec4		cVsPhongParam;
uniform vec4		cVsVertexFhongEnable;
uniform vec4		cVsCameraPosition;
uniform bool		cVsRimEnable = true;
uniform bool		cVsHalfVecLightEnable = true;
uniform bool		cVsVertexColorEnable = false;
uniform mat4x4	cVsJointMatrix[128];

#define		phongPow		cVsPhongParam.x
#define		phongScale	cVsPhongParam.y
#define		rimPow			cVsRimParam.x
#define		rimScale		cVsRimParam.y


// ------------------------------------------------------------
// 頂点シェーダプログラム
// ------------------------------------------------------------
void main(void)
{
	vec4		pos = vec4(0, 0, 0, 1);
	vec4		view = vec4(0, 0, 0, 1);
	vec4		objNormal = vec4(0, 0, 0, 0);
	vec4		objOrigNormal = vec4(0, 0, 0, 0);
	vec4		worldViewNormal = vec4(0, 0, 0, 0);
	vec4		objLight = vec4(0, 0, 0, 0);
	vec4		objView = vec4(0, 0, 0, 0);
	vec4		worldViewLight = vec4(0, 0, 0, 0);
	vec4		halfVec = vec4(0, 0, 0, 0);
	vec4		lightVec = cVsWorldLightVec;
	vec4		objColor = vec4(1, 1, 1, 1);
	mat4		modelInvMatrix = inverse( cVsModelMatrix );
	mat4		modelViewInvMatrix = inverse( gl_ModelViewMatrix );

	if ( cVsVertexColorEnable )
	{
		objColor = inColor;
	}
	
	pos = inPosition;
	pos = cVsModelMatrix * pos;
	view = gl_ModelViewMatrix * pos;
	objView = normalize(pos);
	pos = gl_ProjectionMatrix * view;

	objNormal.xyz = normalize(inNormal.xyz).xyz;
	objNormal.w = 0.0;
	objNormal = cVsModelMatrix * objNormal;
	objOrigNormal.xyz = normalize(inOrigNormal.xyz).xyz;
	
	if ( cVsHalfVecLightEnable )
	{
		vec4		objView = inPosition - cVsCameraPosition;
		objView.w = 0.0f;
		objView.xyz = normalize( objView.xyz );
		
		lightVec.xyz = -lightVec.xyz;
		lightVec.xyz += -objView.xyz;
		lightVec.xyz = -normalize( lightVec.xyz ).xyz;
		
		lightVec.xyz = objView.xyz;
	}

	objLight = lightVec;
	objLight.w = 0.0;//平行移動をしない
	objLight = normalize(modelInvMatrix * objLight);

	worldViewNormal = normalize(gl_ModelViewMatrix * objNormal);
	worldViewLight = normalize(gl_ModelViewMatrix * objLight);	

	view = normalize( -view );
	float	rim = dot( worldViewNormal, view );
	rim = max( rim, 0.0 );
	rim = 1.0 - rim;
	rim = pow( rim, rimPow ) * rimScale;

	halfVec = view - worldViewLight;
	halfVec = normalize( halfVec );

	{//フォン
		float		phong = dot(worldViewNormal, halfVec);
		phong = max( phong, 0.0 );
		phong = pow( phong, phongPow ) * phongScale;
		objColor *= 0.5;
		objColor.xyz += phong;
	}

	gl_Position = pos;
	outUv = inUv0 + cVsUvOffSet;
	outColor.xyz = min( objColor.xyz, 1.0 );

	if ( cVsRimEnable )
	{
		outColor.w = rim;
	}
	else
	{
		outColor.w = objColor.w;
	}

	outObjNormal.xyz = objNormal.xyz;
	
	outWorldViewNormal.xyz = worldViewNormal.xyz;
	outWorldViewLight = worldViewLight.xyz;
	outWorldViewHalfVec = halfVec.xyz;

	outDebugVec.xyz = objLight.xyz;
	outObjLight = objLight.xyz;
}
