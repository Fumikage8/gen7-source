// -------------------------------------------------------------
// 定数
// -------------------------------------------------------------

#define		UvMax					(3)
#define		TextureMax		(4)

#define		UvMap					(0)
#define		CubeMap				(1)
#define		SphereMap			(2)
#define		ProjMax				(3)

#define		rimPow			cVsRimParam.x
#define		rimScale		cVsRimParam.y
#define		phongPow		cVsRimParam.z
#define		phongScale	cVsRimParam.w

#if defined HLSL

	#define				vec2				float2
	#define				vec3				float3
	#define				vec4				float4
	#define				mat4x4			float4x4
	#define				mat3x3			float3x3
	#define				inverse			transpose

	#define ConstantBuffer( NAME, REGNAME, NO )		cbuffer NAME : register( REGNAME )
	
	struct VsInput
	{
		vec4 Position0		: POSITION0;
		vec4 Normal				: NORMAL0;
		vec4 Tangent0			: TANGENT0;
		vec4 Uv0					: TEXCOORD0;
		vec4 Uv1					: TEXCOORD1;
		vec4 Uv2					: TEXCOORD2;	
		vec4 Color				: COLOR0;
		vec4 JointIndex		: BLENDINDICES0;
		vec4 JointWeight	: BLENDWEIGHT0;
		
		vec4 Tangent1			: TANGENT1;
		vec4 Tangent2			: TANGENT2;
		vec4 OrigNormal		: NORMAL1;
		vec4 OrigTangent	: TANGENT3;
		vec4 Position1		: POSITION1;
		vec4 Position2		: POSITION2;
		vec4 Position3		: POSITION3;
	};

	struct PsInput
	{
		vec4 Position											: SV_POSITION;
		vec4 Uv[TextureMax]								: TEXCOORD0;
		vec4 Color												: COLOR0;
		vec3 Normal												: NORMAL0;
		vec3 ObjNormal										: NORMAL1;
		vec3 ObjLight											: NORAML2;
		vec3 ObjTangentVec								: TANGENT0;
		vec4 View													: NORMAL3;
		vec4 Velocity0										: COLOR1;
		vec4 Velocity1										: COLOR2;
		vec4 Velocity2										: COLOR3;
		vec4 DebugVec											: POSITION0;
	};

#else

	#version 430

	#define				float2			vec2
	#define				float3			vec3
	#define				float4			vec4
	#define				float4x4		mat4x4
	#define				float3x3		mat3x3
	#define				transpose		inverse

	#define ConstantBuffer( NAME, REGNAME, NO )		layout(std140,column_major,binding=NO) uniform NAME

	layout (location = 0) in vec4 inPosition0;
	layout (location = 1) in vec4 inNormal;
	layout (location = 2) in vec4 inTangent0;
	layout (location = 3) in vec4 inUv0;
	layout (location = 4) in vec4 inUv1;
	layout (location = 5) in vec4 inUv2;
	layout (location = 6) in vec4 inColor;
	layout (location = 7) in vec4 inJointIndex;
	layout (location = 8) in vec4 inJointWeight;
	layout (location = 9) in vec4 inTangent1;
	layout (location = 10) in vec4 inTangent2;
	layout (location = 11) in vec4 inOrigNormal;
	layout (location = 12) in vec4 inOrigTangent;
	layout (location = 13) in vec4 inPosition1;
	layout (location = 14) in vec4 inPosition2;
	layout (location = 15) in vec4 inPosition3;

	out vec4 outPos;
	out vec4 outUv[TextureMax];
	out vec4 outColor;
	out vec3 outNormal;
	out vec3 outObjNormal;
	out vec3 outObjLight;
	out vec3 outObjTangentVec;
	out vec4 outView;
	out vec4 outVelocity0;
	out vec4 outVelocity1;
	out vec4 outVelocity2;
	out vec4 outDebugVec;

	struct VsInput
	{
		vec4 Position0;
		vec4 Normal;
		vec4 Tangent0;
		vec4 Uv0;
		vec4 Uv1;
		vec4 Uv2;
		vec4 Color;
		vec4 JointIndex;
		vec4 JointWeight;
		vec4 Tangent1;
		vec4 Tangent2;
		vec4 OrigNormal;
		vec4 OrigTangent;
		vec4 Position1;
		vec4 Position2;
		vec4 Position3;
	};

	struct PsInput
	{
		vec4 Position;
		vec4 Uv[TextureMax];
		vec4 Color;
		vec3 Normal;
		vec3 ObjNormal;
		vec3 ObjLight;
		vec3 ObjTangentVec;
		vec4 View;
		vec4 Velocity0;
		vec4 Velocity1;
		vec4 Velocity2;
		vec4 DebugVec;
	};
	
	//-----------------------------------------------------------------------------
	/// @brief	
	//-----------------------------------------------------------------------------
	mat3x3 mul( mat3x3 matA, mat3x3 matB )
	{
		return matA * matB;
	}

	//-----------------------------------------------------------------------------
	/// @brief	
	//-----------------------------------------------------------------------------
	mat4x4 mul( mat4x4 matA, mat4x4 matB )
	{
		return matA * matB;
	}

	//-----------------------------------------------------------------------------
	/// @brief	
	//-----------------------------------------------------------------------------
	vec3 mul( vec3 vec, mat3x3 mat )
	{
		vec3		outVec = vec3(0, 0, 0);
		
		outVec.x = dot( vec, mat[0] );
		outVec.y = dot( vec, mat[1] );
		outVec.z = dot( vec, mat[2] );
		
		return outVec;
	}

	//-----------------------------------------------------------------------------
	/// @brief	
	//-----------------------------------------------------------------------------
	vec4 mul( vec4 vec, mat4 mat )
	{
		vec4		outVec = vec4(0, 0, 0, 1);
		
		outVec.x = dot( vec, mat[0] );
		outVec.y = dot( vec, mat[1] );
		outVec.z = dot( vec, mat[2] );
		outVec.w = dot( vec, mat[3] );
		
		return outVec;
	}

#endif

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■				頂点シェーダー
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
ConstantBuffer( VertexShaderVectorConstants, b0, 0 )
{
	mat4x4	cVsModelMatrix;
	mat4x4	cVsViewMatrix;
	mat4x4	cVsModelViewMatrix;
	mat4x4	cVsProjectionMatrix;

	mat4x4	cVsNoramlRotate;
	vec4		cVsWorldLightVec;
	vec4		cVsWorldLightColor;
	vec4		cVsAmbientLightColor;
	//vec4		cVsMaterialColor;
	//vec4		cVsVertexFhongEnable;
	vec4		cVsCameraPosition;
	vec4		cVsObjSpaceCameraPosition;
	vec4		cVsRimParam;
	vec4		cVsShaderParam;
	vec4		cVsBlendMeshWeight;
};

ConstantBuffer( VertexShaderJotinConstants, b1, 1 )
{
	mat4x4	cVsJointMatrix[255];
	bool		cVsSkiningEnable;
	bool		cVsPadding[3];
};

ConstantBuffer( VertexShaderFloatConstants, b2, 2 )
{
	float		cVsSceneContrast;
	float		cVsZoneContrast;
};

ConstantBuffer( VertexShaderBoolConstants, b3, 3 )
{
	bool		cVsHalfVecLightEnable;
	bool		cVsVertexColorEnable;
	bool		cVsSceneContrastEnable;
	bool		cVsLightingEnable;
	bool		cIsPokemonShader;
	bool		cIsMegaGangarShader;
	bool		cUseObjectSpaceNormalMap;
	bool		cRotateSphereMapEnable;
};

ConstantBuffer( VertexShaderIntArrayConstants, b4, 4 )
{
	int			cMappingType[TextureMax];
	int     cUvSetNo[TextureMax];
};

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
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

	tangentSpaceMatrix = mul( origTangentSpaceInvMatrix, tangentSpaceMatrix );
	toOrigTangentSpaceMatrix = inverse( tangentSpaceMatrix );

	return toOrigTangentSpaceMatrix;
}

VsInput VsJointAnimation( VsInput In )
{
	float4x4		mat;
	
	mat = cVsJointMatrix[int(In.JointIndex.x)] * In.JointWeight.x;

	if ( In.JointIndex.y != -1.0f )
	{
		mat += cVsJointMatrix[int(In.JointIndex.y)] * In.JointWeight.y;

		if ( In.JointIndex.z != -1.0f )
		{
			mat += cVsJointMatrix[int(In.JointIndex.z)] * In.JointWeight.z;

			if ( In.JointIndex.w != -1.0f )
				mat += cVsJointMatrix[int(In.JointIndex.w)] * In.JointWeight.w;
		}
	}
	
	In.Position0 = mul( In.Position0, mat );
	In.Position0.w = 1.0f;

	float3x3		mat33;
	
	for( int i = 0; i < 3; i++ )
		mat33[i] = mat[i].xyz;
		
	In.Normal.xyz = mul( In.Normal.xyz, mat33 );
	In.Normal.xyz = normalize( In.Normal.xyz );
	In.Normal.w = 1.0f;
	In.Tangent0.xyz = mul( In.Tangent0.xyz, mat33 );
	In.Tangent0.xyz = normalize( In.Tangent0.xyz );
	In.Tangent0.w = 1.0f;
	
	return In;
}

// ------------------------------------------------------------
// 頂点シェーダプログラム
// ------------------------------------------------------------
#if defined HLSL
PsInput VsMain( VsInput input )
{
	PsInput			outPut = (PsInput)0;
	
#else

void main( void )
{
	PsInput			outPut;
	VsInput			input;
	
	input.Position0 = inPosition0;
	input.Normal = inNormal;
	input.Tangent0 = inTangent0;
	input.Uv0 = inUv0;
	input.Uv1 = inUv1;
	input.Uv2 = inUv2;
	input.Color = inColor;
	input.JointIndex = inJointIndex;
	input.JointWeight = inJointWeight;
	input.Tangent1 = inTangent1;
	input.Tangent2 = inTangent2;
	input.OrigNormal = inOrigNormal;
	input.OrigTangent = inOrigTangent;
	input.Position1 = inPosition1;
	input.Position2 = inPosition2;
	input.Position3 = inPosition3;

#endif

	VsInput			origInput = input;

	outPut.DebugVec = vec4( 1.0f, 0.0f, 0.0f, 1.0f );

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
	mat4x4	modelInvMatrix = inverse( cVsModelMatrix );
	mat4x4	modelViewInvMatrix = inverse( cVsModelViewMatrix );
	
	//IsBattleChar2.0 input.Position0 = lerp( input.Position0, input.Position1, cVsBlendMeshWeight.y );
	//IsBattleChar2.0 input.Position0 = lerp( input.Position0, input.Position2, cVsBlendMeshWeight.z );
	
	if ( cVsSkiningEnable )
	{
		input = VsJointAnimation( input );
	}

	if ( cVsVertexColorEnable )
	{
		objColor = input.Color;
	}

	{
		vec4		velocity3;

		world_pos = mul( input.Position0, cVsModelMatrix );
		outPut.Velocity0 = world_pos;

		outPut.Velocity1 = mul( input.Position1, cVsModelMatrix );
		outPut.Velocity2 = mul( input.Position2, cVsModelMatrix );
		velocity3 = mul( input.Position3, cVsModelMatrix );

		outPut.Velocity0.w = velocity3.x;
		outPut.Velocity1.w = velocity3.y;
		outPut.Velocity2.w = velocity3.z;
	}

	//-----------------

	view = world_pos;
	view = mul( view, cVsViewMatrix );

	//view = mul( world_pos, cVsModelViewMatrix );
	objView = normalize(world_pos);
	pos = mul( view, cVsProjectionMatrix );
	outPut.Position = pos;
	view = vec4(0, 0, -1, 0);

	objNormal.xyz = normalize(input.Normal.xyz).xyz;
	objNormal.w = 0.0;
	outPut.Normal.xyz = objNormal.xyz;
	objNormal = mul( objNormal, cVsModelMatrix );
	objNormal.w = 0.0;

	{
		objTangent[0].xyz = normalize(input.Tangent0.xyz).xyz;
		objTangent[1].xyz = normalize(input.Tangent1.xyz).xyz;
		objTangent[2].xyz = normalize(input.Tangent2.xyz).xyz;

		for( int i = 0; i < UvMax; i++ )
		{
			objTangent[i].w = 0.0;
			objTangent[i] = mul( objTangent[i], cVsModelMatrix );
		}
	}
	
	if ( cVsHalfVecLightEnable )
	{
		vec4		_objView = input.Position0 - cVsCameraPosition;
		_objView.w = 0.0f;
		_objView.xyz = normalize( _objView.xyz );
		
		//lightVec.xyz = -lightVec.xyz;
		//lightVec.xyz += -_objView.xyz;
		//lightVec.xyz = -normalize( lightVec.xyz ).xyz;
		
		lightVec.xyz = _objView.xyz;
	}

	if ( cUseObjectSpaceNormalMap )
	{
		mat3x3	toOrigTangentSpaceMatrix;
		toOrigTangentSpaceMatrix = GetBindPoseMatrix( input.Normal, input.Tangent0, origInput.Normal, origInput.Tangent0 );
		objSpaceLightVec.xyz = mul( lightVec.xyz, toOrigTangentSpaceMatrix );
	}

	objLight = lightVec;
	objLight.w = 0.0;//平行移動をしない
	objLight = normalize( mul( objLight, modelInvMatrix) );

	worldViewNormal = normalize( mul(objNormal, cVsViewMatrix));
	worldViewLight = normalize( mul(objLight, cVsViewMatrix) );	

	view = normalize( view );

	halfVec = -view - worldViewLight;
	halfVec.xyz = normalize( halfVec.xyz );

	for( int i = 0; i < UvMax; i++)
	{
		vec2		uv;
		vec4		uvValue = vec4(0, 0, 0, 0);

		if ( cMappingType[i] == UvMap )
		{
      if ( cUvSetNo[i] == 0 )
			  uvValue.xy = input.Uv0.xy;
      else if ( cUvSetNo[i] == 1 )
			  uvValue.xy = input.Uv1.xy;
      else if ( cUvSetNo[i] == 2 )
			  uvValue.xy = input.Uv2.xy;
		}
		else if ( cMappingType[i] == CubeMap )
		{
			//uvValue = reflectVec;
			uvValue = worldViewNormal;
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
			outPut.Uv[0] = uvValue;
		else if ( i == 1 )
			outPut.Uv[1] = uvValue;
		else if ( i == 2 )
			outPut.Uv[2] = uvValue;
	}

	outPut.Uv[3].xy = vec2(0,2);

	{//outColor出力
		outPut.Color = objColor;
	}

	{//outObjNormal出力
		outPut.ObjNormal.xyz = objNormal.xyz;
	}

	{//outObjTangentVec出力
		outPut.ObjTangentVec.xyz = objTangent[0].xyz;
	}

	{//outObjLight出力
		outPut.ObjLight = objLight.xyz;
	}

	{//outView出力
		outPut.View.xyz = cVsObjSpaceCameraPosition.xyz - world_pos.xyz;
		outPut.View.xyz = normalize( outPut.View.xyz );
		outPut.View.w = 0.0;

		if ( cUseObjectSpaceNormalMap )
		{
			outPut.View.xyz = -objSpaceLightVec.xyz;//上書き

			outPut.ObjNormal.xyz = vec3(0, 0, 1);
			outPut.ObjTangentVec.xyz = vec3(1, 0, 0);
		}
	}

	if ( cVsLightingEnable )
	{
		float		lightPow = dot( objNormal, -objLight );
		vec4		lightCol;

		lightPow = max( lightPow, 0.0 );
		lightPow = min( lightPow, 1.0 );

		lightCol = cVsWorldLightColor * lightPow + cVsAmbientLightColor;

		outPut.Color.xyz = objColor.xyz * lightCol.xyz;
		outPut.Color.w = objColor.w;
		//クランプ
		outPut.Color = max( outPut.Color, 0.0 );
		outPut.Color = min( outPut.Color, 1.0 );
	}

	//==================================
	if ( cIsPokemonShader )
	{
		if ( cIsMegaGangarShader )
		{
			outPut.Color.x = world_pos.y / cVsShaderParam.w;
		}

		//逆光
		float		backlight = dot( objNormal, objLight );
		float		scale = dot( view, worldViewLight ) * -1.0;//ライトと視野ベクトルが平行のときは０、直行するときは1.0

		backlight = max( backlight, 0.0 );

		scale = min( scale, 1.0 );
		scale = max( scale, 0.0 );
		scale = (scale * 0.5) + 0.5;//ライトと視野ベクトルが平行のときは0.5、直行するときは1.0に調整

		outPut.Color.y = backlight * scale;
		
		//フォン計算
		worldViewNormal.xyz = normalize( worldViewNormal.xyz );
		float		phong = dot(worldViewNormal.xyz, halfVec.xyz);
		phong = max( phong, 0.0 );
		phong = min( phong, 1.0 );
		phong = pow( phong, phongPow ) * phongScale;
		outPut.Color.z = phong;

		//リム計算
		worldViewNormal.xyz = normalize( worldViewNormal.xyz );
		float	rim = dot( worldViewNormal.xyz, -view.xyz );
		rim = max( rim, 0.0 );
		rim = min( rim, 1.0 );
		rim = 1.0 - rim;
		rim = pow( rim, rimPow ) * rimScale;
		outPut.Color.w = rim;

		//クランプ
		outPut.Color = max( outPut.Color, 0.0 );
		outPut.Color = min( outPut.Color, 1.0 );
	}

	if ( cVsSceneContrastEnable )
	{
		outPut.Color.xyz = objColor.xyz;
		outPut.Color.w = cVsSceneContrast * cVsZoneContrast;
	}

	//outPut.DebugVec.xyz = objSpaceLightVec.xyz;
	//outPut.DebugVec += 1.0f;
	//outPut.DebugVec *= 0.5f;

	/*outPut.DebugVec.xyz = worldViewNormal.xyz;
	outPut.DebugVec.w = worldViewNormal.w;
	outPut.DebugVec += 1.0;
	outPut.DebugVec *= 0.5;*/
	
	outPut.DebugVec = world_pos;

#if defined HLSL
	return outPut;
}

#else

	outPos = outPut.Position;
	for( int i = 0; i < UvMax; i++)
		outUv[i] = outPut.Uv[i];
	outColor = outPut.Color;
	outNormal = outPut.Normal;
	outObjNormal = outPut.ObjNormal;
	outObjLight = outPut.ObjLight;
	outObjTangentVec = outPut.ObjTangentVec;
	outView = outPut.View;
	outVelocity0 = outPut.Velocity0;
	outVelocity1 = outPut.Velocity1;
	outVelocity2 = outPut.Velocity2;
	outDebugVec = outPut.DebugVec;
	
	gl_Position = outPos;
}

#endif
