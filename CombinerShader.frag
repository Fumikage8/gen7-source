// -------------------------------------------------------------
// 定数
// -------------------------------------------------------------
const int InputSourceTypeTexture0 = 0;
const int InputSourceTypeTexture1 = 1;
const int InputSourceTypeTexture2 = 2;
const int InputSourceTypeTexture3 = 3;
const int InputSourceTypeConstnat = 4;
const int InputSourceTypeVSColor = 5;
const int InputSourceTypePrimary = 6;
const int InputSourceTypeSecondary = 7;
const int InputSourceTypeBeforeAnswer = 8;
const int InputSourceTypeBeforeBuffer = 9;

const int SourceTypeA = 0;
const int SourceTypeB = 1;
const int SourceTypeC = 2;
const int SourceTypeMax = 3;

const int OperandTypeRGB = 0;
const int OperandType1_RGB = 1;
const int OperandTypeA = 2;
const int OperandType1_A = 3;
const int OperandTypeR = 4;
const int OperandType1_R = 5;
const int OperandTypeG = 6;
const int OperandType1_G = 7;
const int OperandTypeB = 8;
const int OperandType1_B = 9;
const int OperandTypeMax = 10;

const int ColorTypeRGB = 0;
const int ColorTypeA = 1;
const int ColorTypeMax = 2;

const int MathFormulaA = 0;
const int MathFormulaAxB = 1;
const int MathFormulaAaddB = 2;
const int MathFormulaAaddBsub05 = 3;
const int MathFormulaAxCaddBx1subC = 4;
const int MathFormulaAsubB = 5;
const int MathFormulaRGBDot = 6;
const int MathFormulaRGBADot = 7;
const int MathFormulaAaddBxC = 8;
const int MathFormulaAxBaddC = 9;

const int LutDistribution0 = 0;
const int LutDistribution1 = 1;
const int LutReflectionR = 2;
const int LutReflectionG = 3;
const int LutReflectionB = 4;
const int LutFresnel = 5;
const int LutMax = 6;

const int BumpMap = 0;
const int TangentMap = 1;

const int NH = 0;//法線とハーフベクトル （デフォルト）
const int VH = 1;//ビューベクトルとハーフベクトル
const int NV = 2;//法線とビューベクトル
const int LN = 3;//ライトベクトルと法線
const int SP = 4;//ライトベクトルの逆ベクトルとスポットライトの方向ベクトル（RR, RG, RB, FR
const int CP = 5;//ハーフベクトルの接平面への投影と接線（RR, RG, RB, FR は不可）

const int TextureMax = 4;
const int UvMax = 3;
const int LightMax = 8;

// -------------------------------------------------------------
// 頂点プログラムからフラグメントプログラムに渡すデータ
// -------------------------------------------------------------
varying vec4 outPos;
varying vec4 outUv[TextureMax];
varying vec4 outColor;
varying vec3 outObjNormal;
varying vec3 outObjLight;
varying vec3 outObjTangentVec[TextureMax];
varying vec4 outView;
varying vec4 outDebugVec;

// -------------------------------------------------------------
// グローバル変数
// -------------------------------------------------------------

const int CombinerStageMax = 6;
uniform int cCombinerStageMax = CombinerStageMax;

uniform sampler2D Texture[TextureMax];//テクスチャユニット
uniform samplerCube CubeTexture[TextureMax];//テクスチャユニット
uniform sampler2D HighLightTexture;//テクスチャユニット

uniform vec4 cPsObjSpaceCameraPosition;

//-----カラー-------
uniform vec4 cEmission;
uniform vec4 cAmbient;
uniform vec4 cDiffuse;
uniform vec4 cSpecular0;
uniform vec4 cSpecular1;
uniform vec4 cConsntantColor[CombinerStageMax];
uniform vec4 cBufferColor;

//-------- ライト情報 --------------
uniform int cPsObjSpaceLightCount;
uniform vec4 cPsObjSpaceLightPos[LightMax];
uniform vec4 cPsObjSpaceLightColor[LightMax];
uniform vec4 cPsObjSpaceLightVec[LightMax];
uniform float cPsObjSpaceLightConeAngle[LightMax];
uniform bool cPsIsSpotLight[LightMax];
uniform vec4 cPsAmbientLightColor;

//-------- 参照テーブル情報 --------------
uniform sampler2D LutTexture[LutMax];
uniform bool cLookUpTableEnable[LutMax];
uniform int cLutInputAngle[LutMax];
uniform float cLutScale[LutMax];
uniform bool cCookTorrance0Enable;
uniform bool cCookTorrance1Enable;
uniform bool cSpecularColorClamp;
uniform bool cPrimaryFresnelEnable;
uniform bool cSecondaryFresnelEnable;
uniform vec4 cPsCookTorranceParam;

//-----テクスチャ-------
uniform bool cTextureEnable[TextureMax];
uniform bool cTexture2D[TextureMax];

uniform int cBumpMapNo;
uniform int cTangentNo;
uniform int cBumpMapType;
uniform bool cZRecastEnable;

uniform float cScaleU[TextureMax];
uniform float cScaleV[TextureMax];
uniform float cRotate[TextureMax];
uniform float cTranslateU[TextureMax];
uniform float cTranslateV[TextureMax];

//-----フラグメント-------
uniform int cMathFormula[ColorTypeMax][CombinerStageMax];
uniform float cScale[ColorTypeMax][CombinerStageMax];
uniform int cInputSource[ColorTypeMax][CombinerStageMax][SourceTypeMax];
uniform int cSourceOperand[ColorTypeMax][CombinerStageMax][SourceTypeMax];
uniform bool cOutPutBuffer[ColorTypeMax][CombinerStageMax];
uniform int cTextureCombinerConstant[CombinerStageMax];

//-----特殊フラグ-------
uniform bool cUseShadowTexture;
uniform bool cHigLightTextureEnable;
uniform bool cFsIsPokemonShader;
uniform bool cBattleChara20Enable;

//-----Ｆｏｇ-------
uniform vec4 cPsFogColor;//(r, g, b, 1.0f)
uniform vec4 cPsFogParam;//(FogIntencity, FogNear, FogFar, 1.0f)
uniform int cPsFogType = 0;//None=0;Linear;Log;Log2;

//---------- コンバイナで扱える変数 ---------------------

vec4					g_vTexture[TextureMax];
vec4					g_vNormalTexture;
vec4					g_vHighLightTexture;
vec4					g_vConstnat[CombinerStageMax];
vec4					g_vVSColor = vec4(0.0, 0.0, 1.0, 0.0);
vec4					g_vPrimary = vec4(1.0, 0.0, 0.0, 0.0);
vec4					g_vSecondary = vec4(0.0, 1.0, 0.0, 0.0);
vec4					g_vAnswer = vec4(0.0, 0.0, 0.0, 0.0);
vec4					g_vBuffer = vec4(1.0, 0.0, 0.0, 0.0);
vec4					g_vBeforeAnswer = vec4(0.0, 0.0, 0.0, 0.0);

//-------------------------------

vec4					finalColorRgb;

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
vec3 satulate( vec3 value )
{
	value = max( 0.0, value );
	value = min( 1.0, value );

	return value;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
float satulate( float value )
{
	value = max( 0.0, value );
	value = min( 1.0, value );

	return value;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
vec3 satulate( vec3 value, float minValue, float maxValue )
{
	value = max( minValue, value );
	value = min( maxValue, value );

	return value;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
float satulate( float value, float minValue, float maxValue )
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
		color = texture2D( Texture[texNo], uv.xy );
	}
	else
	{
		vec3		nor = uv.xyz;
		nor.y = -nor.y;
		nor.z = -nor.z;
		color = textureCube( CubeTexture[texNo], nor );
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
		tangent.xyz = normalize( outObjTangentVec[cTangentNo].xyz );
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
    
	  normal = tangentSpaceMatrix * normal;
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
	Spot.xyz = dot( normal.xyz, spotVec.xyz );
	
	spotArea = dot( lightVec.xyz, spotVec.xyz );
	if ( spotArea < cPsObjSpaceLightConeAngle[lightNo] )
	{
		Spot.xyz = 0.0;
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
			Spot = getSpotPow( i, lightVec, normal );
		}
		else
		{	
			DP = dot( normal.xyz, lightVec.xyz );	
			Spot.xyz = 1.0;
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
	
	sincos( 3.14 * 90 / 180, sinA, cosA);
	vec3 tang = sinA * TsbTangent + cosA * TsbBiNormal;
	
	float cs = -dot( EyeVec, tang );
	float sn = sqrt(1 - cs * cs);
	
	float cl = dot( VsLightVec0, tang );
	float sl = sqrt(1 - cl * cl);
	
	float				Dir;
	Dir = dot( TsbNormal, VsLightVec0 );
	Dir = max( Dir, 0 );//パワーを０～１にする
	
	Col.xyz = pow( saturate(cs * cl + sn * sl), 1.0 ) * Dir;
	
	return Col;
}


//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
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

			/*tangent *= dot( -halfVec, tangent );
			binormal *= dot( -halfVec, binormal );

			projVec = tangent + binormal;
			projVec = normalize( projVec );

			dp = dot( projVec, tangentVec );*/

      //　計算に使う角度
	    /*float NV = dot(normal, viewVec);
	    float NH = dot(normal, halfVec);
	    float VH = dot(viewVec, halfVec);
	    float NL = dot(normal, lightVec);

	    //　幾何減衰率
	    float G = min( 1, min( 2 * NH * NV / VH, 2 * NH * NL / VH ) );

      dp = Z( NH ) * A( dot( projVec, tangentVec ) );*/

      
			/*float cosH = -dot( halfVec, tangent );
			float sinH = sqrt(1.0 - cosH * cosH);

			projVec = tangent * cosH;
			projVec += binormal * sinH;
			projVec = normalize( projVec );

			dp = dot( projVec, tangent );*/

			/*normal *= dot( halfVec, normal );
			halfVec = halfVec - normal;
			projVec = normalize( -halfVec );

			dp = dot( projVec, tangent );*/

			/*mat3x3		tangentSpaceMatrix;

			normal.xyz = normalize( outObjNormal.xyz );
			tangent.xyz = normalize( outObjTangentVec[cTangentNo].xyz );
			binormal.xyz = cross( normal.xyz, tangent.xyz ).xyz;

			tangentSpaceMatrix[0] = tangent.xyz;
			tangentSpaceMatrix[1] = binormal.xyz;
			tangentSpaceMatrix[2] = normal.xyz;

			tangentSpaceMatrix = inverse( tangentSpaceMatrix );
			tangent = tangentSpaceMatrix * tangent;

			dp = dot( tangent, halfVec );*/
		}
		
		//dp = -1.0 ～ 1.0
		dp += 1.0;//0.0 ～ 2.0
		dp *= 0.5;//0.0 ～ 1.0

		if ( lutNo == LutReflectionR )
		{
			if ( cBattleChara20Enable )
			{
				float			adjust = g_vVSColor.x;
				
				adjust *= 2.0f;//0.0 to 2.0
				adjust -= 1.0f;//-1.0 to 1.0
				adjust *= 0.5f;//-0.5 to 0.5
				
				dp += adjust;
				dp = satulate( dp, -1.0, 1.0 );
			}
		}
		
		dp = texture2D( LutTexture[lutNo], vec2( dp, 0.0) ).x;
		dp = dp * cLutScale[lutNo];
		outCol = satulate( dp, 0.0, 1.0 );
	}
	
	return outCol;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
vec3 getSpecularM1( vec3 normal, vec3 lightVec, vec3 viewVec, vec3 halfVec )
{
	vec3		spotVec;
	vec3		tangentVec;
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
	tangentVec = normalize( outObjTangentVec[cTangentNo].xyz );

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
		Distribution0.xyz = getLutColor( LutDistribution0, normal, lightVec, viewVec, halfVec, spotVec, tangentVec );
		Distribution1.xyz = getLutColor( LutDistribution1, normal, lightVec, viewVec, halfVec, spotVec, tangentVec );

		float		ct = getCookTorrance( normal, lightVec, viewVec, halfVec );

		if ( cCookTorrance0Enable )
			Geometry0 = ct;

		if ( cCookTorrance1Enable )
			Geometry1 = ct;

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
	tangentVec = normalize( outObjTangentVec[cTangentNo].xyz );

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

		outCol.xyz = getLutColor( LutFresnel, normal, lightVec, viewVec, halfVec, spotVec, tangentVec );
	}

	return outCol.x;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void readInputValue()
{
	g_vVSColor = outColor;

	for( int i = 0; i < cCombinerStageMax; i ++ )
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
		g_vTexture[1] = g_vTexture[1] - g_vTexture[0];//-1.0f ～ +1.0f
		g_vTexture[1] += 1.0f;												//0.0f ～ +2.0f
		g_vTexture[1] *= 0.5f;												//0.0f ～ +1.0f
		g_vTexture[1].w = g_vTexture[0].w;
	}

	if ( cBumpMapNo >= 0 )
	{
		g_vNormalTexture = g_vTexture[cBumpMapNo];
	}

	if ( cHigLightTextureEnable )
	{
		vec4		uv = getTextureUv( cBumpMapNo );
		g_vHighLightTexture = texture2D( HighLightTexture, uv.xy );
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
/// @brief	カラー内積関数
//-----------------------------------------------------------------------------
float		dotRGB;

vec3 DotRGB( vec3 A, vec3 B )
{
	A *= 2.0;
	A -= 1.0;
	
	B *= 2.0;
	B -= 1.0;
	
	dotRGB = dot( A, B );
	
	return vec3(dotRGB, dotRGB, dotRGB);
}

float DotRGBA( float A, float B )
{
	return dotRGB;
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
		g_vAnswer.xyz = dot(sourceA, sourceB);
	}
	else if ( mathFormulaType == MathFormulaRGBADot )
	{
		g_vAnswer.xyz = dot(sourceA, sourceB);
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
void main (void)
{
	readInputValue();
	
	<include>

	gl_FragColor = finalColorRgb;
		
	// 距離フォグ
	if( cPsFogType==1 )
	{
		float distance		= outPos.z;
		float blend	   	 	= (cPsFogParam.z - distance) / (cPsFogParam.z - cPsFogParam.y);
		blend		   	 	= max( min(1.0f,blend) , 0.0f );		
		gl_FragColor.rgb   	= (gl_FragColor.rgb*blend) + (cPsFogColor.rgb*(1.0f-blend));
	}
	
	//gl_FragColor = outDebugVec;
}
