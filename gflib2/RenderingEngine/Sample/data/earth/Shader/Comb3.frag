#version 430
#extension GL_NV_shadow_samplers_cube : enable 

// -------------------------------------------------------------
// 定数
// -------------------------------------------------------------

#define		UvMax					(3)
#define		TextureMax		(4)
#define		LutMax				(6)

#define		UvMap					(0)
#define		CubeMap				(1)
#define		SphereMap			(2)
#define		ProjMax				(3)

#define		rimPow			cVsRimParam.x
#define		rimScale		cVsRimParam.y
#define		phongPow			cVsRimParam.z
#define		phongScale		cVsRimParam.w

#define		bPad					bool
#define		iPad					int
#define		fPad					float

#if defined HLSL

	#define				STATIC				 static
	
	#define ConstantBuffer( NAME, REGNAME, NO )		cbuffer NAME : register( REGNAME )

	#define				vec2					float2
	#define				vec3					float3
	#define				vec4					float4
	#define				mat4x4				float4x4
	#define				mat3x3				float3x3
	#define				inverse				transpose
	#define				Math::Vector	vec4
	
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
	
	#define				texture2D			tex2D
	#define				textureCube		texCUBE
	
	// -------------------------------------------------------------
	// 頂点プログラムからフラグメントプログラムに渡すデータ
	// -------------------------------------------------------------
	static vec4 outPos;
	static vec4 outUv[TextureMax];
	static vec4 outColor;
	static vec3 outObjNormal;
	static vec3 outObjLight;
	static vec3 outObjTangentVec;
	static vec4 outView;
	static vec4 outDebugVec;
	
	Texture2D TextureImage[TextureMax] : register( t0 );
	SamplerState Texture[TextureMax] : register( s0 );

	TextureCube CubeTextureImage : register( t4 );
	SamplerState CubeTexture : register( s4 );

	Texture2D NoizeTextureImage : register( t5 );
	SamplerState NoizeTexture : register( s5 );

	Texture2D MaterialNoizeTextureImage : register( t6 );
	SamplerState MaterialNoizeTexture : register( s6 );

	Texture2D BrushStrokeTextureImage : register( t7 );
	SamplerState BrushStrokeTexture : register( s7 );

	Texture2D HighLightTextureImage : register( t8 );
	SamplerState HighLightTexture : register( s8 );

	Texture2D LutTextureImage[LutMax] : register( t9 );
	SamplerState LutTexture[LutMax] : register( s9 );

	Texture2D OutLineColorTextureImage : register( t15 );
	SamplerState OutLineColorTexture : register( s15 );

#else

	#define				STATIC				 
	
	#define ConstantBuffer( NAME, REGNAME, NO )		layout(std140,column_major,binding=NO) uniform NAME

	#define				float2				vec2
	#define				float3				vec3
	#define				float4				vec4
	#define				float4x4			mat4x4
	#define				float3x3			mat3x3
	#define				transpose			inverse
	#define				Math::Vector	vec4

	in vec4 outPos;
	in vec4 outUv[TextureMax];
	in vec4 outColor;
	in vec3 outNormal;
	in vec3 outObjNormal;
	in vec3 outObjLight;
	in vec3 outObjTangentVec;
	in vec4 outView;
	in vec4 outVelocity0;
	in vec4 outVelocity1;
	in vec4 outVelocity2;
	in vec4 outDebugVec;

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
	
	#define				tex2D						texture2D
	#define				texCUBE					textureCube
	
	uniform sampler2D Texture[TextureMax];//テクスチャユニット
	uniform samplerCube CubeTexture[TextureMax];//テクスチャユニット
	uniform sampler2D LutTexture[LutMax];
	uniform sampler2D HighLightTexture;//テクスチャユニット
	
	#define		lerp			mix

#endif

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■				ピクセルシェーダー
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
// -------------------------------------------------------------
// 定数
// -------------------------------------------------------------
STATIC const int InputSourceTypeTexture0 = 0;
STATIC const int InputSourceTypeTexture1 = 1;
STATIC const int InputSourceTypeTexture2 = 2;
STATIC const int InputSourceTypeTexture3 = 3;
STATIC const int InputSourceTypeConstnat = 4;
STATIC const int InputSourceTypeVSColor = 5;
STATIC const int InputSourceTypePrimary = 6;
STATIC const int InputSourceTypeSecondary = 7;
STATIC const int InputSourceTypeBeforeAnswer = 8;
STATIC const int InputSourceTypeBeforeBuffer = 9;

STATIC const int SourceTypeA = 0;
STATIC const int SourceTypeB = 1;
STATIC const int SourceTypeC = 2;
STATIC const int SourceTypeMax = 3;

STATIC const int OperandTypeRGB = 0;
STATIC const int OperandType1_RGB = 1;
STATIC const int OperandTypeA = 2;
STATIC const int OperandType1_A = 3;
STATIC const int OperandTypeR = 4;
STATIC const int OperandType1_R = 5;
STATIC const int OperandTypeG = 6;
STATIC const int OperandType1_G = 7;
STATIC const int OperandTypeB = 8;
STATIC const int OperandType1_B = 9;
STATIC const int OperandTypeMax = 10;

STATIC const int ColorTypeRGB = 0;
STATIC const int ColorTypeA = 1;
STATIC const int ColorTypeMax = 2;

STATIC const int MathFormulaA = 0;
STATIC const int MathFormulaAxB = 1;
STATIC const int MathFormulaAaddB = 2;
STATIC const int MathFormulaAaddBsub05 = 3;
STATIC const int MathFormulaAxCaddBx1subC = 4;
STATIC const int MathFormulaAsubB = 5;
STATIC const int MathFormulaRGBDot = 6;
STATIC const int MathFormulaRGBADot = 7;
STATIC const int MathFormulaAaddBxC = 8;
STATIC const int MathFormulaAxBaddC = 9;

STATIC const int LutDistribution0 = 0;
STATIC const int LutDistribution1 = 1;
STATIC const int LutReflectionR = 2;
STATIC const int LutReflectionG = 3;
STATIC const int LutReflectionB = 4;
STATIC const int LutFresnel = 5;

STATIC const int BumpMap = 0;
STATIC const int TangentMap = 1;

STATIC const int NH = 0;//法線とハーフベクトル （デフォルト）
STATIC const int VH = 1;//ビューベクトルとハーフベクトル
STATIC const int NV = 2;//法線とビューベクトル
STATIC const int LN = 3;//ライトベクトルと法線
STATIC const int SP = 4;//ライトベクトルの逆ベクトルとスポットライトの方向ベクトル（RR, RG, RB, FR
STATIC const int CP = 5;//ハーフベクトルの接平面への投影と接線（RR, RG, RB, FR は不可）

STATIC const int LightMax = 8;

// -------------------------------------------------------------
// グローバル変数
// -------------------------------------------------------------

STATIC const int CombinerStageMax = 6;

ConstantBuffer( PixcelShaderVectorConstants, b8, 8 )
{
	vec4 cPsObjSpaceCameraPosition;

	//-----カラー-------
	vec4 cEmission;
	vec4 cAmbient;
	vec4 cDiffuse;
	vec4 cSpecular0;
	vec4 cSpecular1;
	vec4 cConsntantColor[CombinerStageMax];
	vec4 cBufferColor;

	//-------- ライト情報 --------------
	vec4 cPsObjSpaceLightPos[LightMax];
	vec4 cPsObjSpaceLightColor[LightMax];
	vec4 cPsObjSpaceLightVec[LightMax];
	vec4 cPsAmbientLightColor;

	//-------- 参照テーブル情報 --------------
	vec4 cPsCookTorranceParam;

	//-----テクスチャ-------
	vec4 cGaussianOffset[16];

	//-----フラグメント-------

	//-----特殊フラグ-------

	vec4 cFsParam[2];
	vec4 cFsScreanSize;
};

ConstantBuffer( PixcelShaderFloatArrayConstants, b9, 9 )
{
	float cPsObjSpaceLightConeAngle[LightMax];

	//-------- 参照テーブル情報 --------------
	float cLutScale[LutMax];

	//-----テクスチャ-------
	float cScaleU[TextureMax];
	float cScaleV[TextureMax];
	float cRotate[TextureMax];
	float cTranslateU[TextureMax];
	float cTranslateV[TextureMax];

	//-----フラグメント-------
	float cScale[ColorTypeMax][CombinerStageMax];

	vec4 cTestColor1;
};

ConstantBuffer( PixcelShaderBoolConstants, b10, 10 )
{
	//-------- 参照テーブル情報 --------------
	bool cCookTorrance0Enable;
	bool cCookTorrance1Enable;
	bool cSpecularColorClamp;
	bool cPrimaryFresnelEnable;
	bool cSecondaryFresnelEnable;

	//-----テクスチャ-------
	bool cZRecastEnable;

	//-----特殊フラグ-------
	bool cUseShadowTexture;
	bool cHigLightTextureEnable;
	bool cFsIsPokemonShader;
	bool cBattleChara20Enable;

	bool cFsRenderNormalMap;
	bool cFsDisableNormalColor;
	bool cFsBorderLineMapSampling;
};

ConstantBuffer( PixcelShaderBoolArrayConstants, b11, 11 )
{
	//-------- ライト情報 --------------
	bool cPsIsSpotLight[LightMax];

	//-------- 参照テーブル情報 --------------
	bool cLookUpTableEnable[LutMax];

	//-----テクスチャ-------
	bool cTextureEnable[TextureMax];
	bool cTexture2D[TextureMax];

	//-----フラグメント-------
	bool cOutPutBuffer[ColorTypeMax][CombinerStageMax];

	vec4 cTestColor3;
};

ConstantBuffer( PixcelShaderIntConstants, b12, 12 )
{
	//-------- ライト情報 --------------
	int cPsObjSpaceLightCount;

	//-----テクスチャ-------
	int cBumpMapNo;
	int cTangentNo;
	int cBumpMapType;

	vec4 cTestColor4;
};

ConstantBuffer( PixcelShaderIntArrayConstants, b13, 13 )
{
	//-------- 参照テーブル情報 --------------
	int cLutInputAngle[LutMax];

	//-----フラグメント-------
	int cMathFormula[ColorTypeMax][CombinerStageMax];
	int cInputSource[ColorTypeMax][CombinerStageMax][SourceTypeMax];
	int cSourceOperand[ColorTypeMax][CombinerStageMax][SourceTypeMax];
	int cTextureCombinerConstant[CombinerStageMax];

	vec4 cTestColor5;
};

//---------- コンバイナで扱える変数 ---------------------

STATIC vec4					g_vTexture[TextureMax];
STATIC vec4					g_vNormalTexture;
STATIC vec4					g_vHighLightTexture;
STATIC vec4					g_vConstnat[CombinerStageMax];
STATIC vec4					g_vVSColor = vec4(0.0, 0.0, 1.0, 0.0);
STATIC vec4					g_vPrimary = vec4(1.0, 0.0, 0.0, 0.0);
STATIC vec4					g_vSecondary = vec4(0.0, 1.0, 0.0, 0.0);
STATIC vec4					g_vAnswer = vec4(0.0, 0.0, 0.0, 0.0);
STATIC vec4					g_vBuffer = vec4(1.0, 0.0, 0.0, 0.0);
STATIC vec4					gl_FragColor = vec4(1.0, 0.0, 0.0, 0.0);

//-------------------------------

STATIC vec4					finalColorRgb;

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
vec3 saturate( vec3 value )
{
	for( int i = 0; i < 3; i++)
	{
		value[i] = max( 0.0, value[i] );
		value[i] = min( 1.0, value[i] );
	}

	return value;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
float saturate( float value )
{
	value = max( 0.0, value );
	value = min( 1.0, value );

	return value;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
vec3 saturate( vec3 value, float minValue, float maxValue )
{
	for( int i = 0; i < 3; i++)
	{
		value[i] = max( minValue, value[i] );
		value[i] = min( maxValue, value[i] );
	}

	return value;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
float saturate( float value, float minValue, float maxValue )
{
	value = max( minValue, value );
	value = min( maxValue, value );

	return value;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
vec4 getTextureUv( int texNo )
{
	vec4			uv;
	vec2			rotuv;
	float 		rad = 3.1415926535898 / 180.0 * 90.0;

	if ( cTextureEnable[texNo] == false )
		return vec4( 0.0, 0.0, 0.0, 1.0 );//何も表示されなくなってしまうのでアルファに１を入れとく。

	uv = outUv[texNo];

	//Mayaはテクスチャが移動するので逆スクロール
	uv.x -= cTranslateU[texNo];
	uv.y -= cTranslateV[texNo];

	uv.x -= 0.5f;
	uv.y -= 0.5f;

	rotuv.x = uv.x * cos( cRotate[texNo] ) - uv.y * sin( cRotate[texNo] );
	rotuv.y = uv.x * sin( cRotate[texNo] ) + uv.y * cos( cRotate[texNo] );
	uv.xy = rotuv.xy;

	uv.x += 0.5f;
	uv.y += 0.5f;

	uv.x *= cScaleU[texNo];
	uv.y *= cScaleV[texNo];

	return uv;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
vec4 getTextureColor( int texNo )
{
	vec4			color;
	vec4			uv;

	uv = getTextureUv( texNo );

	if ( cTexture2D[texNo] )
	{
#if defined HLSL
		color = TextureImage[texNo].Sample( Texture[texNo], uv.xy );
#else
		color = texture2D( Texture[texNo], uv.xy );
#endif
	}
	else
	{
		vec3		nor = uv.xyz;
		nor.y = -nor.y;
		nor.z = -nor.z;
#if defined HLSL		
		color = CubeTextureImage.Sample( CubeTexture, nor );
#else		
		color = textureCube( CubeTexture[texNo], nor );
#endif		
	}

	return color;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
vec3 getNormal()
{
	vec3			tangent;
	vec3			binormal;
	vec3			normal;
	mat3x3		tangentSpaceMatrix;

	normal.xyz = normalize( outObjNormal.xyz );

	if ( cBumpMapNo >= 0 )
	{
		tangent.xyz = normalize( outObjTangentVec.xyz );
		binormal.xyz = cross( normal.xyz, tangent.xyz ).xyz;

		tangentSpaceMatrix[0] = tangent.xyz;
		tangentSpaceMatrix[1] = binormal.xyz;
		tangentSpaceMatrix[2] = normal.xyz;

    normal = g_vTexture[cBumpMapNo].xyz;
	  normal *= 2.0;
	  normal -= 1.0;
		normal.xyz = normalize( normal.xyz );

		if ( cHigLightTextureEnable )
		{
			normal.xyz *= g_vHighLightTexture.xyz;
		}
    
	  //normal = mul( normal, tangentSpaceMatrix );//オブジェクトスペース法線マップを使用
	}
	else if ( cFsIsPokemonShader || cBattleChara20Enable )
	{
		normal *= 0.5;
	}

	return normal;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
vec3 getSpotPow( int lightNo, vec3 lightVec, vec3 normal )
{
	vec3			Spot;
	vec4			spotVec;
	float			spotArea;

	spotVec.xyz = normalize( cPsObjSpaceLightVec[lightNo].xyz );
	spotVec.xyz = normalize( spotVec.xyz );
	Spot.x = dot( normal.xyz, spotVec.xyz );
	Spot.y = Spot.x;
	Spot.z = Spot.x;

	spotArea = dot( lightVec.xyz, spotVec.xyz );
	if ( spotArea < cPsObjSpaceLightConeAngle[lightNo] )
	{
		Spot.xyz = vec3(0,0,0);
	}

	return Spot;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
vec4 getPrimaryColor()
{
	vec4			outCol = vec4( 0.0, 0.0, 0.0, 1.0 );
	vec3			normal;
	
	normal.xyz = getNormal();

	for( int i = 0; i < cPsObjSpaceLightCount; i ++ )
	{
		vec4			lightVec;
		vec3			Diffuse;
		float			DP;
		vec3			Spot;	
		float			distanceDecay = 1.0;

		lightVec.xyz = cPsObjSpaceLightPos[i].xyz - outPos.xyz;
		
		if ( cPsObjSpaceLightPos[i].w > 0.0 )
		{//ポイントライト、スポットタイトの場合、wにはIntensityが入っている。
			distanceDecay = 1.0f / dot( lightVec.xyz, lightVec.xyz ) * cPsObjSpaceLightPos[i].w;
			distanceDecay = min( distanceDecay, 1.0 );
		}
		
		lightVec.xyz = normalize( lightVec.xyz );

		if ( cPsIsSpotLight[i] )
		{
			DP = 1.0;
			Spot = getSpotPow( i, lightVec.xyz, normal.xyz ).xyz;
		}
		else
		{	
			DP = dot( normal.xyz, lightVec.xyz );	
			Spot.xyz = vec3( 1.0, 1.0, 1.0 );
		}

		Spot = max( Spot, 0.0 );
		DP = max( DP, 0.0 );

		Diffuse = cPsObjSpaceLightColor[i].xyz * cDiffuse.xyz;

		{
			vec3			lightColor;

			lightColor = Diffuse * DP * distanceDecay;
			lightColor *= Spot;
			outCol.xyz += lightColor.xyz;
		}
	}

	{
		vec4			ambient = cPsAmbientLightColor * cAmbient;

		outCol.xyz += ambient.xyz;
		outCol.xyz += cEmission.xyz;
	}

	outCol.w = 1.0;
	outCol = min( outCol, 1.0f );

	return outCol;
}

float Z( float t )
{
  float     r = 0.5f;
  float     val;

  t = t * t;

  val = ( 1.0 + (t*t) - t );
  val = val * val;


  return r / val;
}

float A( float pai )
{
  const float     p = 1.0;
  const float     p2 = p * p;
  float           pai2 = pai * pai;
  float           val;

  val = p2 - (p2 * pai2) + pai2;
  val = p / val;

  return sqrt( val );
}

//Anisotropictest = { 0.0f, 1.0f, 0.0f, 32.0f };
//Vector4			VsPow0 = { 1.0f, 5.0f, ChangePai( Anisotropictest.z ), Anisotropictest.w };

vec3 anisotropicLighting( vec3 VsLightVec0, vec3 EyeVec, vec3 TsbTangent, vec3 TsbBiNormal, vec3 TsbNormal )
{
	vec3	Col;
	float	cosA, sinA;
	
	//sincos( 3.14 * 90 / 180, sinA, cosA);
	sinA = sin( 3.14 * 90 / 180);
	cosA = cos( 3.14 * 90 / 180);
	vec3 tang = sinA * TsbTangent + cosA * TsbBiNormal;
	
	float cs = -dot( EyeVec, tang );
	float sn = sqrt(1 - cs * cs);
	
	float cl = dot( VsLightVec0, tang );
	float sl = sqrt(1 - cl * cl);
	
	float				Dir;
	Dir = dot( TsbNormal, VsLightVec0 );
	Dir = max( Dir, 0 );//パワーを０〜１にする
	
	Col.x = Col.y = Col.z = pow( saturate(cs * cl + sn * sl), 1.0 ) * Dir;
	
	return Col;
}


//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
#if 0
float getLutColor( int lutNo, vec3 normal, vec3 lightVec, vec3 viewVec, vec3 halfVec, vec3 spotVec, vec3 tangentVec )
{
	float		outCol = 1.0;
	int			outCnt = 0;

	if ( cLookUpTableEnable[lutNo] )
	{
		float		dp = 1.0;

		if( cLutInputAngle[ lutNo ] == NH )
		{//法線とハーフベクトル （デフォルト）
			dp = dot( normal, halfVec );
		}
		else if( cLutInputAngle[ lutNo ] == VH )
		{//ビューベクトルとハーフベクトル
			dp = dot( viewVec, halfVec );
		}
		else if( cLutInputAngle[ lutNo ] == NV )
		{//法線とビューベクトル
			dp = dot( normal, viewVec );
		}
		else if( cLutInputAngle[ lutNo ] == LN )
		{//ライトベクトルと法線
			dp = dot( lightVec, normal );
		}
		else if( cLutInputAngle[ lutNo ] == SP )
		{//ライトベクトルの逆ベクトルとスポットライトの方向ベクトル
			dp = dot( -lightVec, spotVec );
		}
		else if( cLutInputAngle[ lutNo ] == CP )
		{//ハーフベクトルの接平面への投影と接線
			vec3		binormal = cross( normal, tangentVec );
			vec3		tangent = tangentVec;
			vec3		projVec;

      dp = anisotropicLighting( lightVec, viewVec, tangent, binormal, normal ).x;
		}

		dp += 1.0;
		dp *= 0.5;
		//dp = max( dp, 0.0f );

		if ( lutNo == LutReflectionR )
		{
			if ( cBattleChara20Enable )
			{
				float			adjust = g_vVSColor.x;
				
				adjust *= 2.0f;//0.0 to 2.0
				adjust -= 1.0f;//-1.0 to 1.0
				adjust *= 0.5f;//-0.5 to 0.5
				
				dp += adjust;
				dp = saturate( dp, -1.0, 1.0 );
			}
		}

#if defined HLSL
		dp = LutTextureImage[lutNo].Sample( LutTexture[lutNo], vec2( dp, 0.0) ).x;
#else
		dp = texture2D( LutTexture[lutNo], vec2( dp, 0.0) ).x;
#endif		

		dp = dp * cLutScale[lutNo];
		outCol = saturate( dp );
	}
	
	return outCol;
}
#else
float getLutColor( int lutNo, vec3 normal, vec3 lightVec, vec3 viewVec, vec3 halfVec, vec3 spotVec, vec3 tangentVec )
{
	float		outCol = 1.0;
	int			outCnt = 0;

	if ( cLookUpTableEnable[lutNo] )
	{
		float		dp = 1.0;

		if( cLutInputAngle[ lutNo ] == NH )
		{//法線とハーフベクトル （デフォルト）
			dp = dot( normal, halfVec );
		}
		else if( cLutInputAngle[ lutNo ] == VH )
		{//ビューベクトルとハーフベクトル
			dp = dot( viewVec, halfVec );
		}
		else if( cLutInputAngle[ lutNo ] == NV )
		{//法線とビューベクトル
			dp = dot( normal, viewVec );
		}
		else if( cLutInputAngle[ lutNo ] == LN )
		{//ライトベクトルと法線
			dp = dot( lightVec, normal );
		}
		else if( cLutInputAngle[ lutNo ] == SP )
		{//ライトベクトルの逆ベクトルとスポットライトの方向ベクトル
			dp = dot( -lightVec, spotVec );
		}
		else if( cLutInputAngle[ lutNo ] == CP )
		{//ハーフベクトルの接平面への投影と接線
			vec3		binormal = cross( normal, tangentVec );
			vec3		tangent = tangentVec;
			vec3		projVec;

      dp = anisotropicLighting( lightVec, viewVec, tangent, binormal, normal ).x;
		}

		dp += 1.0;
		dp *= 0.5;
		//dp = max( dp, 0.0f );
		
		if ( lutNo == LutReflectionR )
		{
			if ( cBattleChara20Enable )
			{
				float			adjust = g_vVSColor.x;
				
				adjust *= 2.0f;//0.0 to 2.0
				adjust -= 1.0f;//-1.0 to 1.0
				adjust *= 0.5f;//-0.5 to 0.5
				
				dp += adjust;
				dp = saturate( dp, -1.0, 1.0 );
			}
		}

#if defined HLSL
		dp = LutTextureImage[lutNo].Sample( LutTexture[lutNo], vec2( dp, 0.0) ).x;
#else		
		dp = texture2D( LutTexture[lutNo], vec2( dp, 0.0) ).x;
#endif		

#if defined HLSL
#if  0
		if ( cFsBorderLineMapSampling )
		{
			if ( lutNo == LutReflectionR )
			{
				float		samplingScale;
				vec4		lutImage = LutTextureImage[lutNo].Load( vec3( outPos.x, outPos.y, 0 ) );
				samplingScale = lutImage.x;
				samplingScale = 1.0f;

				for( int i = 0; i < 16; i++)
				{
					vec3				uvPos = vec3( outPos.x, outPos.y, 0 );

					uvPos.x += cGaussianOffset[i].x * samplingScale;
					uvPos.y += cGaussianOffset[i].y * samplingScale;
					lutImage += LutTextureImage[lutNo].Load( uvPos );
				}

				lutImage /= 17;
				float		borderCheck = lutImage.w;//境界線をまたぐようなら1.0fもしくは0.0fにはなっていない。

				dp = lutImage.w;

				if( dp >= 0.3f && dp <= 0.7f )
				{
					dp = 0.5f;
				}

				/*if ( borderCheck == 0.0f || borderCheck == 1.0f )
				{
					dp = LutTextureImage[lutNo].Load( vec3( outPos.x, outPos.y, 0 ) ).w;
				}*/
				
			}
		}
#else
		if ( cFsBorderLineMapSampling )
		{
			if ( lutNo == LutReflectionR )
			{
				vec4		lutImage[33];
				vec4		lutImages = vec4(0, 0, 0, 0);
				vec4		average = vec4(0, 0, 0, 0);

				{
					float				samplingScale = 1.0f;
					lutImage[32] = LutTextureImage[lutNo].Load( vec3( outPos.x, outPos.y, 0 ) );
					samplingScale = lutImage[32].x;

					for( int i = 0; i < 16; i++)
					{
						vec3				uvPos = vec3( outPos.x, outPos.y, 0 );

						uvPos.x += cGaussianOffset[i].x * samplingScale;
						uvPos.y += cGaussianOffset[i].y * samplingScale;
						lutImage[i] = LutTextureImage[lutNo].Load( uvPos );
					}

					samplingScale *= 0.5f;
					for( int i = 0; i < 16; i++)
					{
						vec3				uvPos = vec3( outPos.x, outPos.y, 0 );

						uvPos.x += cGaussianOffset[i].x * samplingScale;
						uvPos.y += cGaussianOffset[i].y * samplingScale;
						lutImage[i+16] = LutTextureImage[lutNo].Load( uvPos );
					}
				}

				float			sampleCount = 0.0f;
				for( int i = 0; i < 33; i++ )
				{
					if ( lutImage[i].x == 0 )
						continue;

					lutImages += lutImage[i];
					sampleCount += 1.0f;
				}

				if ( sampleCount == 0.0f )
				{
					dp = lutImage[32].w;
				}
				else
				{
					average = lutImages;
					average /= sampleCount;

					dp = average.w;
				}

				if( dp >= 0.2f && dp <= 0.8f )
				{
					dp = lerp( 0.5f, dp, 0.5f );
				}
			}
		}
#endif
#endif		

		dp = dp * cLutScale[lutNo];
		outCol = saturate( dp );
	}
	
	return outCol;
}
#endif
//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
vec3 getSpecularM1( vec3 normal, vec3 lightVec, vec3 viewVec, vec3 halfVec )
{
	vec3		spotVec = vec3(0, 0, 0);
	vec3		tangentVec = vec3(0, 0, 0);
	float		outCol[3];
	int			outCnt = 0;

	outCol[0] = cSpecular1.x;
	outCol[1] = cSpecular1.y;
	outCol[2] = cSpecular1.z;

	for( int lutNo = LutReflectionR; lutNo <= LutReflectionB; lutNo ++ )
	{
		if ( cLookUpTableEnable[lutNo] )
		{
			outCol[outCnt] = getLutColor( lutNo, normal, lightVec, viewVec, halfVec, spotVec, tangentVec );
		}
		outCnt ++;
	}
	
	return vec3( outCol[0], outCol[1], outCol[2]);
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
float CalculateBeckmann(float m, float cosbeta)
{
	return ( exp(-(1-( cosbeta * cosbeta ) ) / ( m * m * cosbeta * cosbeta ) ) / ( 4 * m * m * cosbeta * cosbeta * cosbeta * cosbeta ) );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
float CalculateFresnelTerm(float n, float c)
{
	float g = sqrt(n*n + c*c - 1);
	float T1 = ((g-c)*(g-c))/((g+c)*(g+c));
	float T2 = 1 + ( (c*(g+c)-1)*(c*(g+c)-1) )/( (c*(g-c)+1)*(c*(g-c)+1) );
	return 0.5 * T1 * T2;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
#if 0
float getCookTorrance( vec3 normal, vec3 lightVec, vec3 viewVec, vec3 halfVec )
{
	float			Ks = 1.0;

	//　計算に使う角度
	float NV = dot(normal, viewVec);
	float NH = dot(normal, halfVec);
	float VH = dot(viewVec, halfVec);
	float NL = dot(normal, lightVec);

	//　Beckmann分布関数
	float D = CalculateBeckmann( 0.35f, NH );

	//　幾何減衰率
	float G = min( 1, min( 2 * NH * NV / VH, 2 * NH * NL / VH ) );

	//　フレネル項
	float F = CalculateFresnelTerm( 20.0f, dot(lightVec, halfVec) );
	float	value = F * D * G / NV;

	value = max(0.0, value );
	value = min(1.0, value );

	return value * Ks;
}

#else

float getCookTorrance( vec3 normal, vec3 lightVec, vec3 viewVec, vec3 halfVec )
{
	float	rim = dot( normal, viewVec );
	rim = max( rim, 0.0 );
	rim = 1.0 - rim;
	rim = pow( rim, cPsCookTorranceParam.x ) * cPsCookTorranceParam.y;
  rim = max( rim, cPsCookTorranceParam.z );

  return rim;
}
#endif

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
float getclampHighlights( vec3 lightVec, vec3 normal )
{
	float		f = 1.0;

	if ( cSpecularColorClamp )
	{
		f = dot( lightVec, normal );
		if ( f < 0.0 )
			f = 0.0;
		else
			f = 1.0;
	}

	return f;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
vec4 getSecondaryColor()
{
	vec4			outCol = vec4( 0.0, 0.0, 0.0, 1.0 );
	vec3			normal;
	vec3			tangentVec;
	
	normal.xyz = getNormal();
	tangentVec = normalize( outObjTangentVec.xyz );

	for( int i = 0; i < cPsObjSpaceLightCount; i ++ )
	{
		vec3			lightVec;
		vec3			viewVec;
		vec3			halfVec;
		vec3			spotVec;

		vec3			Specular0 = vec3( 1.0, 1.0, 1.0 );
		vec3			SpecularM0 = cSpecular0.xyz;
		vec3			SpecularL0 = vec3( 1.0, 1.0, 1.0 );
		vec3			Distribution0 = vec3( 1.0, 1.0, 1.0 );
		vec3			Geometry0 = vec3( 1.0, 1.0, 1.0 );

		vec3			Specular1 = vec3( 1.0, 1.0, 1.0 );
		vec3			SpecularM1 = vec3( 1.0, 1.0, 1.0 );
		vec3			SpecularL1 = vec3( 1.0, 1.0, 1.0 );
		vec3			Distribution1 = vec3( 1.0, 1.0, 1.0 );
		vec3			Geometry1 = vec3( 1.0, 1.0, 1.0 );
		vec3			Spot = vec3( 1.0, 1.0, 1.0 );
		float			F;
		float			DistAttr = 1.0;

		lightVec = cPsObjSpaceLightPos[i].xyz - outPos.xyz;
		
		if ( cPsObjSpaceLightPos[i].w > 0.0 )
		{//ポイントライト、スポットタイトの場合、wにはIntensityが入っている。
			DistAttr = 1.0f / dot( lightVec.xyz, lightVec.xyz ) * cPsObjSpaceLightPos[i].w;
			DistAttr = min( DistAttr, 1.0 );
		}
		
		lightVec = normalize( lightVec );

		//viewVec = cPsObjSpaceCameraPosition.xyz - outPos.xyz;
		//viewVec = normalize( viewVec );
		viewVec = normalize( outView.xyz );

		halfVec = viewVec + lightVec;
		halfVec = normalize( halfVec );

		if ( cPsIsSpotLight[i] )
		{
			spotVec.xyz = normalize( cPsObjSpaceLightVec[i].xyz );
			Spot = getSpotPow( i, lightVec, normal );
			Spot = max( Spot, 0.0 );
		}
		else
		{
			spotVec = -lightVec;//getLutColor関数にて内積結果が１になれば、無効と同じ
		}

		SpecularM1 = getSpecularM1( normal, lightVec, viewVec, halfVec );
		Distribution0.x = Distribution0.y = Distribution0.z = getLutColor( LutDistribution0, normal, lightVec, viewVec, halfVec, spotVec, tangentVec );
		Distribution1.x = Distribution1.y = Distribution1.z = getLutColor( LutDistribution1, normal, lightVec, viewVec, halfVec, spotVec, tangentVec );

		float		ct = getCookTorrance( normal, lightVec, viewVec, halfVec );

		if ( cCookTorrance0Enable )
			Geometry0 = vec3( ct, ct, ct);

		if ( cCookTorrance1Enable )
			Geometry1 = vec3( ct, ct, ct);

		//マテリアルにライトカラーを適応
		SpecularM0 = SpecularM0 * cPsObjSpaceLightColor[i].xyz;
		SpecularM1 = SpecularM1 * cPsObjSpaceLightColor[i].xyz;

		Specular0 = SpecularM0 * SpecularL0 * Distribution0 * Geometry0;
		Specular1 = SpecularM1 * SpecularL1 * Distribution1 * Geometry1;

		F = getclampHighlights( lightVec, normal );

		outCol.xyz += ( Specular0 + Specular1 ) * Spot * F * DistAttr;
	}

	return outCol;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
float getFresnelColor()
{
	vec4			outCol = vec4( 0.0, 0.0, 0.0, 1.0 );
	vec3			normal;
	vec3			tangentVec;
	
	normal.xyz = getNormal();
	tangentVec = normalize( outObjTangentVec.xyz );

	{
		vec3			lightVec;
		vec3			viewVec;
		vec3			halfVec;
		vec3			spotVec;

		lightVec = cPsObjSpaceLightPos[0].xyz - outPos.xyz;
		lightVec = normalize( lightVec );

		//viewVec = cPsObjSpaceCameraPosition.xyz - outPos.xyz;
		//viewVec = normalize( viewVec );
		viewVec = normalize( outView.xyz );

		halfVec = viewVec + lightVec;
		halfVec = normalize( halfVec );

		spotVec = -lightVec;//getLutColor関数にて内積結果が１になれば、無効と同じ

		outCol.x = outCol.y = outCol.z = getLutColor( LutFresnel, normal, lightVec, viewVec, halfVec, spotVec, tangentVec );
	}

	return outCol.x;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void readInputValue()
{
	g_vVSColor = outColor;

	for( int i = 0; i < CombinerStageMax; i ++ )
	{
		int		no = cTextureCombinerConstant[i];
		g_vConstnat[i] = cConsntantColor[no];
	}

	for( int i = 0; i < TextureMax; i ++ )
	{
		g_vTexture[i] = getTextureColor( i );
	}

	if ( cUseShadowTexture )
	{//差分データに変換
		g_vTexture[1] = g_vTexture[1] - g_vTexture[0];//-1.0f 〜 +1.0f
		g_vTexture[1] += 1.0f;												//0.0f 〜 +2.0f
		g_vTexture[1] *= 0.5f;												//0.0f 〜 +1.0f
		g_vTexture[1].w = g_vTexture[0].w;
	}

	if ( cBumpMapNo >= 0 )
	{
		g_vNormalTexture = g_vTexture[cBumpMapNo];
	}

	if ( cHigLightTextureEnable )
	{
		vec4		uv = getTextureUv( cBumpMapNo );
#if defined HLSL		
		g_vHighLightTexture = HighLightTextureImage.Sample( HighLightTexture, uv.xy );
#else		
		g_vHighLightTexture = texture2D( HighLightTexture, uv.xy );
#endif		
		g_vNormalTexture.xyz *= g_vHighLightTexture.xyz;
	}

	g_vBuffer += cBufferColor;

	g_vPrimary = getPrimaryColor();
	g_vSecondary = getSecondaryColor(); 

	float		alpha = getFresnelColor();

	if ( cPrimaryFresnelEnable )
		g_vPrimary.w = alpha;

	if ( cSecondaryFresnelEnable )
		g_vSecondary.w = alpha;
}

//-----------------------------------------------------------------------------
/// @brief	ソース取得関数
//-----------------------------------------------------------------------------
vec3 getSourceRgb( int colorType, int stageNo, int sourceType )
{
	vec4			sourceRgba;
	int				inputSourceNo = cInputSource[colorType][stageNo][sourceType];
	int				sourceOperandNo = cSourceOperand[colorType][stageNo][sourceType];

	if ( inputSourceNo == InputSourceTypeTexture0 )							sourceRgba = g_vTexture[0];
	else if ( inputSourceNo == InputSourceTypeTexture1 )				sourceRgba = g_vTexture[1];
	else if ( inputSourceNo == InputSourceTypeTexture2 )				sourceRgba = g_vTexture[2];
	else if ( inputSourceNo == InputSourceTypeTexture3 )				sourceRgba = g_vTexture[3];
	else if ( inputSourceNo == InputSourceTypeConstnat )				sourceRgba = g_vConstnat[stageNo];
	else if ( inputSourceNo == InputSourceTypeVSColor )					sourceRgba = g_vVSColor;
	else if ( inputSourceNo == InputSourceTypePrimary )					sourceRgba = g_vPrimary;
	else if ( inputSourceNo == InputSourceTypeSecondary )				sourceRgba = g_vSecondary;
	else if ( inputSourceNo == InputSourceTypeBeforeAnswer )		sourceRgba = g_vAnswer;
	else if ( inputSourceNo == InputSourceTypeBeforeBuffer )		sourceRgba = g_vBuffer;


	if ( sourceOperandNo == OperandTypeRGB )										sourceRgba = sourceRgba;
	else if ( sourceOperandNo == OperandType1_RGB )							sourceRgba = 1.0 - sourceRgba;
	else if ( sourceOperandNo == OperandTypeA )									sourceRgba = sourceRgba.wwww;
	else if ( sourceOperandNo == OperandType1_A )								sourceRgba = 1.0 - sourceRgba.wwww;
	else if ( sourceOperandNo == OperandTypeR )									sourceRgba = sourceRgba.xxxx;
	else if ( sourceOperandNo == OperandType1_R )								sourceRgba = 1.0 - sourceRgba.xxxx;
	else if ( sourceOperandNo == OperandTypeG )									sourceRgba = sourceRgba.yyyy;
	else if ( sourceOperandNo == OperandType1_G )								sourceRgba = 1.0 - sourceRgba.yyyy;
	else if ( sourceOperandNo == OperandTypeB )									sourceRgba = sourceRgba.zzzz;
	else if ( sourceOperandNo == OperandType1_B )								sourceRgba = 1.0 - sourceRgba.zzzz;

	return sourceRgba.xyz;
}

//-----------------------------------------------------------------------------
/// @brief	ソース取得関数
//-----------------------------------------------------------------------------
float getSourceAlpha( int colorType, int stageNo, int sourceType )
{
	vec4			sourceRgba;
	int				inputSourceNo = cInputSource[colorType][stageNo][sourceType];
	int				sourceOperandNo = cSourceOperand[colorType][stageNo][sourceType];

	if ( inputSourceNo == InputSourceTypeTexture0 )							sourceRgba = g_vTexture[0];
	else if ( inputSourceNo == InputSourceTypeTexture1 )				sourceRgba = g_vTexture[1];
	else if ( inputSourceNo == InputSourceTypeTexture2 )				sourceRgba = g_vTexture[2];
	else if ( inputSourceNo == InputSourceTypeTexture3 )				sourceRgba = g_vTexture[3];
	else if ( inputSourceNo == InputSourceTypeConstnat )				sourceRgba = g_vConstnat[stageNo];
	else if ( inputSourceNo == InputSourceTypeVSColor )					sourceRgba = g_vVSColor;
	else if ( inputSourceNo == InputSourceTypePrimary )					sourceRgba = g_vPrimary;
	else if ( inputSourceNo == InputSourceTypeSecondary )				sourceRgba = g_vSecondary;
	else if ( inputSourceNo == InputSourceTypeBeforeAnswer )		sourceRgba = g_vAnswer;
	else if ( inputSourceNo == InputSourceTypeBeforeBuffer )		sourceRgba = g_vBuffer;


	if ( sourceOperandNo == OperandTypeRGB )										sourceRgba = sourceRgba;
	else if ( sourceOperandNo == OperandType1_RGB )							sourceRgba = 1.0 - sourceRgba;
	else if ( sourceOperandNo == OperandTypeA )									sourceRgba = sourceRgba.wwww;
	else if ( sourceOperandNo == OperandType1_A )								sourceRgba = 1.0 - sourceRgba.wwww;
	else if ( sourceOperandNo == OperandTypeR )									sourceRgba = sourceRgba.xxxx;
	else if ( sourceOperandNo == OperandType1_R )								sourceRgba = 1.0 - sourceRgba.xxxx;
	else if ( sourceOperandNo == OperandTypeG )									sourceRgba = sourceRgba.yyyy;
	else if ( sourceOperandNo == OperandType1_G )								sourceRgba = 1.0 - sourceRgba.yyyy;
	else if ( sourceOperandNo == OperandTypeB )									sourceRgba = sourceRgba.zzzz;
	else if ( sourceOperandNo == OperandType1_B )								sourceRgba = 1.0 - sourceRgba.zzzz;

	return sourceRgba.w;
}

//-----------------------------------------------------------------------------
/// @brief	コンバイナRGB関数
//-----------------------------------------------------------------------------
void combinerRgbFunc( int stageNo )
{
	vec3	sourceA = getSourceRgb( ColorTypeRGB, stageNo, SourceTypeA );
	vec3	sourceB = getSourceRgb( ColorTypeRGB, stageNo, SourceTypeB );
	vec3	sourceC = getSourceRgb( ColorTypeRGB, stageNo, SourceTypeC );

	int		mathFormulaType = cMathFormula[ColorTypeRGB][stageNo];

	if ( mathFormulaType == MathFormulaA )
	{
		g_vAnswer.xyz = sourceA;
	}
	else if ( mathFormulaType == MathFormulaAxB )
	{
		g_vAnswer.xyz = sourceA * sourceB;
	}
	else if ( mathFormulaType == MathFormulaAaddB )
	{
		g_vAnswer.xyz = sourceA + sourceB;
	}
	else if ( mathFormulaType == MathFormulaAaddBsub05 )
	{
		g_vAnswer.xyz = sourceA + sourceB - 0.5;
	}
	else if ( mathFormulaType == MathFormulaAxCaddBx1subC )
	{
		g_vAnswer.xyz = (sourceA * sourceC) + (sourceB * (1 - sourceC));
	}
	else if ( mathFormulaType == MathFormulaAsubB )
	{
		g_vAnswer.xyz = sourceA - sourceB;
	}
	else if ( mathFormulaType == MathFormulaRGBDot )
	{
		g_vAnswer.x = g_vAnswer.y = g_vAnswer.z = dot(sourceA, sourceB);
	}
	else if ( mathFormulaType == MathFormulaRGBADot )
	{
		g_vAnswer.x = g_vAnswer.y = g_vAnswer.z = dot(sourceA, sourceB);
	}
	else if ( mathFormulaType == MathFormulaAaddBxC )
	{
		g_vAnswer.xyz = (sourceA + sourceB) * sourceC;
	}
	else if ( mathFormulaType == MathFormulaAxBaddC )
	{
		g_vAnswer.xyz = (sourceA * sourceB) + sourceC;
	}

	g_vAnswer.xyz = g_vAnswer.xyz * cScale[ColorTypeRGB][stageNo];

	if ( cOutPutBuffer[ColorTypeRGB][stageNo] )
	{
		g_vBuffer.xyz = g_vAnswer.xyz;
	}
	
	finalColorRgb.xyz = g_vAnswer.xyz;
}

//-----------------------------------------------------------------------------
/// @brief	コンバイナAlpha関数
//-----------------------------------------------------------------------------
void combinerAlphaFunc( int stageNo )
{
	float	sourceA = getSourceAlpha( ColorTypeA, stageNo, SourceTypeA );
	float	sourceB = getSourceAlpha( ColorTypeA, stageNo, SourceTypeB );
	float	sourceC = getSourceAlpha( ColorTypeA, stageNo, SourceTypeC );

	int		mathFormulaType = cMathFormula[ColorTypeA][stageNo];

	if ( mathFormulaType == MathFormulaA )
	{
		g_vAnswer.w = sourceA;
	}
	else if ( mathFormulaType == MathFormulaAxB )
	{
		g_vAnswer.w = sourceA * sourceB;
	}
	else if ( mathFormulaType == MathFormulaAaddB )
	{
		g_vAnswer.w = sourceA + sourceB;
	}
	else if ( mathFormulaType == MathFormulaAaddBsub05 )
	{
		g_vAnswer.w = sourceA + sourceB - 0.5;
	}
	else if ( mathFormulaType == MathFormulaAxCaddBx1subC )
	{
		g_vAnswer.w = (sourceA * sourceC) + (sourceB * (1 - sourceC));
	}
	else if ( mathFormulaType == MathFormulaAsubB )
	{
		g_vAnswer.w = sourceA - sourceB;
	}
	else if ( mathFormulaType == MathFormulaRGBDot )
	{
		g_vAnswer.w = dot(sourceA, sourceB);
	}
	else if ( mathFormulaType == MathFormulaRGBADot )
	{
		g_vAnswer.w = dot(sourceA, sourceB);
	}
	else if ( mathFormulaType == MathFormulaAaddBxC )
	{
		g_vAnswer.w = (sourceA + sourceB) * sourceC;
	}
	else if ( mathFormulaType == MathFormulaAxBaddC )
	{
		g_vAnswer.w = (sourceA * sourceB) + sourceC;
	}

	g_vAnswer.w = g_vAnswer.w * cScale[ColorTypeA][stageNo];

	if ( cOutPutBuffer[ColorTypeA][stageNo] )
	{
		g_vBuffer.w = g_vAnswer.w;
	}
	
	finalColorRgb.w = g_vAnswer.w;
}

//-----------------------------------------------------------------------------
/// @brief	メイン関数
//-----------------------------------------------------------------------------
#if defined HLSL
float4 PsMain( PsInput input ) : SV_Target
{
	//outPos = input.Position;
	outPos = input.DebugVec;
	
	for( int i = 0; i < TextureMax; i ++ )
		outUv[i] = input.Uv[i];
	outColor = input.Color;
	outObjNormal = input.Normal;
	outObjLight = input.ObjLight;
	outObjTangentVec = input.ObjTangentVec;
	outView = input.View;
	outDebugVec = input.DebugVec;
#else
void main (void)
{
#endif	
	readInputValue();
	
	{
  vec4	sourceRgba;
  vec3	sourceA;
  vec3	sourceB;
  vec3	sourceC;
  sourceRgba = g_vTexture[0];
  sourceRgba = sourceRgba;
  sourceA = sourceRgba.xyz;
  sourceRgba = g_vTexture[0];
  sourceRgba = sourceRgba;
  sourceB = sourceRgba.xyz;
  sourceRgba = g_vTexture[0];
  sourceRgba = sourceRgba;
  sourceC = sourceRgba.xyz;
  int		mathFormulaType = cMathFormula[ColorTypeRGB][0];
  g_vAnswer.xyz = sourceA;
  g_vAnswer.xyz = g_vAnswer.xyz * cScale[ColorTypeRGB][0];
  g_vAnswer.xyz = saturate( g_vAnswer.xyz );
  finalColorRgb.xyz = g_vAnswer.xyz;
}
{
  vec4	  sourceRgba;
  float	sourceA;
  float	sourceB;
  float	sourceC;
  int		mathFormulaType = cMathFormula[ColorTypeA][0];
  sourceRgba = g_vTexture[0];
  sourceRgba = sourceRgba.wwww;
  sourceA = sourceRgba.w;
  sourceRgba = g_vTexture[0];
  sourceRgba = sourceRgba.wwww;
  sourceB = sourceRgba.w;
  sourceRgba = g_vTexture[0];
  sourceRgba = sourceRgba.wwww;
  sourceC = sourceRgba.w;
  g_vAnswer.w = sourceA;
  g_vAnswer.w = g_vAnswer.w * cScale[ColorTypeA][0];
  g_vAnswer.w = saturate( g_vAnswer.w );
  finalColorRgb.w = g_vAnswer.w;
}

	gl_FragColor = finalColorRgb;
	
#if defined HLSL

	#if 0
	if ( cFsRenderNormalMap )
	{
		float3					worldViewNormal = input.DebugVec.xyz;

		gl_FragColor.xyz *= 0.0001;//ほぼ打ち消す

		if ( cFsDisableNormalColor )
		{
			gl_FragColor = vec4( 0.5, 0.5, 0.0, 1.0 );
		}
		else
		{
			worldViewNormal += 1.0;
			worldViewNormal *= 0.5;
			gl_FragColor.xyz += worldViewNormal.xyz;
			gl_FragColor.w = outColor.w;
		}

		gl_FragColor.w *= input.Color.x;//αは0 or 1でないとだめ。
	}

	if ( 1 )
	{//素材ノイズ
		vec4				materialNoizeUv = vec4(outPos.x, outPos.y, 0, 0);
		vec4				materialNoizeColor;
		float				materialNoizeScale = cFsParam[0].z;

		if ( 0 )
		{
			materialNoizeUv = g_vTexture[cBumpMapNo];
			materialNoizeUv.y *= -1.0f;
		}
		else
		{
			materialNoizeUv = materialNoizeUv / cFsScreanSize;
			//noizeUv *= 1.0f;
		}

		materialNoizeColor = MaterialNoizeTextureImage.Sample( MaterialNoizeTexture, materialNoizeUv.xy );
		materialNoizeColor = materialNoizeColor.x + materialNoizeColor.y + materialNoizeColor.z;
		materialNoizeColor /= 3;
		materialNoizeColor *= materialNoizeScale;

		materialNoizeColor = gl_FragColor * materialNoizeColor;

		gl_FragColor += materialNoizeColor;
		//gl_FragColor = MaterialNoizeTextureImage.Sample( BrushStrokeTexture, materialNoizeUv.xy );
	}

	if ( 1 )
	{//筆ムラ
		vec4				brushStrokeUv = vec4(outPos.x, outPos.y, 0, 0);
		vec4				brushStrokeColor;
		float				brushStrokeScale = cFsParam[1].x;

		brushStrokeUv = brushStrokeUv / cFsScreanSize;
		//noizeUv *= 1.0f;

		brushStrokeColor = BrushStrokeTextureImage.Sample( BrushStrokeTexture, brushStrokeUv.xy );
		brushStrokeColor = brushStrokeColor.x + brushStrokeColor.y + brushStrokeColor.z;
		brushStrokeColor /= 3;
		brushStrokeColor *= brushStrokeScale;

		brushStrokeColor = gl_FragColor * brushStrokeColor;

		gl_FragColor += brushStrokeColor;
		//gl_FragColor = BrushStrokeTextureImage.Sample( BrushStrokeTexture, brushStrokeUv.xy );
	}
	#endif
	return gl_FragColor;
	
#endif
}





