#version 150 compatibility
#extension GL_EXT_geometry_shader4 : enable

#define attribute   in
#define varying     out

// -------------------------------------------------------------
// 定数
// -------------------------------------------------------------

const int UvMax = 3;
const int TextureMax = 4;

const int UvMap = 0;
const int CubeMap = 1;
const int SphereMap = 2;
const int ProjMax = 3;

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
varying vec4 <OutName>Pos;
varying vec4 <OutName>Uv[TextureMax];
varying vec4 <OutName>Color;
varying vec3 <OutName>Normal;
varying vec3 <OutName>ObjNormal;
varying vec3 <OutName>ObjLight;
varying vec3 <OutName>ObjTangentVec[TextureMax];
varying vec4 <OutName>View;
varying vec4 <OutName>DebugVec;
varying vec4 <OutName>Raram;

// -------------------------------------------------------------
// グローバル変数
// -------------------------------------------------------------
uniform vec4		cVsWorldLightVec;
uniform vec4		cVsWorldLightColor;
uniform vec4		cVsAmbientLightColor;
uniform mat4x4	cVsModelMatrix;
uniform mat4x4	cVsModelViewMatrix;
uniform mat4		cVsProjectionMatrix;
uniform vec4		cVsMaterialColor;
uniform vec4		cVsVertexFhongEnable;
uniform vec4		cVsCameraPosition;
uniform vec4		cVsObjSpaceCameraPosition;
uniform float		cVsSceneContrast = 1.0;
uniform float		cVsZoneContrast = 1.0;
uniform bool		cVsHalfVecLightEnable = true;
uniform bool		cVsVertexColorEnable = false;
uniform bool		cVsSceneContrastEnable = false;
uniform vec4		cVsRimParam;
uniform vec4		cVsCommonParam;

uniform bool		cVsLightingEnable;
uniform bool		cIsPokemonShader;
uniform bool		cUseObjectSpaceNormalMap;

uniform int			cMappingType[TextureMax];
uniform int     cUvSetNo[TextureMax];
uniform mat4x4	cVsJointMatrix[128];

uniform int			cVsDrawUvSpaceNo;


#define		rimPow			cVsRimParam.x
#define		rimScale		cVsRimParam.y
#define		phongPow			cVsRimParam.z
#define		phongScale		cVsRimParam.w

#define		farLength			cVsCommonParam.x

// ------------------------------------------------------------
// 
// ------------------------------------------------------------
vec3 GetReflectVec( vec3 Pos, vec3 Normal )
{
	vec3				EyeVec;
	vec3				ReflectVec;
	
	EyeVec = normalize( cVsCameraPosition.xyz - Pos.xyz );
	ReflectVec = -EyeVec + 2.0f * dot( EyeVec, Normal ) * Normal;//反射ベクトルの計算

	return ReflectVec;
}

// ------------------------------------------------------------
// バインドポーズへの変換マトリックス取得（スキニングの逆行列）
// ------------------------------------------------------------
mat3x3 GetBindPoseMatrix( const in vec4 objNormal, const in vec4 objTangent, const in vec4 objOrigNormal, const in vec4 objOrigTangent )
{
	vec4		objBiNormal = vec4(0, 0, 0, 0);
	vec4		objOrigBiNormal = vec4(0, 0, 0, 0);
	mat3x3	tangentSpaceMatrix;
	mat3x3	tangentSpaceInvMatrix;
	mat3x3	origTangentSpaceMatrix;
	mat3x3	origTangentSpaceInvMatrix;
	mat3x3	toOrigTangentSpaceMatrix;
	
	objBiNormal.xyz = cross( objNormal.xyz, objTangent.xyz ).xyz;
	objOrigBiNormal.xyz = cross( objOrigNormal.xyz, objOrigTangent.xyz );

	tangentSpaceMatrix[0] = objTangent.xyz;
	tangentSpaceMatrix[1] = objBiNormal.xyz;
	tangentSpaceMatrix[2] = objNormal.xyz;

	origTangentSpaceMatrix[0] = objOrigTangent.xyz;
	origTangentSpaceMatrix[1] = objOrigBiNormal.xyz;
	origTangentSpaceMatrix[2] = objOrigNormal.xyz;

	tangentSpaceInvMatrix = inverse( tangentSpaceMatrix );
	origTangentSpaceInvMatrix = inverse( origTangentSpaceMatrix );

	tangentSpaceMatrix = tangentSpaceMatrix * origTangentSpaceInvMatrix;
	toOrigTangentSpaceMatrix = inverse( tangentSpaceMatrix );

	return toOrigTangentSpaceMatrix;
}

// ------------------------------------------------------------
// 頂点シェーダプログラム
// ------------------------------------------------------------
void main(void)
{
	vec4		pos = vec4(0, 0, 0, 1);
	vec4		world_pos = vec4(0, 0, 0, 1);
	vec4		view = vec4(0, 0, 1, 1);
	vec4		objNormal = vec4(0, 0, 0, 0);
	vec4		objTangent[TextureMax];
	vec4		worldViewNormal = vec4(0, 0, 0, 0);
	vec4		objLight = vec4(0, 0, 0, 0);
	vec4		objView = vec4(0, 0, 0, 0);
	vec4		worldViewLight = vec4(0, 0, 0, 0);
	vec4		halfVec = vec4(0, 0, 0, 0);
	vec4		reflectVec = vec4(0, 0, 0, 0);
	vec4		lightVec = cVsWorldLightVec;
	vec4		objSpaceLightVec;
	vec4		objColor = vec4(1, 1, 1, 1);
	mat4		modelInvMatrix = inverse( cVsModelMatrix );
	mat4		modelViewInvMatrix = inverse( cVsModelViewMatrix );
	float		depth;

	if ( cVsVertexColorEnable )
	{
		objColor = inColor;
	}
	
	world_pos = inPosition;
	<OutName>Pos = world_pos;
	world_pos = cVsModelMatrix * world_pos;
	view = gl_ModelViewMatrix * world_pos;
	
	objView = normalize(world_pos);
	pos = gl_ProjectionMatrix * view;
	depth = view.z;
	view = vec4(0, 0, -1, 0);

	objNormal.xyz = normalize(inNormal.xyz).xyz;
	objNormal.w = 0.0;
	<OutName>Normal.xyz = objNormal.xyz;
	objNormal = cVsModelMatrix * objNormal;
	objNormal.w = 0.0;

	{
		objTangent[0].xyz = normalize(inTangent0.xyz).xyz;
		objTangent[1].xyz = normalize(inTangent1.xyz).xyz;
		objTangent[2].xyz = normalize(inTangent2.xyz).xyz;

		for( int i = 0; i < TextureMax; i++ )
		{
			objTangent[i].w = 0.0;
			objTangent[i] = cVsModelMatrix * objTangent[i];
		}
	}
	
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

	if ( cUseObjectSpaceNormalMap )
	{
		mat3x3	toOrigTangentSpaceMatrix;
		toOrigTangentSpaceMatrix = GetBindPoseMatrix( inNormal, inTangent0, inOrigNormal, inOrigTangent );
		objSpaceLightVec.xyz = toOrigTangentSpaceMatrix * lightVec.xyz;
	}

	objLight = lightVec;
	objLight.w = 0.0;//平行移動をしない
	objLight = normalize(modelInvMatrix * objLight);

	worldViewNormal = normalize(cVsModelViewMatrix * objNormal);
	worldViewLight = normalize(cVsModelViewMatrix * objLight);	

	view = normalize( view );

	halfVec = -view - worldViewLight;
	halfVec.xyz = normalize( halfVec.xyz );

	gl_Position = pos;

	{
		reflectVec.xyz = mat3(modelViewInvMatrix)	* reflect( view.xyz, worldViewNormal.xyz );
	}

	for( int i = 0; i < UvMax; i++)
	{
		vec2		uv;
		vec2		uvValue;

		if ( cMappingType[i] == UvMap )
		{
      if ( cUvSetNo[i] == 0 )
			  uvValue.xy = inUv0.xy;
      else if ( cUvSetNo[i] == 1 )
			  uvValue.xy = inUv1.xy;
      else if ( cUvSetNo[i] == 2 )
			  uvValue.xy = inUv2.xy;
		}
		else if ( cMappingType[i] == CubeMap )
		{
			uvValue.xy = reflectVec.xy;
		}
		else if ( cMappingType[i] == SphereMap )
		{
			uv.xy = worldViewNormal.xy;
			uv *= 0.5;
			uv += 0.5;
			uvValue.xy = uv.xy;
		}
		else if ( cMappingType[i] == ProjMax )
		{
			pos.xyz /= pos.w;
			uv.xy = pos.xy;
			uv *= 0.5;
			uv += 0.5;
			uvValue.xy = uv.xy;
		}

		if ( i == 0 )
			<OutName>Uv[0].xy = uvValue;
		else if ( i == 1 )
			<OutName>Uv[1].xy = uvValue;
		else if ( i == 2 )
			<OutName>Uv[2].xy = uvValue;
	}

	<OutName>Uv[3].xy = vec2(0,2);

	{//<OutName>Color出力
		<OutName>Color = objColor;
	}

	{//<OutName>ObjNormal出力
		<OutName>ObjNormal.xyz = objNormal.xyz;
	}

	{//<OutName>ObjTangentVec出力
		for( int i = 0; i < TextureMax; i++ )
		{
			<OutName>ObjTangentVec[i].xyz = objTangent[i].xyz;
		}
	}

	{//<OutName>ObjLight出力
		<OutName>ObjLight = objLight.xyz;
	}

	{//<OutName>View出力
		<OutName>View.xyz = cVsObjSpaceCameraPosition.xyz - <OutName>Pos.xyz;
		<OutName>View.xyz = normalize( <OutName>View.xyz );
		<OutName>View.w = 0.0;

		if ( cUseObjectSpaceNormalMap )
		{
			<OutName>View.xyz = -objSpaceLightVec.xyz;//上書き

			<OutName>ObjNormal.xyz = vec3(0, 0, 1);

			for( int i = 0; i < TextureMax; i++ )
			{
				<OutName>ObjTangentVec[i].xyz = vec3(1, 0, 0);
			}
		}
	}

	if ( cVsLightingEnable )
	{
		float		lightPow = dot( objNormal, -objLight );
		vec4		lightCol;
		lightCol = cVsWorldLightColor * lightPow + cVsAmbientLightColor;

		<OutName>Color = objColor * lightCol;
		//クランプ
		<OutName>Color = max( <OutName>Color, 0.0 );
		<OutName>Color = min( <OutName>Color, 1.0 );
	}

	//==================================
	if ( cIsPokemonShader )
	{
		<OutName>Color.x = objColor.x;

		//逆光
		float		backlight = dot( objNormal, objLight );
		<OutName>Color.y = backlight;
		
		//フォン計算
		worldViewNormal.xyz = normalize( worldViewNormal.xyz );
		float		phong = dot(worldViewNormal.xyz, halfVec.xyz);
		phong = max( phong, 0.0 );
		phong = min( phong, 1.0 );
		phong = pow( phong, phongPow ) * phongScale;
		<OutName>Color.z = phong;

		//リム計算
		worldViewNormal.xyz = normalize( worldViewNormal.xyz );
		float	rim = dot( worldViewNormal.xyz, -view.xyz );
		rim = max( rim, 0.0 );
		rim = min( rim, 1.0 );
		rim = 1.0 - rim;
		rim = pow( rim, rimPow ) * rimScale;
		<OutName>Color.w = rim;

		//クランプ
		<OutName>Color = max( <OutName>Color, 0.0 );
		<OutName>Color = min( <OutName>Color, 1.0 );
	}

	if ( cVsSceneContrastEnable )
	{
		<OutName>Color.xyz = objColor.xyz;
		<OutName>Color.w = cVsSceneContrast * cVsZoneContrast;
	}

	<OutName>Uv[0].xy = inUv0.xy;
	<OutName>Raram.x = abs(depth) / farLength;
	//<OutName>Raram.x = inPosition.y / farLength;

	if ( cVsDrawUvSpaceNo >= 0 )
	{
		vec4			projUv = gl_Position;

		projUv.xyz /= projUv.w;
		projUv *= 0.5;
		projUv += 0.5;
		<OutName>Raram.yz = projUv.xy;

		vec4			uvSpace;
		uvSpace.xy = <OutName>Uv[ cVsDrawUvSpaceNo ].xy;
		gl_Position = uvSpace;
	}
}
