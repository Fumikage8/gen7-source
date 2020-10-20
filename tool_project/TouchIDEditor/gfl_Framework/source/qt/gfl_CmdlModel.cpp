//==============================================================================

#include <gflib.h>

#include "qt/gfl_CmdlModel.h"
#include "motion/gfl_JointBase.h"

#ifdef MAYA_INPORT
	#include "qt/Shader/ShaderObject.h"
	#include "qt/Shader/CombinerShader.h"
#include "qt/Shader/UvAdjustShader.h"
#include "qt/gfl_Render.h"
#endif

/**
 * @file	gfl_CmdlModel.cpp
 * @brief	
 * @author	kimura_shin
 * @data	2012/03/13, 16:31
 */
// =============================================================================

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void gfl::cmdl::Model::RenderPrimitiveWireFrame( )
{
	glDisable( GL_LIGHTING );
	Mesh* mesh = GetMesh( );
	for( s32 i = 0; i < GetMeshCount( ); ++i, ++mesh ){
		Shape* shape = mesh->GetShape( );
		shape->RenderPolygonWireFrame( shape->GetVertices( ) );
	}
	glEnable( GL_LIGHTING );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void gfl::cmdl::Material::MaterialColor::SetLight()
{
	// light
	glLightfv( GL_LIGHT0, GL_EMISSION, GetEmission( ).Get( ) );
	glLightfv( GL_LIGHT0, GL_AMBIENT, GetAmbient( ).Get( ) );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, GetDiffuse( ).Get( ) );
	glLightfv( GL_LIGHT0, GL_SPECULAR, GetSpecular( 0 ).Get( ) );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void gfl::cmdl::Material::TextureCoordinator::SetTextureEnv(const u32 tex_id)
{
	glBindTexture( GL_TEXTURE_2D, tex_id );
	glMatrixMode( GL_TEXTURE );
	glLoadIdentity( );
	glScalef( GetScaleS( ), GetScaleT( ), 1.0f );
	//		glRotatef
	glTranslatef( GetTranslateS( ), GetTranslateT( ), 0.0f );

	glMatrixMode( GL_MODELVIEW );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void gfl::cmdl::Material::TextureMapper::SetTextureEnv()
{
	//glTexParameteri( formatType, GL_TEXTURE_MIN_FILTER, TextureAttributeHandle::FilterTypeTable[ m_AttributeParam.m_TextureAttribute[texNo].m_MinFilter ] );
	//glTexParameteri( formatType, GL_TEXTURE_MAG_FILTER, TextureAttributeHandle::FilterTypeTable[ m_AttributeParam.m_TextureAttribute[texNo].m_MagFilter ] );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetWrapS() );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetWrapT() );
	//glTexParameterfv( formatType, GL_TEXTURE_BORDER_COLOR, &m_AttributeParam.m_EtcColor.m_TextureBorderColor[texNo][0] );
}

#ifdef MAYA_INPORT

using namespace gfl::maya;
using namespace gfl::maya::plugins;
using namespace gfl::maya::plugins::Node;


//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------

class gfl::cmdl::Material::impl
{
public:
	impl() :
		m_pShaderObject(NULL),
		m_pVertexShader(NULL),
		m_pGeometryShader(NULL),
		m_pPixelShader(NULL)
	{
		m_AttributeParam.Clear();
		m_fLightEnable = true;
	}

	~impl()
	{
		SAFE_DELETE( m_pShaderObject );
		SAFE_DELETE( m_pVertexShader );
		SAFE_DELETE( m_pGeometryShader );
		SAFE_DELETE( m_pPixelShader );
	}

	GLInterface::ShaderObject								*m_pShaderObject;
	GLInterface::IShader										*m_pVertexShader;
	GLInterface::IShader										*m_pGeometryShader;
	GLInterface::IShader										*m_pPixelShader;
	CombinerAttribute::AttributeParam				m_AttributeParam;

	GLint																			m_hVsModelViewMatrix;
	GLint																			m_hVsProjectionMatrix;
	GLint																			m_hVsDrawUvSpaceNo;
	GLint																			m_hVsModelMatrix;
	GLint																			m_hVsRimParam;
	GLint																			m_hVsWorldLightVec;
	GLint																			m_hVsWorldLightCol;
	GLint																			m_hVsAmbientLightColor;
	GLint																			m_hVsJointMatrix;
	GLint																			m_hVsNormalMapEnable;
  GLint																			m_hVsCameraPosition;
  GLint																			m_hVsHalfVecLightEnable;
	GLint																			m_hVsVertexColorEnable;
	GLint																			m_hVsObjSpaceCameraPosition;	
	GLint																			m_hVsCommonParam;

	static const int										LightMax = 8;

	GLint																			m_hPsObjSpaceLightCount;
	GLint																			m_hPsAmbientLightColor;
	GLint																			m_hPsObjSpaceCameraPosition;	
  GLint																			m_hPsCookTorranceParam;
	GLint																			m_hPsObjSpaceLightPos[LightMax];
	GLint																			m_hPsObjSpaceLightColor[LightMax];
	GLint																			m_hPsObjSpaceLightVec[LightMax];
	GLint																			m_hPsObjSpaceLightConeAngle[LightMax];
	GLint																			m_hPsIsSpotLight[LightMax];
	GLint																			m_hPsDrawUvSpaceNo;
	GLint																			m_hPsDrawUvDepthTest;

	GLint																			m_hGsShaderParam;
	GLint																			m_hGsDrawUvSpace;
	
	GLint																			m_hInputSource[CombinerColorType::NumberOf][CombinerAttribute::CombinerMax];
	float																			m_FarLength;

	bool																			m_fLightEnable;
	GLuint																		m_hWhiteTable;
	GLuint																		m_hBlackTable;
};

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void gfl::cmdl::Material::glCreateShader()
{
	struct LocalFunc{

		//------------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------------
		static plugins::Node::CombinerAttribute::AttributeParam GetAttributeParam( const Material &rMaterial )
		{
			plugins::Node::CombinerAttribute::AttributeParam		m_AttributeParam;

			m_AttributeParam.Clear();

			m_AttributeParam.m_IsPokemonShader = false;
			m_AttributeParam.m_VertexShaderType = gfl::maya::plugins::Node::VertexShaderType::Default;
			m_AttributeParam.m_UseObjectSpaceNormalMap = false;
			m_AttributeParam.m_UseShadowTexture = false;//加工澄
			m_AttributeParam.m_BumpMapNo = -1;

			{
				const Material::Rasterization								&rRasterization = rMaterial.m_Rasterization;

				m_AttributeParam.m_PsLightingEnable = rMaterial.m_IsFragmentLightEnabled;
				m_AttributeParam.m_VsLightingEnable = rMaterial.m_IsVertexLightEnabled;
				m_AttributeParam.m_LightSetNo = 0;
				m_AttributeParam.m_FogEnable = false;
				m_AttributeParam.m_FogNo = 0;
				m_AttributeParam.m_CullMode = GetCullMode( rRasterization.m_CullingMode );
			}

			{
				Material::MaterialColor			*pMaterialColor = rMaterial.GetMaterialColor();

				{
					gfl::core::Vector4					rColor;

					rColor = pMaterialColor->GetEmission();
					m_AttributeParam.m_MaterialColor.m_Emission.Set( rColor.GFL_VECTOR_X, rColor.GFL_VECTOR_Y, rColor.GFL_VECTOR_Z, rColor.GFL_VECTOR_W );

					rColor = pMaterialColor->GetAmbient();
					m_AttributeParam.m_MaterialColor.m_Ambient.Set( rColor.GFL_VECTOR_X, rColor.GFL_VECTOR_Y, rColor.GFL_VECTOR_Z, rColor.GFL_VECTOR_W );

					rColor = pMaterialColor->GetDiffuse();
					m_AttributeParam.m_MaterialColor.m_Diffuse.Set( rColor.GFL_VECTOR_X, rColor.GFL_VECTOR_Y, rColor.GFL_VECTOR_Z, rColor.GFL_VECTOR_W );

					rColor = pMaterialColor->GetSpecular( 0 );
					m_AttributeParam.m_MaterialColor.m_Specular0.Set( rColor.GFL_VECTOR_X, rColor.GFL_VECTOR_Y, rColor.GFL_VECTOR_Z, rColor.GFL_VECTOR_W );

					rColor = pMaterialColor->GetSpecular( 1 );
					m_AttributeParam.m_MaterialColor.m_Specular1.Set( rColor.GFL_VECTOR_X, rColor.GFL_VECTOR_Y, rColor.GFL_VECTOR_Z, rColor.GFL_VECTOR_W );
				}

				for( int combNo = 0; combNo < CombinerAttribute::CombinerMax; ++combNo )
				{	
					const gfl::core::Vector4					&rColor = pMaterialColor->GetConstant( combNo );
					m_AttributeParam.m_ConsntantColor[combNo].Set( rColor.GFL_VECTOR_X, rColor.GFL_VECTOR_Y, rColor.GFL_VECTOR_Z, rColor.GFL_VECTOR_W );
				}
			}

			{
				const Material::Shader											*pShader = rMaterial.GetShader();
				const Material::Shader::FragmentShader			&rFragmentShader = pShader->m_FragmentShader;
				const Material::Shader::FragmentOperation		&rFragmentOperation = pShader->m_FragmentOperation;
				gfl::core::Vector4													Color;

				Color = rFragmentOperation.m_BlendOperation.m_BlendColor;
				m_AttributeParam.m_EtcColor.m_BlendColor.Set( Color.GFL_VECTOR_X, Color.GFL_VECTOR_Y, Color.GFL_VECTOR_Z, Color.GFL_VECTOR_W );

				Color = rFragmentShader.m_BufferColor;
				m_AttributeParam.m_EtcColor.m_BufferColor.Set( Color.GFL_VECTOR_X, Color.GFL_VECTOR_Y, Color.GFL_VECTOR_Z, Color.GFL_VECTOR_W );

				for( int texNo = 0; texNo < CombinerAttribute::TextureMax; ++texNo )
				{
					Material::TextureMapper		*pTextureMapper = &rMaterial.GetTextureMapper()[texNo];

					Color = pTextureMapper->m_BorderColor;
					m_AttributeParam.m_EtcColor.m_TextureBorderColor[texNo].Set( Color.GFL_VECTOR_X, Color.GFL_VECTOR_Y, Color.GFL_VECTOR_Z, Color.GFL_VECTOR_W );
				}
			}

#if 0
			{//未解析
				m_AttributeParam.m_BumpMapNo;
				m_AttributeParam.m_BumpMapType;
				m_AttributeParam.m_ZRecastEnable;
			}			
#endif

			for( int texNo = 0; texNo < rMaterial.GetTextureCoordinatorCount(); ++texNo )
			{
				Material::TextureCoordinator		*pTextureCoordinator = &rMaterial.GetTextureCoordinator()[texNo];

				m_AttributeParam.m_TextureAttribute[texNo].m_MappingType = 0;//UvMap
				m_AttributeParam.m_TextureAttribute[texNo].m_UvSetNo = pTextureCoordinator->GetSourceCoordinate();

				//展開した状態のテクスチャを用いるのでデフォルト設定
				m_AttributeParam.m_TextureAttribute[texNo].m_ScaleU = 1.0f;
				m_AttributeParam.m_TextureAttribute[texNo].m_ScaleV = 1.0f;
				m_AttributeParam.m_TextureAttribute[texNo].m_Rotate = 0.0f;
				m_AttributeParam.m_TextureAttribute[texNo].m_TranslateU = 0.0f;
				m_AttributeParam.m_TextureAttribute[texNo].m_TranslateV = 0.0f;

				//設定不要
				//m_AttributeParam.m_TextureAttribute[texNo].m_RepeatU;
				//m_AttributeParam.m_TextureAttribute[texNo].m_RepeatV;
				//m_AttributeParam.m_TextureAttribute[texNo].m_MagFilter;
				//m_AttributeParam.m_TextureAttribute[texNo].m_MinFilter;
			}

			{
				const Material::Shader											*pShader = rMaterial.GetShader();
//				const Material::Shader::FragmentShader			&rFragmentShader = pShader->m_FragmentShader;
				const Material::Shader::FragmentShader::FragmentLightingTable::ReflectanceSampler *tables[] = {
					&pShader->m_FragmentShader.m_FragmentLightingTable.m_Distribution0Sampler,
					&pShader->m_FragmentShader.m_FragmentLightingTable.m_Distribution1Sampler,
					&pShader->m_FragmentShader.m_FragmentLightingTable.m_ReflectanceRSampler,
					&pShader->m_FragmentShader.m_FragmentLightingTable.m_ReflectanceGSampler,
					&pShader->m_FragmentShader.m_FragmentLightingTable.m_ReflectanceBSampler,
					&pShader->m_FragmentShader.m_FragmentLightingTable.m_FresnelSampler
				};

				for( int type = 0; type < LookUpTableType::NumberOf; ++type )
				{
					const Material::Shader::FragmentShader::FragmentLightingTable::ReferenceLookupTable				*pReferenceLookupTable = tables[type]->m_pReferenceLookupTable;

					if ( pReferenceLookupTable )
					{
						m_AttributeParam.m_LookUpTable[type].m_LutTableName = *pReferenceLookupTable->m_TableName;
					}
					
					m_AttributeParam.m_LookUpTable[type].m_LutInputAngle = GetLutInputAngleType( tables[type]->m_Input );
					m_AttributeParam.m_LookUpTable[type].m_LutScale = GetLutScaleType( tables[type]->m_Scale );
				}
			}

			{
				const Material::Shader											*pShader = rMaterial.GetShader();
				const Material::Shader::UserData						*pUserData = pShader->m_UserData[ "VertexShaderType" ];

				if ( pUserData )
				{
					int			value = pUserData->m_Value.toInt();

					if ( value = Node::VertexShaderType::PokeNormal )
					{
						m_AttributeParam.m_LookUpTable[LookUpTableType::ReflectionR].m_LutInputAngle = LutInputAngleType::LN;
						m_AttributeParam.m_LookUpTable[LookUpTableType::ReflectionG].m_LutInputAngle = LutInputAngleType::LN;
						m_AttributeParam.m_LookUpTable[LookUpTableType::ReflectionB].m_LutInputAngle = LutInputAngleType::LN;
					}
				}
			}

			{
				const Material::Shader											*pShader = rMaterial.GetShader();
				const Material::Shader::FragmentShader			&rFragmentShader = pShader->m_FragmentShader;

				m_AttributeParam.m_PrimaryFresnelEnable = IsPrimaryFresnelEnable( rFragmentShader.m_FragmentLighting.m_FresnelConfig );
				m_AttributeParam.m_SecondaryFresnelEnable = IsSecondaryFresnelEnable( rFragmentShader.m_FragmentLighting.m_FresnelConfig );
				m_AttributeParam.m_SpecularColorClamp = rFragmentShader.m_FragmentLighting.m_IsClampHighLight;
				m_AttributeParam.m_CookTorrance0Enable = rFragmentShader.m_FragmentLighting.m_IsGeometricFactor0Enabled;
				m_AttributeParam.m_CookTorrance1Enable = rFragmentShader.m_FragmentLighting.m_IsGeometricFactor1Enabled;
			}

			{
				const Material::Shader											*pShader = rMaterial.GetShader();
				const Material::Shader::FragmentShader			&rFragmentShader = pShader->m_FragmentShader;

				
				for( int i = 0; i < rFragmentShader.m_TextureConbiner.size(); ++i )
				{
					const Material::Shader::FragmentShader::TextureCombiner				*pTextureCombiner = rFragmentShader.m_TextureConbiner.at( i );
					const Material::Shader::FragmentShader::TextureCombiner				*pNextTextureCombiner = NULL;

					if ( (i+1) < rFragmentShader.m_TextureConbiner.size() )
					{
						pNextTextureCombiner = rFragmentShader.m_TextureConbiner.at( i+1 );
					}

					{
						int			type = CombinerColorType::RGB;

						m_AttributeParam.m_TextureCombinerAttribute[type][i].m_MathFormula = GetMathFormulaType( pTextureCombiner->m_CombineRgb );
						m_AttributeParam.m_TextureCombinerAttribute[type][i].m_Scale = GetScaleType( pTextureCombiner->m_ScaleRgb );

						QString			SourceStr[] = { pTextureCombiner->m_SourceRgb.m_Source0, pTextureCombiner->m_SourceRgb.m_Source1, pTextureCombiner->m_SourceRgb.m_Source2 };
						QString			OperandStr[] = { pTextureCombiner->m_OperandRgb.m_Operand0, pTextureCombiner->m_OperandRgb.m_Operand1, pTextureCombiner->m_OperandRgb.m_Operand2 };

						for( int i2 = 0; i2 < InputSourceType::NumberOf; ++i2 )
						{
							m_AttributeParam.m_TextureCombinerAttribute[type][i].m_InputSource[i2] = GetCombInputSourceType( SourceStr[i2] );
							m_AttributeParam.m_TextureCombinerAttribute[type][i].m_Operand[i2] = GetOperandType( OperandStr[i2] );
						}

						if ( pNextTextureCombiner )
							m_AttributeParam.m_TextureCombinerAttribute[type][i].m_OutPutBuffer = IsOutPutBuffer( pNextTextureCombiner->m_BufferInputRgb );
						else
							m_AttributeParam.m_TextureCombinerAttribute[type][i].m_OutPutBuffer = false;
					}

					{
						int			type = CombinerColorType::A;

						m_AttributeParam.m_TextureCombinerAttribute[type][i].m_MathFormula = GetMathFormulaType( pTextureCombiner->m_CombineRgb );
						m_AttributeParam.m_TextureCombinerAttribute[type][i].m_Scale = GetScaleType( pTextureCombiner->m_ScaleAlpha );

						QString			SourceStr[] = { pTextureCombiner->m_SourceAlpha.m_Source0, pTextureCombiner->m_SourceAlpha.m_Source1, pTextureCombiner->m_SourceAlpha.m_Source2 };
						QString			OperandStr[] = { pTextureCombiner->m_OperandAlpha.m_Operand0, pTextureCombiner->m_OperandAlpha.m_Operand1, pTextureCombiner->m_OperandAlpha.m_Operand2 };

						for( int i2 = 0; i2 < InputSourceType::NumberOf; ++i2 )
						{
							m_AttributeParam.m_TextureCombinerAttribute[type][i].m_InputSource[i2] = GetCombInputSourceType( SourceStr[i2] );
							m_AttributeParam.m_TextureCombinerAttribute[type][i].m_Operand[i2] = GetOperandType( OperandStr[i2] );
						}

						if ( pNextTextureCombiner )
							m_AttributeParam.m_TextureCombinerAttribute[type][i].m_OutPutBuffer = IsOutPutBuffer( pNextTextureCombiner->m_BufferInputAlpha );
						else
							m_AttributeParam.m_TextureCombinerAttribute[type][i].m_OutPutBuffer = false;
					}

					m_AttributeParam.m_TextureCombinerConstant[i] = GetConstantType( pTextureCombiner->m_Constant );
				}
			}

			{
				const Material::Shader											*pShader = rMaterial.GetShader();
				const Material::Shader::FragmentShader			&rFragmentShader = pShader->m_FragmentShader;
				const Material::Shader::FragmentOperation		&rFragmentOperation = pShader->m_FragmentOperation;
				const Material::Rasterization								&rRasterization = rMaterial.m_Rasterization;

				m_AttributeParam.m_DrawLayer = GetDrawLayer( rFragmentShader.m_LayerConfig );
				m_AttributeParam.m_AlphaTestType = GetAlphaTestType( rFragmentShader.m_AlphaTest.m_TestFunction );
				m_AttributeParam.m_AlphaTestRefValue = rFragmentShader.m_AlphaTest.m_TestReference;

				m_AttributeParam.m_DepthTestType = GetDepthTestType( rFragmentOperation.m_DepthOperation.m_TestFunction );
				m_AttributeParam.m_UpDateDepth = rFragmentOperation.m_DepthOperation.m_IsMaskEnabled;
				m_AttributeParam.m_PolygonOffSet = rRasterization.m_PolygonOffsetUnit;

				{
					m_AttributeParam.m_StencilTestEnable = rFragmentOperation.m_StencilOperation.m_IsTestEnabled;
					m_AttributeParam.m_StencilTestType = GetStencilTestType( rFragmentOperation.m_StencilOperation.m_TestFunction );
					m_AttributeParam.m_StencilValue = rFragmentOperation.m_StencilOperation.m_TestReference;
					m_AttributeParam.m_StencilMask = rFragmentOperation.m_StencilOperation.m_TestMask;
					m_AttributeParam.m_StencilFailOp = GetStencilOpType( rFragmentOperation.m_StencilOperation.m_FailOperation );
					m_AttributeParam.m_StencilZFailOp = GetStencilOpType( rFragmentOperation.m_StencilOperation.m_ZFailOperation );
					m_AttributeParam.m_StencilZpassOp = GetStencilOpType( rFragmentOperation.m_StencilOperation.m_PassOperation );
				}

				m_AttributeParam.m_BlendMode = GetBlendModeType( rFragmentOperation.m_BlendOperation.m_Mode );

				{
					m_AttributeParam.m_BlendFunc[CombinerColorType::RGB].m_BlendFunc = GetBlendEquationType( rFragmentOperation.m_BlendOperation.m_RgbParameter.m_BlendEquation );
					m_AttributeParam.m_BlendFunc[CombinerColorType::RGB].m_SrcElement = GetBlendFuncType( rFragmentOperation.m_BlendOperation.m_RgbParameter.m_BlendFunctionSource );
					m_AttributeParam.m_BlendFunc[CombinerColorType::RGB].m_DstElement = GetBlendFuncType( rFragmentOperation.m_BlendOperation.m_RgbParameter.m_BlendFunctionDestination );

					m_AttributeParam.m_BlendFunc[CombinerColorType::A].m_BlendFunc = GetBlendEquationType( rFragmentOperation.m_BlendOperation.m_AlphaParameter.m_BlendEquation );
					m_AttributeParam.m_BlendFunc[CombinerColorType::A].m_SrcElement = GetBlendFuncType( rFragmentOperation.m_BlendOperation.m_AlphaParameter.m_BlendFunctionSource );
					m_AttributeParam.m_BlendFunc[CombinerColorType::A].m_DstElement = GetBlendFuncType( rFragmentOperation.m_BlendOperation.m_AlphaParameter.m_BlendFunctionDestination );
				}

				m_AttributeParam.m_LogicalOperation = GetLogicOpType( rFragmentOperation.m_BlendOperation.m_LogicOperation );
			}

#if 0
			for( int type = 0; type < LookUpTableType::NumberOf; ++type )
			{//最後に、bindTextureしないとわからないフラグをここで調べてあげる。
				m_AttributeParam.m_LookUpTableEnable[type];
			}
#endif

			return m_AttributeParam;
		}

		//------------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------------
		static GLint getParameterByName( GLInterface::ShaderObject *pShaderObject, const char* message, ... )
		{
			static const int			CharMax = 1024;

			char tmp[CharMax];

			va_list list;

			va_start( list, message );
			vsprintf( tmp, message, list );
			va_end( list );

			return pShaderObject->GetParameterByName( tmp );
		}

		//------------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------------
		static void SetShaderParamHadle( gfl::cmdl::Material::impl *pImpl, GLInterface::ShaderObject *m_pShaderObject )
		{
			m_pShaderObject->SetShader();

			pImpl->m_hVsModelViewMatrix = m_pShaderObject->GetParameterByName("cVsModelViewMatrix");
			pImpl->m_hVsProjectionMatrix = m_pShaderObject->GetParameterByName("cVsProjectionMatrix");
			pImpl->m_hVsDrawUvSpaceNo = m_pShaderObject->GetParameterByName("cVsDrawUvSpaceNo");

			pImpl->m_hGsDrawUvSpace = m_pShaderObject->GetParameterByName("cGsDrawUvSpace");
			
			pImpl->m_hVsModelMatrix = m_pShaderObject->GetParameterByName("cVsModelMatrix");
			pImpl->m_hVsRimParam = m_pShaderObject->GetParameterByName("cVsRimParam");
			pImpl->m_hVsWorldLightVec = m_pShaderObject->GetParameterByName("cVsWorldLightVec");
			pImpl->m_hVsWorldLightCol = m_pShaderObject->GetParameterByName("cVsWorldLightColor");
			pImpl->m_hVsAmbientLightColor = m_pShaderObject->GetParameterByName("cVsAmbientLightColor");

			pImpl->m_hVsJointMatrix = m_pShaderObject->GetParameterByName("cVsJointMatrix");
			pImpl->m_hVsNormalMapEnable = m_pShaderObject->GetParameterByName("cVsNormalMapEnable");
			pImpl->m_hVsCameraPosition = m_pShaderObject->GetParameterByName("cVsCameraPosition");  
			pImpl->m_hVsHalfVecLightEnable = m_pShaderObject->GetParameterByName("cVsHalfVecLightEnable");
			pImpl->m_hVsVertexColorEnable = m_pShaderObject->GetParameterByName("cVsVertexColorEnable"); 
			pImpl->m_hVsObjSpaceCameraPosition = m_pShaderObject->GetParameterByName("cVsObjSpaceCameraPosition");
			pImpl->m_hVsCommonParam = m_pShaderObject->GetParameterByName("cVsCommonParam");

			pImpl->m_hPsObjSpaceLightCount = m_pShaderObject->GetParameterByName("cPsObjSpaceLightCount");
			pImpl->m_hPsAmbientLightColor = m_pShaderObject->GetParameterByName("cPsAmbientLightColor");
			pImpl->m_hPsObjSpaceCameraPosition = m_pShaderObject->GetParameterByName("cPsObjSpaceCameraPosition");
			pImpl->m_hPsCookTorranceParam = m_pShaderObject->GetParameterByName("cPsCookTorranceParam");
			pImpl->m_hPsDrawUvSpaceNo = m_pShaderObject->GetParameterByName("cPsDrawUvSpaceNo");
			pImpl->m_hPsDrawUvDepthTest = m_pShaderObject->GetParameterByName("cPsDrawUvDepthTest");			

			pImpl->m_hGsShaderParam = m_pShaderObject->GetParameterByName("cGsShaderParam");

			for( int lightNo = 0; lightNo < gfl::cmdl::Material::impl::LightMax; ++lightNo )
			{
				pImpl->m_hPsObjSpaceLightPos[lightNo] = getParameterByName( m_pShaderObject, "cPsObjSpaceLightPos[%d]", lightNo );
				pImpl->m_hPsObjSpaceLightColor[lightNo] = getParameterByName( m_pShaderObject, "cPsObjSpaceLightColor[%d]", lightNo );
				pImpl->m_hPsObjSpaceLightVec[lightNo] = getParameterByName( m_pShaderObject, "cPsObjSpaceLightVec[%d]", lightNo );
				pImpl->m_hPsObjSpaceLightConeAngle[lightNo] = getParameterByName( m_pShaderObject, "cPsObjSpaceLightConeAngle[%d]", lightNo );
				pImpl->m_hPsIsSpotLight[lightNo] = getParameterByName( m_pShaderObject, "cPsIsSpotLight[%d]", lightNo );
			}

			glUseProgram(0);//一旦外す	
		}


		//------------------------------------------------------------------------------
		// 
		//------------------------------------------------------------------------------
		static void SetAttributeParamHandle( CombinerAttribute::AttributeParam	&m_AttributeParam,
																				 GLInterface::ShaderObject					*m_pShaderObject )
		{
			for( int i = 0; i < LookUpTableType::NumberOf; ++i )
			{
				m_AttributeParam.m_hLookUpTableEnable[i] = getParameterByName( m_pShaderObject, "cLookUpTableEnable[%d]", i );
			}

			m_AttributeParam.m_hVsLightingEnable = getParameterByName( m_pShaderObject, "cVsLightingEnable" );
			
			m_AttributeParam.m_hCombinerStageMax = getParameterByName( m_pShaderObject, "cCombinerStageMax" );

			{
				m_AttributeParam.m_MaterialColor.m_hEmission = getParameterByName( m_pShaderObject, "cEmission" );
				m_AttributeParam.m_MaterialColor.m_hAmbient = getParameterByName( m_pShaderObject, "cAmbient" );
				m_AttributeParam.m_MaterialColor.m_hDiffuse = getParameterByName( m_pShaderObject, "cDiffuse" );
				m_AttributeParam.m_MaterialColor.m_hSpecular0 = getParameterByName( m_pShaderObject, "cSpecular0" );
				m_AttributeParam.m_MaterialColor.m_hSpecular1 = getParameterByName( m_pShaderObject, "cSpecular1" );
			}

			for( int combNo = 0; combNo < CombinerAttribute::CombinerMax; ++combNo )
			{
				m_AttributeParam.m_hConsntantColor[combNo] = getParameterByName( m_pShaderObject, "cConsntantColor[%d]", combNo );
			}

			{
				m_AttributeParam.m_EtcColor.m_hBufferColor = getParameterByName( m_pShaderObject, "cBufferColor" );
			}

			m_AttributeParam.m_hBumpMapNo = getParameterByName( m_pShaderObject, "cBumpMapNo" );
			m_AttributeParam.m_hBumpMapType = getParameterByName( m_pShaderObject, "cBumpMapType" );
			m_AttributeParam.m_hZRecastEnable = getParameterByName( m_pShaderObject, "cZRecastEnable" );
			m_AttributeParam.m_hTangentNo = getParameterByName( m_pShaderObject, "cTangentNo" );

			for( int texNo = 0; texNo < CombinerAttribute::TextureMax; ++texNo )
			{
				m_AttributeParam.m_hTextureAttributeEnable[texNo] = getParameterByName( m_pShaderObject, "cTextureEnable[%d]", texNo );
				m_AttributeParam.m_hIsTexture2D[texNo] = getParameterByName( m_pShaderObject, "cTexture2D[%d]", texNo );

				m_AttributeParam.m_TextureAttribute[texNo].m_hMappingType = getParameterByName( m_pShaderObject, "cMappingType[%d]", texNo );
				m_AttributeParam.m_TextureAttribute[texNo].m_hUvSetNo = getParameterByName( m_pShaderObject, "cUvSetNo[%d]", texNo );
				m_AttributeParam.m_TextureAttribute[texNo].m_hScaleU = getParameterByName( m_pShaderObject, "cScaleU[%d]", texNo );
				m_AttributeParam.m_TextureAttribute[texNo].m_hScaleV = getParameterByName( m_pShaderObject, "cScaleV[%d]", texNo );
				m_AttributeParam.m_TextureAttribute[texNo].m_hRotate = getParameterByName( m_pShaderObject, "cRotate[%d]", texNo );
				m_AttributeParam.m_TextureAttribute[texNo].m_hTranslateU = getParameterByName( m_pShaderObject, "cTranslateU[%d]", texNo );
				m_AttributeParam.m_TextureAttribute[texNo].m_hTranslateV = getParameterByName( m_pShaderObject, "cTranslateV[%d]", texNo );
			}

			for( int type = 0; type < LookUpTableType::NumberOf; ++type )
			{
				m_AttributeParam.m_LookUpTable[type].m_hLutInputAngle = getParameterByName( m_pShaderObject, "cLutInputAngle[%d]", type );
				m_AttributeParam.m_LookUpTable[type].m_hLutScale = getParameterByName( m_pShaderObject, "cLutScale[%d]", type );
			}

			m_AttributeParam.m_hPrimaryFresnelEnable = getParameterByName( m_pShaderObject, "cPrimaryFresnelEnable" );
			m_AttributeParam.m_hSecondaryFresnelEnable = getParameterByName( m_pShaderObject, "cSecondaryFresnelEnable" );
			m_AttributeParam.m_hSpecularColorClamp = getParameterByName( m_pShaderObject, "cSpecularColorClamp" );
			m_AttributeParam.m_hCookTorrance0Enable = getParameterByName( m_pShaderObject, "cCookTorrance0Enable" );
			m_AttributeParam.m_hCookTorrance1Enable = getParameterByName( m_pShaderObject, "cCookTorrance1Enable" );

			for( int i = 0; i < CombinerAttribute::CombinerMax; ++i )
			{
				for( int type = 0; type < CombinerColorType::NumberOf; ++type )
				{
					m_AttributeParam.m_TextureCombinerAttribute[type][i].m_hMathFormula = getParameterByName( m_pShaderObject, "cMathFormula[%d][%d]", type, i );
					m_AttributeParam.m_TextureCombinerAttribute[type][i].m_hScale = getParameterByName( m_pShaderObject, "cScale[%d][%d]", type, i );

					for( int i2 = 0; i2 < InputSourceType::NumberOf; ++i2 )
					{
						m_AttributeParam.m_TextureCombinerAttribute[type][i].m_hInputSource[i2] = getParameterByName( m_pShaderObject, "cInputSource[%d][%d][%d]", type, i, i2 );
						m_AttributeParam.m_TextureCombinerAttribute[type][i].m_hOperand[i2] = getParameterByName( m_pShaderObject, "cSourceOperand[%d][%d][%d]", type, i, i2 );
					}

					m_AttributeParam.m_TextureCombinerAttribute[type][i].m_hOutPutBuffer = getParameterByName( m_pShaderObject, "cOutPutBuffer[%d][%d]", type, i );
				}

				m_AttributeParam.m_hTextureCombinerConstant[i] = getParameterByName( m_pShaderObject, "cTextureCombinerConstant[%d]", i );
			}

			m_AttributeParam.m_hIsPokemonShader = getParameterByName( m_pShaderObject, "cIsPokemonShader" );
			m_AttributeParam.m_hFsIsPokemonShader = getParameterByName( m_pShaderObject, "cFsIsPokemonShader" );
			m_AttributeParam.m_hUseShadowTexture = getParameterByName( m_pShaderObject, "cUseShadowTexture" );
			m_AttributeParam.m_hUseObjectSpaceNormalMap = getParameterByName( m_pShaderObject, "cUseObjectSpaceNormalMap" );
			m_AttributeParam.m_hHigLightTextureEnable = getParameterByName( m_pShaderObject, "cHigLightTextureEnable" );
			m_AttributeParam.m_hVsRimParam = getParameterByName( m_pShaderObject, "cVsRimParam" );

			m_AttributeParam.m_hVsSceneContrastEnable = getParameterByName( m_pShaderObject, "cVsSceneContrastEnable" );
			m_AttributeParam.m_hVsSceneContrast = getParameterByName( m_pShaderObject, "cVsSceneContrast" );
			m_AttributeParam.m_hVsZoneContrast = getParameterByName( m_pShaderObject, "cVsZoneContrast" );
		}
	};

	
	plugins::Node::CombinerAttribute::AttributeParam			param = LocalFunc::GetAttributeParam( *this );

	for( int type = 0; type < ShaderType::NumberOf; ++type )
	{
		SAFE_DELETE( m_pImpl[type] );

		m_pImpl[type] = DbgNew impl();
		m_pImpl[type]->m_AttributeParam = param;

		if ( type == ShaderType::SimpleIDRender )
		{
			m_pImpl[type]->m_AttributeParam.InitTextureCombinerAttribute();
		}

		m_pImpl[type]->m_pVertexShader = DbgNew plugins::Shader::UvAdjustVertexShader( plugins::Shader::Connection::GeometryShader );
		m_pImpl[type]->m_pGeometryShader = DbgNew plugins::Shader::UvAdjustGeometryShader();
		m_pImpl[type]->m_pPixelShader = DbgNew plugins::Shader::UvAdjustPixelShader( &m_pImpl[type]->m_AttributeParam );

		m_pImpl[type]->m_pShaderObject = GLInterface::ShaderObject::CreateShaderObject(
			m_pImpl[type]->m_pVertexShader,
			m_pImpl[type]->m_pGeometryShader,
			m_pImpl[type]->m_pPixelShader );

		LocalFunc::SetShaderParamHadle( m_pImpl[type], m_pImpl[type]->m_pShaderObject );
		LocalFunc::SetAttributeParamHandle( m_pImpl[type]->m_AttributeParam, m_pImpl[type]->m_pShaderObject );

		{
			static u32			imageData[256];

			memset( &imageData, 0xFFFFFFFF, sizeof(imageData) );

			glGenTextures( 1, &m_pImpl[type]->m_hWhiteTable );
			glBindTexture( GL_TEXTURE_2D, m_pImpl[type]->m_hWhiteTable );
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, 256, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData );
			glBindTexture( GL_TEXTURE_2D, 0 );//リセット
		}

		{
			static u32			imageData[256];

			memset( &imageData, 0, sizeof(imageData) );

			glGenTextures( 1, &m_pImpl[type]->m_hBlackTable );
			glBindTexture( GL_TEXTURE_2D, m_pImpl[type]->m_hBlackTable );
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, 256, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData );
			glBindTexture( GL_TEXTURE_2D, 0 );//リセット
		}
	}

	m_DraswUvSpaceNo = -1;
	m_DrawUvDepthTest = true;
	m_ShaderType = ShaderType::ModelRender;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
#ifdef MAYA_INPORT
//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void gfl::cmdl::Material::SetLightEnable( bool f )
{
	for( int type = 0; type < ShaderType::NumberOf; ++type )
		m_pImpl[type]->m_fLightEnable = f;
}

void gfl::cmdl::Material::glBind( ShaderType::Enum shaderType, QList<LookupTable*> &rLookupTables )
{
	using namespace gfl::maya;
	using namespace gfl::maya::plugins;
	using namespace gfl::maya::plugins::Node;

	glBindShader( shaderType );

	gfl::cmdl::Material::MaterialColor* mc = this->GetMaterialColor();
	mc->SetLight();

	Material::TextureMapper					*pTextureMapper = this->GetTextureMapper();
	Material::TextureCoordinator		*pTextureCoordinator = this->GetTextureCoordinator();

	CombinerAttribute::AttributeParam				&m_AttributeParam = m_pImpl[shaderType]->m_AttributeParam;

	for( int i = 0; i < GetTextureMapperCoordCount(); ++i )
	{
		glUniform1i( m_AttributeParam.m_hTextureAttributeEnable[i], true );
		glUniform1i( m_AttributeParam.m_hIsTexture2D[i], true );

		glActiveTexture( GL_TEXTURE0 + i );
		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, *(pTextureMapper[i].GetTextureId()) );
		pTextureMapper[i].SetTextureEnv();
	}

	for( int i = GetTextureMapperCoordCount(); i < 3; ++i )
	{
		glUniform1i( m_AttributeParam.m_hTextureAttributeEnable[i], false );
	}

	for( int type = 0; type < LookUpTableType::NumberOf; ++type )
	{
		QString						TableName = m_AttributeParam.m_LookUpTable[type].m_LutTableName;
		bool							fEnable = false;

		{
			for( int i = 0; i < rLookupTables.size(); ++i )
			{
				QString				srcName = rLookupTables.at(i)->m_Name;

				if ( rLookupTables.at(i)->m_Name == TableName )
				{
					fEnable = true;

					glActiveTexture( GL_TEXTURE0 + (CombinerAttribute::TextureMax*2) + type );
					glEnable( GL_TEXTURE_2D );

					if ( m_pImpl[shaderType]->m_fLightEnable )
					{
						glBindTexture( GL_TEXTURE_2D, rLookupTables.at(i)->m_hLutTexture );
					}
					else
					{
						if( type == LookUpTableType::ReflectionR )
						{
							glBindTexture( GL_TEXTURE_2D, m_pImpl[shaderType]->m_hWhiteTable );
						}
						else if( type == LookUpTableType::ReflectionG )
						{
							glBindTexture( GL_TEXTURE_2D, m_pImpl[shaderType]->m_hBlackTable );
						}
						else
						{
							glBindTexture( GL_TEXTURE_2D, m_pImpl[shaderType]->m_hWhiteTable );
							//glBindTexture( GL_TEXTURE_2D, rLookupTables.at(i)->m_hLutTexture );
						}
					}
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				}
			}
		}

		glUniform1i( m_AttributeParam.m_hLookUpTableEnable[type], fEnable );
	}
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void gfl::cmdl::Material::SetCamera( gfl::core::Camera* pCamera )
{
	if ( pCamera )
	{
		for( int type = 0; type < ShaderType::NumberOf; ++type )
			m_pImpl[type]->m_FarLength = pCamera->_FarClip;
	}
}

#endif

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void gfl::cmdl::Material::glBindShader( ShaderType::Enum shaderType )
{
	struct LocalFunc{

		//-----------------------------------------------------------------------------
		/// @brief	
		//-----------------------------------------------------------------------------
		static void setAttributeParam( GLInterface::ShaderObject *m_pShaderObject, CombinerAttribute::AttributeParam &m_AttributeParam )
		{
			glUniform1i( m_AttributeParam.m_hVsLightingEnable, m_AttributeParam.m_VsLightingEnable );
			
			{
				switch( m_AttributeParam.m_CullMode ){
				case GL_NONE:
					glDisable( GL_CULL_FACE );
					break;
				default:
					glEnable( GL_CULL_FACE );
					glCullFace( m_AttributeParam.m_CullMode );
					break;
				}
			}

			{
				m_pShaderObject->SetVector( m_AttributeParam.m_MaterialColor.m_hEmission, m_AttributeParam.m_MaterialColor.m_Emission );
				m_pShaderObject->SetVector( m_AttributeParam.m_MaterialColor.m_hAmbient, m_AttributeParam.m_MaterialColor.m_Ambient );
				m_pShaderObject->SetVector( m_AttributeParam.m_MaterialColor.m_hDiffuse, m_AttributeParam.m_MaterialColor.m_Diffuse );
				m_pShaderObject->SetVector( m_AttributeParam.m_MaterialColor.m_hSpecular0, m_AttributeParam.m_MaterialColor.m_Specular0 );
				m_pShaderObject->SetVector( m_AttributeParam.m_MaterialColor.m_hSpecular1, m_AttributeParam.m_MaterialColor.m_Specular1 );
			}

			for( int combNo = 0; combNo < CombinerAttribute::CombinerMax; ++combNo )
			{
				m_pShaderObject->SetVector( m_AttributeParam.m_hConsntantColor[combNo], m_AttributeParam.m_ConsntantColor[combNo] );
			}

			{
				glBlendColor( m_AttributeParam.m_EtcColor.m_BlendColor.x, m_AttributeParam.m_EtcColor.m_BlendColor.y, m_AttributeParam.m_EtcColor.m_BlendColor.z, m_AttributeParam.m_EtcColor.m_BlendColor.w );
				m_pShaderObject->SetVector( m_AttributeParam.m_EtcColor.m_hBufferColor, m_AttributeParam.m_EtcColor.m_BufferColor );
			}

			glUniform1i( m_AttributeParam.m_hBumpMapNo, m_AttributeParam.m_BumpMapNo );
			glUniform1i( m_AttributeParam.m_hBumpMapType, m_AttributeParam.m_BumpMapType );
			glUniform1i( m_AttributeParam.m_hZRecastEnable, m_AttributeParam.m_ZRecastEnable );

			if ( m_AttributeParam.m_BumpMapNo >= 0 )
			{
				int			tangentNo = m_AttributeParam.m_TextureAttribute[ m_AttributeParam.m_BumpMapNo ].m_UvSetNo;
				glUniform1i( m_AttributeParam.m_hTangentNo, tangentNo );
			}

			for( int texNo = 0; texNo < CombinerAttribute::TextureMax; ++texNo )
			{
				float		        rad = Math::Util::DegreeToPai( m_AttributeParam.m_TextureAttribute[texNo].m_Rotate );
				//float		        c = cos( rad ) - sin( rad );
				//float		        s = sin( rad ) + cos( rad );

				glUniform1i( m_AttributeParam.m_TextureAttribute[texNo].m_hMappingType, m_AttributeParam.m_TextureAttribute[texNo].m_MappingType );
				glUniform1i( m_AttributeParam.m_TextureAttribute[texNo].m_hUvSetNo, m_AttributeParam.m_TextureAttribute[texNo].m_UvSetNo );
				glUniform1f( m_AttributeParam.m_TextureAttribute[texNo].m_hScaleU, m_AttributeParam.m_TextureAttribute[texNo].m_ScaleU );
				glUniform1f( m_AttributeParam.m_TextureAttribute[texNo].m_hScaleV, m_AttributeParam.m_TextureAttribute[texNo].m_ScaleV );
				glUniform1f( m_AttributeParam.m_TextureAttribute[texNo].m_hRotate, rad );
				glUniform1f( m_AttributeParam.m_TextureAttribute[texNo].m_hTranslateU, m_AttributeParam.m_TextureAttribute[texNo].m_TranslateU );
				glUniform1f( m_AttributeParam.m_TextureAttribute[texNo].m_hTranslateV, m_AttributeParam.m_TextureAttribute[texNo].m_TranslateV );
			}

			for( int type = 0; type < LookUpTableType::NumberOf; ++type )
			{
				float ScaleTypeTable[] = { 0.25f, 0.5f, 1.0f, 2.0f, 4.0f, 8.0f };
				float scaleValue = ScaleTypeTable[ m_AttributeParam.m_LookUpTable[type].m_LutScale ];

				glUniform1i( m_AttributeParam.m_LookUpTable[type].m_hLutInputAngle, m_AttributeParam.m_LookUpTable[type].m_LutInputAngle );
				glUniform1f( m_AttributeParam.m_LookUpTable[type].m_hLutScale, scaleValue );
			}

			glUniform1i( m_AttributeParam.m_hPrimaryFresnelEnable, m_AttributeParam.m_PrimaryFresnelEnable );
			glUniform1i( m_AttributeParam.m_hSecondaryFresnelEnable, m_AttributeParam.m_SecondaryFresnelEnable );
			glUniform1i( m_AttributeParam.m_hSpecularColorClamp, m_AttributeParam.m_SpecularColorClamp );
			glUniform1i( m_AttributeParam.m_hCookTorrance0Enable, m_AttributeParam.m_CookTorrance0Enable );
			glUniform1i( m_AttributeParam.m_hCookTorrance1Enable, m_AttributeParam.m_CookTorrance1Enable );

			for( int i = 0; i < CombinerAttribute::CombinerMax; ++i )
			{
				for( int type = 0; type < CombinerColorType::NumberOf; ++type )
				{
					glUniform1i( m_AttributeParam.m_TextureCombinerAttribute[type][i].m_hMathFormula, m_AttributeParam.m_TextureCombinerAttribute[type][i].m_MathFormula );

					{
						int		scaleNo = m_AttributeParam.m_TextureCombinerAttribute[type][i].m_Scale;
						float		scalePow[] = { 1.0f, 2.0f, 4.0f };
						glUniform1f( m_AttributeParam.m_TextureCombinerAttribute[type][i].m_hScale, scalePow[scaleNo] );
					}

					for( int i2 = 0; i2 < InputSourceType::NumberOf; ++i2 )
					{
						glUniform1i( m_AttributeParam.m_TextureCombinerAttribute[type][i].m_hInputSource[i2], m_AttributeParam.m_TextureCombinerAttribute[type][i].m_InputSource[i2] );
						glUniform1i( m_AttributeParam.m_TextureCombinerAttribute[type][i].m_hOperand[i2], m_AttributeParam.m_TextureCombinerAttribute[type][i].m_Operand[i2] );
					}

					glUniform1i( m_AttributeParam.m_TextureCombinerAttribute[type][i].m_hOutPutBuffer, m_AttributeParam.m_TextureCombinerAttribute[type][i].m_OutPutBuffer );
				}

				glUniform1i( m_AttributeParam.m_hTextureCombinerConstant[i], m_AttributeParam.m_TextureCombinerConstant[i] );
			}

			{
				static const GLint func[] = { GL_LESS, GL_LEQUAL, GL_EQUAL, GL_GEQUAL, GL_GREATER, GL_NOTEQUAL, GL_ALWAYS, GL_NEVER };
				static const GLint stencil_op[] = { GL_KEEP, GL_ZERO, GL_REPLACE, GL_INCR, GL_INCR_WRAP, GL_DECR, GL_DECR_WRAP, GL_INVERT };

				//m_AttributeParam.m_DrawLayer		これは再現不可能
				glEnable( GL_ALPHA_TEST );
				glAlphaFunc( func[ m_AttributeParam.m_AlphaTestType ], m_AttributeParam.m_AlphaTestRefValue );

				{
					glEnable( GL_DEPTH_TEST );
					glDepthFunc( func[ m_AttributeParam.m_DepthTestType ] );
					glDepthMask( m_AttributeParam.m_UpDateDepth ? GL_TRUE : GL_FALSE );

					glEnable( GL_POLYGON_OFFSET_FILL );
					glPolygonOffset( 1.0f, static_cast<float>( m_AttributeParam.m_PolygonOffSet ) );
				}

				if ( 0 )
				{//Mayaはステンシルを持たない
					if ( m_AttributeParam.m_StencilTestEnable )				glEnable( GL_STENCIL_TEST );
					else																							glDisable( GL_STENCIL_TEST );

					glStencilFunc(func[ m_AttributeParam.m_StencilTestType ],
												m_AttributeParam.m_StencilValue,
												m_AttributeParam.m_StencilMask );
					glStencilOp(stencil_op[m_AttributeParam.m_StencilFailOp],
											stencil_op[m_AttributeParam.m_StencilZFailOp],
											stencil_op[m_AttributeParam.m_StencilZpassOp] );
				}

				switch( m_AttributeParam.m_BlendMode ){
				case BlendModeType::Blend:
				case BlendModeType::Separate:
					{
						glEnable( GL_BLEND );
						glDisable( GL_COLOR_LOGIC_OP );

						{
							static const GLint mode[] = { GL_FUNC_ADD, GL_FUNC_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT, GL_MIN, GL_MAX };

							GLenum		modeRGB = mode[ m_AttributeParam.m_BlendFunc[CombinerColorType::RGB].m_BlendFunc ];
							GLenum		modeAlpha = mode[ m_AttributeParam.m_BlendFunc[CombinerColorType::A].m_BlendFunc ];

							if ( m_AttributeParam.m_BlendMode == BlendModeType::Blend )
								modeAlpha = modeRGB;

							glBlendEquationSeparate( modeRGB, modeAlpha );
						}

						{
							static const GLint mode[] = { 
								GL_ZERO, GL_ONE, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR, GL_SRC_ALPHA,
								GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR,
								GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA, GL_SRC_ALPHA_SATURATE
							};

							GLenum		srcRGB = mode[ m_AttributeParam.m_BlendFunc[CombinerColorType::RGB].m_SrcElement ];
							GLenum		dstRGB = mode[ m_AttributeParam.m_BlendFunc[CombinerColorType::RGB].m_DstElement ];
							GLenum		srcAlpha = mode[ m_AttributeParam.m_BlendFunc[CombinerColorType::A].m_SrcElement ];
							GLenum		dstAlpha = mode[ m_AttributeParam.m_BlendFunc[CombinerColorType::A].m_DstElement ];

							if ( m_AttributeParam.m_BlendMode == BlendModeType::Blend )
							{
								srcAlpha = srcRGB;
								dstAlpha = dstRGB;
							}

							glBlendFuncSeparate( srcRGB, dstRGB, srcAlpha, dstAlpha );
						}
					}
					break;
				case BlendModeType::LogicOp:
					{
						static const GLint mode[] = { 
							GL_CLEAR,//すべてのビット値を 0
							GL_SET,//すべてのビット値を 1	

							GL_COPY,//ソースカラーのビット値	
							GL_NOOP,//デスティネーションカラーのビット値	
							
							GL_COPY_INVERTED,//ソースカラーの反転ビット値	
							GL_INVERT,//デスティネーションカラーの反転ビット値	

							GL_AND,//ソースとデスティネーションカラーの論理積	
							GL_OR,//ソースとデスティネーションカラーの論理和	
							GL_XOR,//ソースとデスティネーションカラーの排他的論理和	

							GL_AND_REVERSE,//ソースとデスティネーションカラーの反転ビット値の論理積	
							GL_OR_REVERSE,//ソースとデスティネーションカラーの反転ビット値の論理和	
							
							GL_AND_INVERTED,//ソースカラーの反転ビット値と、デスティネーションカラーの論理積	
							GL_OR_INVERTED,//ソースカラーの反転ビット値と、デスティネーションカラーの論理和	

							GL_NAND,//ソースとデスティネーションカラーの論理積の反転ビット値	
							GL_NOR,//ソースとデスティネーションカラーの論理和の反転ビット値						
							GL_EQUIV,//ソースとデスティネーションカラーの排他的論理和の反転ビット値	
						};

						glDisable( GL_BLEND );
						glEnable( GL_COLOR_LOGIC_OP );
						glLogicOp( mode[ m_AttributeParam.m_LogicalOperation ] );
					}
					break;
				case BlendModeType::None:
					{
						glDisable( GL_BLEND );
						glDisable( GL_COLOR_LOGIC_OP );
					}
					break;
				}
			}

			{
				if ( m_AttributeParam.m_VertexShaderType.IsPokemonShader() )
				{
					glUniform1i( m_AttributeParam.m_hIsPokemonShader, m_AttributeParam.m_VertexShaderType.IsPokemonShader() );
					glUniform1i( m_AttributeParam.m_hFsIsPokemonShader, m_AttributeParam.m_VertexShaderType.IsPokemonShader() );
				}
				else
				{//旧パラメーターを使用
					glUniform1i( m_AttributeParam.m_hIsPokemonShader, m_AttributeParam.m_IsPokemonShader );
					glUniform1i( m_AttributeParam.m_hFsIsPokemonShader, m_AttributeParam.m_IsPokemonShader );
				}
				glUniform1i( m_AttributeParam.m_hUseObjectSpaceNormalMap, m_AttributeParam.m_UseObjectSpaceNormalMap );
				glUniform1i( m_AttributeParam.m_hUseShadowTexture, m_AttributeParam.m_UseShadowTexture );
			}

			{
#if 0
				if ( m_AttributeParam.m_VertexShaderType.Is( VertexShaderType::Contrast ) )
				{
					std::vector<Util::UserAttribute>		sceneSettingAttr = MenuWindow::XyWindow::GetUserAttribute( "SceneSetting" );

					for( int i = 0; i < sceneSettingAttr.size(); ++i )
					{
						if ( sceneSettingAttr[i].m_AttributeName == "GlobalContrast" )
						{
							glUniform1f( m_AttributeParam.m_hVsSceneContrast, sceneSettingAttr[i].m_FloatVal );
							glUniform1i( m_AttributeParam.m_hVsSceneContrastEnable, true );
						}
						else if ( sceneSettingAttr[i].m_AttributeName == "ZoneContrast" )
						{
							glUniform1f( m_AttributeParam.m_hVsZoneContrast, sceneSettingAttr[i].m_FloatVal );
							glUniform1i( m_AttributeParam.m_hVsSceneContrastEnable, true );
						}
					}
				}
				else
#endif
				{
					glUniform1i( m_AttributeParam.m_hVsSceneContrastEnable, false );
				}
			}

			{
				Math::Vector				param( 1.0f, 0.0f, 1.0f, 0.0f );
			
				{
#if 0
					plug = shaderNode.findPlug( ShaderNodeBase::s_RimPowAttributeName, &status );
					if ( status == MStatus::kSuccess )
						plug.getValue( param.x );
					plug = shaderNode.findPlug( ShaderNodeBase::s_RimScaleAttributeName, &status );
					if ( status == MStatus::kSuccess )
						plug.getValue( param.y );
					plug = shaderNode.findPlug( ShaderNodeBase::s_PhongPowAttributeName, &status );
					if ( status == MStatus::kSuccess )
						plug.getValue( param.z );
					plug = shaderNode.findPlug( ShaderNodeBase::s_PhongScaleAttributeName, &status );
					if ( status == MStatus::kSuccess )
						plug.getValue( param.w );
#endif

					m_pShaderObject->SetVector( m_AttributeParam.m_hVsRimParam, param );
				}

#if 0
				{
					for( int i = 0; i < 4; ++i )
					{
						plug = shaderNode.findPlug( ShaderNodeBase::s_ShaderParam[i], &status );
						if ( status == MStatus::kSuccess )
							plug.getValue( param[i] );
					}

					m_pShaderObject->SetVector( m_hGsShaderParam, param );
				}
#endif
			}
		}
	};

	GLInterface::ShaderObject								*m_pShaderObject = m_pImpl[shaderType]->m_pShaderObject;
	CombinerAttribute::AttributeParam				&m_AttributeParam = m_pImpl[shaderType]->m_AttributeParam;

	m_pShaderObject->SetShader();

	for( int combNo = 0; combNo < CombinerAttribute::CombinerMax; ++combNo )
	{
		m_pShaderObject->SetVector( m_AttributeParam.m_hConsntantColor[combNo], m_AttributeParam.m_ConsntantColor[combNo] );
	}

	m_pShaderObject->SetMatrix( m_pImpl[shaderType]->m_hVsModelMatrix, Math::Matrix::GetIdentity() );

	LocalFunc::setAttributeParam( m_pShaderObject, m_AttributeParam );

	{//ライト設定
		glUniform1i( m_pImpl[shaderType]->m_hPsObjSpaceLightCount, 1 );
		
		{
			m_pShaderObject->SetVector( m_pImpl[shaderType]->m_hPsObjSpaceLightPos[0], Math::Vector(0, 0, 10000, 1) );
			m_pShaderObject->SetVector( m_pImpl[shaderType]->m_hPsObjSpaceLightVec[0], Math::Vector(0, 0, -1, 1) );
			m_pShaderObject->SetVector( m_pImpl[shaderType]->m_hPsObjSpaceLightColor[0], Math::Vector(0.7f, 0.7f, 0.7f, 0.7f) );
			glUniform1f( m_pImpl[shaderType]->m_hPsObjSpaceLightConeAngle[0], 0.0f );
			glUniform1f( m_pImpl[shaderType]->m_hPsIsSpotLight[0], false );
		}

		{
			Math::Vector						lightVec(1.0f, 0.0f, 0.0f );
			Math::Vector						lightCol(1.0f, 1.0f, 1.0f );

			m_pShaderObject->SetVector( m_pImpl[shaderType]->m_hVsWorldLightVec, lightVec );
			m_pShaderObject->SetVector( m_pImpl[shaderType]->m_hVsWorldLightCol, lightCol );

			glUniform1i( m_pImpl[shaderType]->m_hVsHalfVecLightEnable, true );//デバッグ用デフォルトライト有効無効
		}

		m_pShaderObject->SetVector( m_pImpl[shaderType]->m_hVsAmbientLightColor, Math::Vector(0.3f, 0.3f, 0.3f, 0.3f) );	
		m_pShaderObject->SetVector( m_pImpl[shaderType]->m_hPsAmbientLightColor, Math::Vector(0.3f, 0.3f, 0.3f, 0.3f) );	
	}

	{//@todo 外から設定する
		m_pShaderObject->SetVector( m_pImpl[shaderType]->m_hVsCommonParam, Math::Vector(m_pImpl[shaderType]->m_FarLength, 0, 0, 0) );
	}
	
	//m_DraswUvSpaceNo = 0;
	glUniform1i( m_pImpl[shaderType]->m_hVsDrawUvSpaceNo, m_DraswUvSpaceNo );
	glUniform1i( m_pImpl[shaderType]->m_hPsDrawUvSpaceNo, m_DraswUvSpaceNo );
	glUniform1i( m_pImpl[shaderType]->m_hPsDrawUvDepthTest, m_DrawUvDepthTest );	

	glUniform1i( m_pImpl[shaderType]->m_hGsDrawUvSpace, (m_DraswUvSpaceNo >= 0 ) ? true : false );

	//ここでシェーダーバインド
	//glUseProgram(0);
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void gfl::cmdl::Material::glDeleteShader()
{
	for( int i= 0; i < ShaderType::NumberOf; ++i )
		SAFE_DELETE( m_pImpl[i] );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void gfl::cmdl::Material::SetShaderType(ShaderType::Enum type)
{
	this->m_ShaderType = type;
}

#endif


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void gfl::cmdl::Model::RenderPrimitive( )
{
	Mesh* mesh = GetMesh( );
	if(mesh){
		glEnable( GL_TEXTURE_2D );
		glShadeModel( GL_SMOOTH );
		for( s32 i = 0; i < GetMeshCount( ); ++i, ++mesh ){
			Material* mate = mesh->GetMaterial();
			if(mate){
				mate->GetMaterialColor()->SetLight();
				mate->GetTextureCoordinator( )->SetTextureEnv(*mesh->GetMaterial( )->GetTextureMapper( )->GetTextureId( ));
				mate->GetTextureMapper( )->SetTextureEnv();
			}
			Shape* shape = mesh->GetShape( );
			shape->RenderPolygon( shape->GetVertices( ) );
		}
		glDisable( GL_TEXTURE_2D );
	}
}

// ---------------------------------------------------------------------------
// ワイヤーフレーム描画
// ---------------------------------------------------------------------------
void gfl::cmdl::Shape::RenderPolygonWireFrame( gfl::core::Vector* vp )
{
	Shape::Primitive* pri = GetPrimitives( );
	for( s32 j = 0; j < GetPrimitiveCount( ); ++j, ++pri ){
		s32* tri = pri->GetTriangles( );
		for( s32 k = 0; k < pri->GetTriangleCount( ); ++k ){
			glBegin( GL_LINE_LOOP );
			glVertex3fv( vp[*tri++].Get( ) );
			glVertex3fv( vp[*tri++].Get( ) );
			glVertex3fv( vp[*tri++].Get( ) );
			glEnd( );
		}
	}
}

// ---------------------------------------------------------------------------
// ポリゴン描画
// ---------------------------------------------------------------------------
void gfl::cmdl::Shape::RenderPolygon( gfl::core::Vector* vp )
{
	Primitive* pri = GetPrimitives( );
	gfl::core::Vector2* tex = GetTextures( );
	//	gfl::core::Vector* np = GetNormals();
	if( tex ){
		for( s32 j = 0; j < GetPrimitiveCount( ); ++j, ++pri ){
			s32* tri = pri->GetTriangles( );
			for( s32 k = 0; k < pri->GetTriangleCount( ); ++k ){
				glBegin( GL_TRIANGLES );
				gfl::qt::Render::SetTriangleNormal( vp[tri[0]], vp[tri[1]], vp[tri[2]] );

				s32 idx = *tri++;
				//				glNormal3fv( np[idx].Get() );
				glTexCoord2fv( tex[idx].Get( ) );
				glVertex3fv( vp[idx].Get( ) );

				idx = *tri++;
				//				glNormal3fv( np[idx].Get() );
				glTexCoord2fv( tex[idx].Get( ) );
				glVertex3fv( vp[idx].Get( ) );

				idx = *tri++;
				//				glNormal3fv( np[idx].Get() );
				glTexCoord2fv( tex[idx].Get( ) );
				glVertex3fv( vp[idx].Get( ) );
				glEnd( );
			}
		}
	} else {
		for( s32 j = 0; j < GetPrimitiveCount( ); ++j, ++pri ){
			s32* tri = pri->GetTriangles( );
			for( s32 k = 0; k < pri->GetTriangleCount( ); ++k ){
				glBegin( GL_TRIANGLES );
				gfl::qt::Render::SetTriangleNormal( vp[tri[0]], vp[tri[1]], vp[tri[2]] );

				s32 idx = *tri++;
				//				glNormal3fv( np[idx].Get() );
				glVertex3fv( vp[idx].Get( ) );

				idx = *tri++;
				//				glNormal3fv( np[idx].Get() );
				glVertex3fv( vp[idx].Get( ) );

				idx = *tri++;
				//				glNormal3fv( np[idx].Get() );
				glVertex3fv( vp[idx].Get( ) );
				glEnd( );
			}
		}
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void gfl::cmdl::Shape::RenderSkinPolygon( gfl::core::Vector* skin_vert, const gfl::qt::Render::ShadingType type )
{
	if( type == gfl::qt::Render::SHD_WIRE ){
		RenderPolygonWireFrame( skin_vert );
	} else {
		RenderPolygon( skin_vert );
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void gfl::cmdl::Model::RenderSkin( gfl::motion::MotionBase* mot, const gfl::qt::Render::ShadingType type, const size_t ofs )
{
	gfl::core::Matrix* mat = GetJointMatrix( );
	const s32 size = GetBoneCount();
	if( !mat ){
		mat = DbgNew gfl::core::Matrix [size];
		SetJointMatrix( mat );
	}
	for( s32 i = 0; i < size; ++i, ++mat ){
		gfl::motion::JointBase* jnt = mot->GetJointByName<gfl::motion::JointBase*>( GetBone( )[i].GetName( ).toAscii( ).data( ) );
		if( jnt ){
			mat->Set( *GFL_GET_POINTER_BY_OFFSET( jnt, gfl::core::Matrix, ofs ) );
		} else {
			mat->SetUnit();
		}
	}
	RenderSkin(GetJointMatrix( ),type);
	
#if 0 // 
#if 1 // CMDL
	Mesh* mesh = GetMesh( );
	const s32 shape_count = GetShapeCount( );
	Shape* shape = GetShape( );

	glEnable( GL_TEXTURE_2D );
//	glShadeModel( GL_SMOOTH );
	if( !GetJoint( ) ){
		const s32 size = GetBoneCount();
		motion::JointBase** idp = DbgNew motion::JointBase* [size];
		SetJoint( idp );
		for( s32 i = 0; i < size; ++i ){
			gfl::motion::JointBase* jnt = mot->GetJointByName<gfl::motion::JointBase*>( GetBone( )[i].GetName( ).toAscii( ).data( ) );
			if( jnt ){
				*idp++ = jnt;
			} else {
				//					m_ErrorMessage = "Skin Error : " + GetBone( )[id_tbl[i]].GetName( ) + "にスキンが割り当てられています";
				*idp++ = 0;
			}
		}
	}
			
	for( s32 i = 0; i < shape_count; ++i, ++shape, ++mesh ){
#if 1 // 
		gfl::cmdl::Material* mate = mesh->GetMaterial();
		if(!mate){
			continue;
		}
		gfl::cmdl::Material::MaterialColor* mc = mate->GetMaterialColor();
		mc->SetLight();
		Material::TextureMapper* tm = mesh->GetMaterial( )->GetTextureMapper( );
		glColor3fv( mc->GetDiffuse().Get() );
		mesh->GetMaterial( )->GetTextureCoordinator( )->SetTextureEnv(*tm->GetTextureId( ));
		mesh->GetMaterial( )->GetTextureMapper( )->SetTextureEnv();
#endif
		// スキン計算
		const s32* bone_idx = shape->GetBoneIdIndex( );
		const f32* idw = shape->GetWeights( );
		const s32 step = shape->GetIndexStep( );
		const gfl::core::Vector2* tex = shape->GetTextures( );
		Shape::Primitive* pri = shape->GetPrimitives( );
		for( s32 p = 0; p < shape->GetPrimitiveCount( ); ++p, ++pri )
		{
			const s32* id_tbl = pri->GetBoneIndexTable( );
			const s32 loop = pri->GetTriangleCount( ); // 3 == triangle edge count
			const s32* idx = pri->GetTriangles();
			gfl::core::Vector* vert = shape->GetVertices( );
			for( s32 j = 0; j < loop; ++j ){
				gfl::core::Vector vp[gfl::core::TXYZ];
				for( s32 l = 0; l < 3; ++l ){ // 3 == triangle edge count
					vp[l].SetZero( );
					const s32 id_val = idx[l];

					gfl::core::Vector v;					
					for( s32 k = 0; k < step; ++k ){
						const s32 idxx = id_val * step + k;
						const s32 b_id = id_tbl[bone_idx[idxx]];
						gfl::motion::JointBase* col_jnt = GetJoint( )[b_id];
						if( col_jnt ){
							( ( *GFL_GET_POINTER_BY_OFFSET( col_jnt
								, gfl::core::Matrix, ofs ) ) * (*GetBone()[b_id].GetInvMatrix()) ).Trans( &v, vert[id_val] );
							vp[l] += (idw)? v * idw[ idxx] : v ;
						}
					}
				}
				
				glBegin( GL_TRIANGLES );
				gfl::qt::Render::SetTriangleNormal( vp[0], vp[1], vp[2] );

				//				glNormal3fv( np[idx].Get() );
				glTexCoord2fv( tex[idx[0]].Get( ) );
				glVertex3fv( vp[0].Get( ) );

				//				glNormal3fv( np[idx].Get() );
				glTexCoord2fv( tex[idx[1]].Get( ) );
				glVertex3fv( vp[1].Get( ) );

				//				glNormal3fv( np[idx].Get() );
				glTexCoord2fv( tex[idx[2]].Get( ) );
				glVertex3fv( vp[2].Get( ) );
				
				idx += gfl::core::TXYZ;
				glEnd( );
			}
		}
	}
	glDisable( GL_TEXTURE_2D );
#else // DAE
	Mesh* mesh = GetMesh( );
	const s32 shape_count = GetShapeCount( );
	Shape* shape = GetShape( );

	glEnable( GL_TEXTURE_2D );
	glShadeModel( GL_SMOOTH );
	for( s32 i = 0; i < shape_count; ++i, ++shape, ++mesh ){
#if 1 // 
		Material::TextureMapper* tm = mesh->GetMaterial( )->GetTextureMapper( );
		glColor3fv( tm->GetDiffuse().Get() );
		glMaterialfv( GL_FRONT, GL_EMISSION, tm->GetEmission().Get() );
		glMaterialfv( GL_FRONT, GL_AMBIENT, tm->GetAmbient().Get() );
		glMaterialfv( GL_FRONT, GL_DIFFUSE, tm->GetDiffuse().Get() );
		glMaterialfv( GL_FRONT, GL_SPECULAR, tm->GetSpecular().Get() );
		tm->SetTextureEnv();
#endif
		// スキン計算
		s32 loop = shape->GetIndexCount( );

		gfl::core::Vector *shape_vert_org = shape->GetWorkVertices( );
		if( !shape_vert_org ){
			shape_vert_org = DbgNew gfl::core::Vector [loop];
			shape->SetWorkVertices( shape_vert_org );
		}

		Shape::Primitive* pri = shape->GetPrimitives( );
		const s32* id_tbl = pri->GetBoneIndexTable( );
		if( !pri->GetJoint( ) ){
			const s32 size = pri->GetBoneIndexTableCount( );
			motion::JointBase** idp = DbgNew motion::JointBase* [size];
			pri->SetJoint( idp );
			for( s32 i = 0; i < size; ++i ){
				gfl::motion::JointBase* jnt = mot->GetJointByName<gfl::motion::JointBase*>( GetBone( )[id_tbl[i]].GetName( ).toAscii( ).data( ) );
				if( jnt ){
					*idp++ = jnt;
				} else {
//					m_ErrorMessage = "Skin Error : " + GetBone( )[id_tbl[i]].GetName( ) + "にスキンが割り当てられています";
					*idp++ = 0;
				}
			}
		}

		const s32* idx = shape->GetBoneIdIndex( );
		const f32* idw = shape->GetWeights( );
		gfl::core::Vector* vert = shape->GetVertices( );
		gfl::core::Vector* shape_vert = shape_vert_org;
		for( s32 j = 0; j < loop; ++j, ++vert, ++shape_vert ){
			shape_vert->SetZero( );
			for( s32 k = 0; k < shape->GetIndexStep( ); ++k ){
				const s32 id = *idx++;
				gfl::core::Vector v;
				gfl::motion::JointBase* col_jnt = pri->GetJoint( )[id];
				if( col_jnt ){
					( ( *GFL_GET_POINTER_BY_OFFSET( col_jnt, gfl::core::Matrix, ofs ) ) * (*GetBone()[id_tbl[id]].GetInvMatrix()) )
						.Trans( &v, *vert );
					*shape_vert += v * ( *idw++ );
				}
			}
		}
		shape->RenderSkinPolygon( shape_vert_org, type );
	}
	glDisable( GL_TEXTURE_2D );
#endif
#endif
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

static void DrawCube(double size=1.0)
{
	glBegin( GL_QUADS );
	glNormal3d( 1.0, 0.0, 0.0 );
	glTexCoord2f( 0.0f, 0.0f );		glVertex3f( size/2.0, size/2.0, -size/2.0 );
	glTexCoord2f( 1.0f, 0.0f );		glVertex3f( size/2.0, size/2.0, size/2.0 );
	glTexCoord2f( 1.0f, 1.0f );		glVertex3f( size/2.0, -size/2.0, size/2.0 );
	glTexCoord2f( 0.0f, 1.0f );		glVertex3f( size/2.0, -size/2.0, -size/2.0 );

	glNormal3d( -1.0, 0.0, 0.0 );
	glTexCoord2f( 0.0f, 0.0f );		glVertex3f( -size/2.0, size/2.0, size/2.0 );
	glTexCoord2f( 1.0f, 0.0f );		glVertex3f( -size/2.0, size/2.0, -size/2.0 );
	glTexCoord2f( 1.0f, 1.0f );		glVertex3f( -size/2.0, -size/2.0, -size/2.0 );
	glTexCoord2f( 0.0f, 1.0f );		glVertex3f( -size/2.0, -size/2.0, size/2.0 );

	glNormal3d( 0.0, 0.0, 1.0 );
	glTexCoord2f( 1.0f, 0.0f );		glVertex3f( -size/2.0, size/2.0, size/2.0 );
	glTexCoord2f( 1.0f, 1.0f );		glVertex3f( -size/2.0, -size/2.0, size/2.0 );
	glTexCoord2f( 0.0f, 1.0f );		glVertex3f( size/2.0, -size/2.0, size/2.0 );
	glTexCoord2f( 0.0f, 0.0f );		glVertex3f( size/2.0, size/2.0, size/2.0 );

	glNormal3d( 0.0, 0.0, -1.0 );
	glTexCoord2f( 0.0f, 0.0f );		glVertex3f( -size/2.0, size/2.0, -size/2.0 );
	glTexCoord2f( 1.0f, 0.0f );		glVertex3f( size/2.0, size/2.0, -size/2.0 );
	glTexCoord2f( 1.0f, 1.0f );		glVertex3f( size/2.0, -size/2.0, -size/2.0 );
	glTexCoord2f( 0.0f, 1.0f );		glVertex3f( -size/2.0, -size/2.0, -size/2.0 );

	glNormal3d( 0.0, -1.0, 0.0 );
	glTexCoord2f( 0.0f, 0.0f );		glVertex3f( -size/2.0, -size/2.0, size/2.0 );
	glTexCoord2f( 0.0f, 1.0f );		glVertex3f( -size/2.0, -size/2.0, -size/2.0 );
	glTexCoord2f( 1.0f, 1.0f );		glVertex3f( size/2.0, -size/2.0, -size/2.0 );
	glTexCoord2f( 1.0f, 0.0f );		glVertex3f( size/2.0, -size/2.0, size/2.0 );

	glNormal3d( 0.0, 1.0, 0.0 );
	glTexCoord2f( 0.0f, 0.0f );		glVertex3f( -size/2.0, size/2.0, size/2.0 );
	glTexCoord2f( 1.0f, 0.0f );		glVertex3f( size/2.0, size/2.0, size/2.0 );
	glTexCoord2f( 1.0f, 1.0f );		glVertex3f( size/2.0, size/2.0, -size/2.0 );
	glTexCoord2f( 0.0f, 1.0f );		glVertex3f( -size/2.0, size/2.0, -size/2.0 );
	glEnd();
}

void gfl::cmdl::Model::RenderSkin(gfl::core::Matrix* mat, const gfl::qt::Render::ShadingType type )
{
#if 1 // CMDL
#ifdef MAYA_INPORT
	glUseProgram( 0 );
#    endif

	Mesh* mesh = GetMesh( );
	const s32 shape_count = GetShapeCount( );

	glEnable( GL_TEXTURE_2D );

#    ifdef MAYA_INPORT
	for( s32 layerNo = 0; layerNo < 4; ++layerNo )
#endif
	{
		for( s32 i = 0; i < GetMeshCount(); ++i )
		{
			gfl::cmdl::Material		*mate = mesh[i].GetMaterial();
			Shape									*shape = mesh[i].GetShape();

			//QString								matName = mate->GetName();

			if(!mate){ // skip ParticleShape
				continue;
			}

	#    ifdef MAYA_INPORT //
			if ( mate->m_ShaderType == Material::ShaderType::Ignore )
			{ // skip Ignore
				continue;
			}

			{
				static QString translucencyKindNames[4] = {
					"Layer0", "Layer1", "Layer2", "Layer3"
				};

				if( mate->m_TranslucencyKind != translucencyKindNames[ layerNo ] )
					continue;
			}

			mate->SetCamera( m_pCamera );
			mate->SetLightEnable( m_fLightEnable );

			/*{
				s32 val = MainWindow::GetInstance()->GetTextureWidget()->GetMaterialTreatment( mate->GetName() );

				switch( val ){
				case TextureWidget::MATERIAL_SPECIAL:
					mate->glBind( Material::ShaderType::SimpleIDRender, m_LookupTable );
					break;
				default:
					
					break;
				}
			}*/

			mate->glBind( mate->m_ShaderType, m_LookupTable );
			
	#    endif

			// スキン計算
            shape->Skining( mat, GetBone(), mate );
//			shape->m_BoundingBox.Render();

			if(shape->GetIndexCount())
			{
	#        ifdef MAYA_INPORT // 

				void   *p; 

				p = shape->GetWorkVertices();
				if ( shape->GetWorkVertices() )
				{
					glEnableVertexAttribArray( GLInterface::StreamUsage::Position );
					glVertexAttribPointer( GLInterface::StreamUsage::Position, 3, GLInterface::VertexType::Float, GL_FALSE, sizeof(float) * 3, shape->GetWorkVertices( ) );
				}else{
					glDisableVertexAttribArray( GLInterface::StreamUsage::Position );
				}

				p = shape->GetWorkTextures(0);
				if ( shape->GetWorkTextures(0) )
				{
					glEnableVertexAttribArray( GLInterface::StreamUsage::TexCoord0 );
					glVertexAttribPointer( GLInterface::StreamUsage::TexCoord0, 2, GLInterface::VertexType::Float, GL_FALSE, sizeof(float) * 2, shape->GetWorkTextures(0) );
				}else{
					glDisableVertexAttribArray( GLInterface::StreamUsage::TexCoord0 );
				}

				p = shape->GetWorkTextures(1);
				if ( shape->GetWorkTextures(1) )
				{
					glEnableVertexAttribArray( GLInterface::StreamUsage::TexCoord1 );
					glVertexAttribPointer( GLInterface::StreamUsage::TexCoord1, 2, GLInterface::VertexType::Float, GL_FALSE, sizeof(float) * 2, shape->GetWorkTextures(1) );
				}else{
					glDisableVertexAttribArray( GLInterface::StreamUsage::TexCoord1 );
				}

				p = shape->GetWorkTextures(2);
				if ( shape->GetWorkTextures(2) )
				{
					glEnableVertexAttribArray( GLInterface::StreamUsage::TexCoord2 );
					glVertexAttribPointer( GLInterface::StreamUsage::TexCoord2, 2, GLInterface::VertexType::Float, GL_FALSE, sizeof(float) * 2, shape->GetWorkTextures(2) );
				}else{
					glDisableVertexAttribArray( GLInterface::StreamUsage::TexCoord2 );
				}

				p = shape->GetWorkNormals();
				if ( shape->GetWorkNormals() )
				{
					glEnableVertexAttribArray( GLInterface::StreamUsage::Normal );
					glVertexAttribPointer( GLInterface::StreamUsage::Normal, 3, GLInterface::VertexType::Float, GL_FALSE, sizeof(float) * 3, shape->GetWorkNormals() );
				}else{
					glDisableVertexAttribArray( GLInterface::StreamUsage::Normal );
				}

				p = shape->GetWorkColor();
				if ( shape->GetWorkColor() )
				{
					glEnableVertexAttribArray( GLInterface::StreamUsage::Color );
					glVertexAttribPointer( GLInterface::StreamUsage::Color, 4, GLInterface::VertexType::UByte, GL_FALSE, sizeof(unsigned char) * 4, shape->GetWorkColor() );
				}else{
					glDisableVertexAttribArray( GLInterface::StreamUsage::Color );
				}

//				int		cnt = shape->GetIndexCount();
				glDrawArrays(GL_TRIANGLES, 0, shape->GetIndexCount());
	#        else // 消さないで?
				glEnableClientState(GL_VERTEX_ARRAY);
				glEnableClientState(GL_NORMAL_ARRAY);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glVertexPointer(3,GL_FLOAT,0,shape->GetWorkVertices( ));
				glNormalPointer(GL_FLOAT,0,shape->GetWorkNormals( ));
	//			glTexCoordPointer(2, GL_FLOAT, 0, shape->GetWorkTextures());

				gfl::cmdl::Material::MaterialColor* mc = mate->GetMaterialColor();
				mc->SetLight();
				glColor3fv( mc->GetDiffuse().Get() );

				Material::TextureMapper* tm = mate->GetTextureMapper( );
				Material::TextureCoordinator* tc = mate->GetTextureCoordinator( );
				// とりあえず　１にする テスト用
				s32 tex_max = 1;
				for( s32 t = 0; t < tex_max; ++t ){
					tc[t].SetTextureEnv(*tm[t].GetTextureId( ));
					tm[t].SetTextureEnv();
					
					gfl::core::Vector2* tex = shape->GetWorkTextures(t);
					if(tex){
						glTexCoordPointer(2, GL_FLOAT, 0, tex );
					}
				}
				glDrawArrays(GL_TRIANGLES, 0, shape->GetIndexCount());
				//glDrawArrays(GL_TRIANGLES, 0, 3);
	#        endif
			}

			//glEnd();

		}
	}

#ifdef MAYA_INPORT
	glUseProgram(0);//シェーダーアンバインド
#    endif
	glDisable( GL_TEXTURE_2D );
#else // DAE
	Mesh* mesh = GetMesh( );
	const s32 shape_count = GetShapeCount( );
	Shape* shape = GetShape( );

	glEnable( GL_TEXTURE_2D );
	glShadeModel( GL_SMOOTH );
	for( s32 i = 0; i < shape_count; ++i, ++shape, ++mesh ){
#if 1 // 
		Material::TextureMapper* tm = mesh->GetMaterial( )->GetTextureMapper( );
		glColor3fv( tm->GetDiffuse().Get() );
		glMaterialfv( GL_FRONT, GL_EMISSION, tm->GetEmission().Get() );
		glMaterialfv( GL_FRONT, GL_AMBIENT, tm->GetAmbient().Get() );
		glMaterialfv( GL_FRONT, GL_DIFFUSE, tm->GetDiffuse().Get() );
		glMaterialfv( GL_FRONT, GL_SPECULAR, tm->GetSpecular().Get() );
		tm->SetTextureEnv();
#endif
		// スキン計算
		const s32 loop = shape->GetBoneIndexCount( );

		gfl::core::Vector *shape_vert_org = shape->GetWorkVertices( );
		if( !shape_vert_org ){
			shape_vert_org = DbgNew gfl::core::Vector [loop];
			shape->SetWorkVertices( shape_vert_org );
		}
		Shape::Primitive* pri = shape->GetPrimitives( );
		//for( s32 p = 0; p < shape->GetPrimitiveCount( ); ++p, ++pri )
		{
			const s32* id_tbl = pri->GetBoneIndexTable( );
			const s32* idx = shape->GetBoneIdIndex( );
			const f32* idw = shape->GetWeights( );
			gfl::core::Vector* vert = shape->GetVertices( );
			gfl::core::Vector* shape_vert = shape_vert_org;
			for( s32 j = 0; j < loop; ++j, ++vert, ++shape_vert ){
				shape_vert->SetZero( );
				for( s32 k = 0; k < shape->GetIndexStep( ); ++k ){
					//				gfl::core::Debug::PrintConsole( "[%d][%d][%d/%d] %d %d\n",i,j,k,shape->GetIndexStep( ),*idx, id_tbl[*idx] );
					const s32 id_val = *idx++;
					s32 id;
					if(id_val < pri->GetBoneIndexTableCount()){ // これが謎すぎる！わきゃわからん　やらないととんじゃうし…
						id = id_tbl[id_val];
					}else{
						s32 over_id_val = id_val - pri->GetBoneIndexTableCount();
						id = (pri+1)->GetBoneIndexTable( )[over_id_val];
					}

					gfl::core::Vector v;
					( mat[id] * (*GetBone()[id].GetInvMatrix()) ).Trans( &v, *vert );
					*shape_vert += v * ( *idw++ );
				}
			}
			shape->RenderSkinPolygon( shape_vert_org, type );
		}
	}
	glDisable( GL_TEXTURE_2D );
#endif
}

//==============================================================================
//==============================================================================
void gfl::cmdl::Model::RenderNoSkin( gfl::motion::MotionBase* mot, const gfl::qt::Render::ShadingType type, const size_t ofs )
{
#if 0 // 
	Shape* shape = GetShape( );
	for( s32 i = 0; i < GetShapeCount( ); ++i, ++shape ){
		gfl::motion::JointBase* jnt = mot->GetJointByName<gfl::motion::JointBase*>( shape->GetMeshName( ).toAscii( ).data( ) );

		if( jnt ){
			glPushMatrix( );
			GlMultMatrix( ( f32* )GFL_GET_POINTER_BY_OFFSET( jnt, gfl::core::Matrix, ofs ) );
			shape->RenderSkinPolygon( shape->GetVertices( ), type );
			glPopMatrix( );
		}
	}
#endif
}

//------------------------------------------------------------------------------
// set image data
//------------------------------------------------------------------------------
void gfl::cmdl::Model::SetImageData(s32 w, s32 h, c8* dat, s32 rgb_type, s32 order_type, Material::TextureMapper* tm)
{
	if(!tm){
		return ;
	}
	gfl::cmdl::Image* c_img = DbgNew gfl::cmdl::Image;
	c_img->SetHeader( w, h );
	c_img->SetImage( dat );
	if(rgb_type){
		c_img->SetRGB_Type(rgb_type);
	}
	if(order_type){
		c_img->SetOrderType(order_type);
	}

	c_img->SetTextureMapper(tm);
	tm->GetImageList()->append( c_img );
	GetImageData( )->append( c_img );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void gfl::cmdl::Model::SetCamera( gfl::core::Camera* pCamera )
{
	m_pCamera = pCamera;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void gfl::cmdl::Model::SetLightEnable( bool f )
{
	m_fLightEnable = f;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void gfl::cmdl::Shape::Skining( gfl::core::Matrix* mat, Bone* pBone, Material* mate )
{
	// スキン計算
	const s32* bone_idx = this->GetBoneIdIndex( );
	const f32* idw = this->GetWeights( );
	const s32 step = this->GetIndexStep( );
	const gfl::core::Vector2* tex = this->GetTextures( );
	Shape::Primitive* pri = this->GetPrimitives( );
	
	bool idv_flag = false;
	s32 idv_cnt = 0;
	gfl::core::Vector* work_vert = this->GetWorkVertices();
	gfl::core::Vector* work_norm = this->GetWorkNormals();
	u32* work_col = GetWorkColor();
	gfl::core::Vector2* work_tex[gfl::cmdl::Shape::MULTI_TEX_MAX];
	s32 tex_max = mate->GetTextureMapperCount();
	for( s32 t = 0; t < tex_max; ++t ){
		work_tex[t] = this->GetWorkTextures(t);
	}
	if( !work_vert ){ // 初期化　一回だけよ
		for( s32 p = 0; p < this->GetPrimitiveCount( ); ++p, ++pri){
			idv_cnt += pri->GetTriangleCount( );
		}
		pri = this->GetPrimitives( );
		idv_cnt *= 3; // 3 for triangle edge
		idv_flag = true;
//			gfl::core::Debug::PrintConsole( "idv_max %d\n",idv_cnt);

		work_vert = DbgNew gfl::core::Vector [idv_cnt];
		this->SetWorkVertices(work_vert);
		work_norm = DbgNew gfl::core::Vector [idv_cnt];
		this->SetWorkNormals(work_norm);
		if(GetColor( )){
			work_col = DbgNew u32 [idv_cnt];
			this->SetWorkColor(work_col);
		}
		for( s32 t = 0; t < tex_max; ++t ){
			work_tex[t] = DbgNew gfl::core::Vector2 [idv_cnt];
			this->SetWorkTextures(t,work_tex[t]);
		}
	}
	idv_cnt = 0;

	//glBegin( GL_TRIANGLES );

	for( s32 p = 0; p < this->GetPrimitiveCount( ); ++p, ++pri )
	{
		const s32 loop = pri->GetTriangleCount( ); // 3 == triangle edge count
		const s32* id_tbl = pri->GetBoneIndexTable( );
		const s32* idx = pri->GetTriangles();
		gfl::core::Vector* vert = this->GetVertices( );
		gfl::core::Vector* norm = this->GetNormals( );
		u8* col = this->GetColor( );

		for( s32 j = 0; j < loop; ++j ){
			for( s32 l = 0; l < 3; ++l, ++work_vert, ++work_norm ){ // 3 == triangle edge count
				const s32 id_val = idx[l];
//				gfl::core::Debug::PrintConsole( "[%d]\n",id_val );
				work_vert->SetZero( );
				work_norm->SetZero( );
				if(col){
					*work_col++ = *((u32*)&col[id_val<<2]);
				}
				gfl::core::Vector v, n;
				gfl::core::Matrix wmat;
				for( s32 k = 0; k < step; ++k ){
					const s32 idxx = id_val * step + k;
                    s32 bone_idx_tmp = bone_idx[idxx];
                    if(bone_idx_tmp < 0){ bone_idx_tmp = 0; }
                    const s32 b_id = id_tbl[bone_idx_tmp];
//					gfl::core::Debug::PrintConsole( "bid %d\n",b_id );
					
					if(idw){
						//wmat.Mul( mat[b_id] , *pBone[b_id].GetInvMatrix());
                      wmat = mat[b_id];
						wmat.Trans( &v, vert[id_val] );
						wmat.Apply( &n, norm[id_val] );

						f32 w = idw[idxx];
						work_vert->Add( v * w );
						work_norm->Add( n * w );
					}else{
						mat[b_id].Trans(work_vert,vert[id_val]);
						mat[b_id].Apply(work_norm,norm[id_val]);
					}
				}
				if(idv_flag){
					for( s32 t = 0; t < tex_max; ++t ){
						gfl::core::Vector2* vp = this->GetTextures( t );
						if(vp){
							work_tex[t]->Set( vp[id_val] );
						}
						++work_tex[t];
					}
				}
			}
			idv_cnt += 3;
			
#        if 0 // 
			glBegin( GL_TRIANGLES );
//				gfl::qt::Render::SetTriangleNormal( vp[0], vp[1], vp[2] );

			//				glNormal3fv( np[idx].Get() );
			glTexCoord2fv( tex[idx[0]].Get( ) );
			glVertex3fv( vp[0].Get( ) );

			//				glNormal3fv( np[idx].Get() );
			glTexCoord2fv( tex[idx[1]].Get( ) );
			glVertex3fv( vp[1].Get( ) );

			//				glNormal3fv( np[idx].Get() );
			glTexCoord2fv( tex[idx[2]].Get( ) );
			glVertex3fv( vp[2].Get( ) );
			
			glEnd( );
#        endif
			
			/*glNormal3d( 1.0, 0.0, 0.0 );

			for( int vNo = 0; vNo < 3; ++vNo )
			{
				glTexCoord2f( tex[idx[vNo]].GFL_VECTOR_X, tex[idx[vNo]].GFL_VECTOR_Y );
				glVertex3f( vp[vNo].GFL_VECTOR_X, vp[vNo].GFL_VECTOR_Y, vp[vNo].GFL_VECTOR_Z );			
			}*/
			
			idx += gfl::core::TXYZ;
		}
	}
//	exit(9);
//		gfl::core::Debug::PrintConsole( "idv_cnt %d\n",idv_cnt );

	if( idv_flag ){
		this->SetIndexCount( idv_cnt );
	}
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void gfl::cmdl::Shape::BoundingBox::Render()
{
	glPushMatrix();
	gfl::core::Matrix mat(m_Matrix);
	mat.Scale(m_Size);
	gfl::qt::GLWidget::GlMultMatrix( mat );
	glutSolidCube(1.0f);
	glPopMatrix();
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void gfl::cmdl::Model::RenderBoundingBox()
{
	const s32 shape_count = GetShapeCount( );
	Shape* shape = GetShape( );

	glDisable( GL_TEXTURE_2D );
	for( s32 i = 0; i < shape_count; ++i, ++shape ){
		shape->m_BoundingBox.Render();
	}
}
