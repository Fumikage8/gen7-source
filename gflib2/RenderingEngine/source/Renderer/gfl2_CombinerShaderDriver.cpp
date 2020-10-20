#if defined(GF_PLATFORM_WIN32)

#include <gfx/include/gfl2_GFGLUtil.h>
#include <math/include/gfl2_MathCommon.h>
#include <RenderingEngine/include/renderer/gfl2_CombinerShaderDriver.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_CombinerAttribute.h>
#include <RenderingEngine/include/renderer/gfl2_MeshDrawTag.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>

#if defined(NN_PLATFORM_CTR)
  #include <gfx/include/CTR/gfl2_CTRGL.h>
#elif defined(_WIN64)
	#include <gfx/include/WinOpenGL/gfl2_WinOpenGLPixelShader.h>
#endif

using namespace gfl2::gfx;
using namespace gfl2::renderingengine::scenegraph::resource;

namespace gfl2 { namespace renderingengine { namespace renderer { 

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  CombinerShaderDriverクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

/*CombinerShaderDriver* CombinerShaderDriver::Create()
{
  return Create( GLMemory::GetAllocator() );
}*/

CombinerShaderDriver* CombinerShaderDriver::Create( gfx::IGLAllocator* pAllocator )
{
  gfx::GLMemory::SetInstanceAllocator(pAllocator);
	CombinerShaderDriver* instance = new CombinerShaderDriver();
  gfx::GLMemory::ClearInstanceAllocator();
  return instance;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
CombinerShaderDriver::CombinerShaderDriver() :
	gfx::GLMemory(),
	IShaderDriver(),
	m_pVertexShader( NULL ),
	m_pGeometryShader(NULL),
	m_pPixelShader( NULL )
{
  m_VertexShaderVectorConstantsBuffer = GFGL::CreateConstantBuffer( this->GetInstanceAllocator(), VertexShaderVectorConstants::GetConstantBufferElement(), Usage::WriteOnly, Pool::Default );
	m_VertexShaderJointMatrixConstantsBuffer = GFGL::CreateConstantBuffer( this->GetInstanceAllocator(), VertexShaderJointMatrixConstants::GetConstantBufferElement(), Usage::WriteOnly, Pool::Default );
	m_VertexShaderBoolConstantsBuffer = GFGL::CreateConstantBuffer( this->GetInstanceAllocator(), VertexShaderBoolConstants::GetConstantBufferElement(), Usage::WriteOnly, Pool::Default );
	m_VertexShaderIntArrayConstantsBuffer = GFGL::CreateConstantBuffer( this->GetInstanceAllocator(), VertexShaderIntArrayConstants::GetConstantBufferElement(), Usage::WriteOnly, Pool::Default );

	m_GeometryShaderVectorConstantsBuffer = GFGL::CreateConstantBuffer( this->GetInstanceAllocator(), GeometryShaderVectorConstants::GetConstantBufferElement(), Usage::WriteOnly, Pool::Default );

	m_PixelShaderVectorConstantsBuffer = GFGL::CreateConstantBuffer( this->GetInstanceAllocator(), PixelShaderVectorConstants::GetConstantBufferElement(), Usage::WriteOnly, Pool::Default );
	m_PixelShaderFloatArrayConstantsBuffer = GFGL::CreateConstantBuffer( this->GetInstanceAllocator(), PixelShaderFloatArrayConstants::GetConstantBufferElement(), Usage::WriteOnly, Pool::Default );
	m_PixelShaderBoolConstantsBuffer = GFGL::CreateConstantBuffer( this->GetInstanceAllocator(), PixelShaderBoolConstants::GetConstantBufferElement(), Usage::WriteOnly, Pool::Default );
	m_PixelShaderBoolArrayConstantsBuffer = GFGL::CreateConstantBuffer( this->GetInstanceAllocator(), PixelShaderBoolArrayConstants::GetConstantBufferElement(), Usage::WriteOnly, Pool::Default );
	m_PixelShaderIntConstantsBuffer = GFGL::CreateConstantBuffer( this->GetInstanceAllocator(), PixelShaderIntConstants::GetConstantBufferElement(), Usage::WriteOnly, Pool::Default );
	m_PixelShaderIntArrayConstantsBuffer = GFGL::CreateConstantBuffer( this->GetInstanceAllocator(), PixelShaderIntArrayConstants::GetConstantBufferElement(), Usage::WriteOnly, Pool::Default );

  m_pColorShaderConstantsBuffer = GFGL::CreateConstantBuffer( this->GetInstanceAllocator(), ColorShaderConstants::GetConstantBufferElement(), Usage::WriteOnly, Pool::Default );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
CombinerShaderDriver::~CombinerShaderDriver()
{
	GFX_SAFE_DELETE( m_VertexShaderVectorConstantsBuffer );
	GFX_SAFE_DELETE( m_VertexShaderJointMatrixConstantsBuffer );
	GFX_SAFE_DELETE( m_VertexShaderBoolConstantsBuffer );
	GFX_SAFE_DELETE( m_VertexShaderIntArrayConstantsBuffer );

	GFX_SAFE_DELETE( m_GeometryShaderVectorConstantsBuffer );

	GFX_SAFE_DELETE( m_PixelShaderVectorConstantsBuffer );
	GFX_SAFE_DELETE( m_PixelShaderFloatArrayConstantsBuffer );
	GFX_SAFE_DELETE( m_PixelShaderBoolConstantsBuffer );
	GFX_SAFE_DELETE( m_PixelShaderBoolArrayConstantsBuffer );
	GFX_SAFE_DELETE( m_PixelShaderIntConstantsBuffer );
	GFX_SAFE_DELETE( m_PixelShaderIntArrayConstantsBuffer );

  GFX_SAFE_DELETE( m_pColorShaderConstantsBuffer );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
b32 CombinerShaderDriver::Apply( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag ) const
{
  if (pMeshDrawTag == NULL)
  {
    GFL_ASSERT(0);
    return false;
  }
#if defined(GF_PLATFORM_CTR)

#elif defined(GF_PLATFORM_WIN32)
	VertexShaderVectorConstants					vertexShaderVectorConstants;
	VertexShaderJointMatrixConstants		vertexShaderJointMatrixConstants;
	VertexShaderBoolConstants						vertexShaderBoolConstants;
	VertexShaderIntArrayConstants				vertexShaderIntArrayConstants;
	GeometryShaderVectorConstants				geometryShaderVectorConstants;
  ColorShaderConstants                colorShaderConstants;
	PixelShaderVectorConstants					pixelShaderVectorConstants;
	PixelShaderFloatArrayConstants			pixelShaderFloatArrayConstants;
	PixelShaderBoolConstants						pixelShaderBoolConstants;
	PixelShaderBoolArrayConstants				pixelShaderBoolArrayConstants;
	PixelShaderIntConstants							pixelShaderIntConstants;
	PixelShaderIntArrayConstants				pixelShaderIntArrayConstants;

	{// ライトの設定は複雑なので, 頂点・ピクセルシェーダ両方の設定をここで行う
		math::Vector3                  worldPt;

    math::Matrix34 objMatrix = *rRenderState.GetWorldMatrix();
		math::Matrix34 objInvMatrix = objMatrix.Inverse();
		math::Vector3						objSpaceCameraPosition;

		// ビュー逆行列から視点の位置を求める
		math::Matrix34 mtxInvView = rRenderState.GetViewMatrix()->Inverse();
		worldPt = mtxInvView * math::Vector3(0.0f, 0.0f, 0.0f);

		objSpaceCameraPosition = objInvMatrix * worldPt;
		
		vertexShaderVectorConstants.cVsCameraPosition = worldPt;
		vertexShaderVectorConstants.cVsObjSpaceCameraPosition = objSpaceCameraPosition;
		pixelShaderVectorConstants.cPsObjSpaceCameraPosition = objSpaceCameraPosition;

				//-------- ライト情報 --------------
		u32 lightNum = rRenderState.GetLightNum();
		pixelShaderIntConstants.cPsObjSpaceLightCount = 0;
		// Ambientは初期値を黒としておく
		pixelShaderVectorConstants.cPsAmbientLightColor = math::Vector4(0.0f, 0.0f, 0.0f, 0.0f );
		vertexShaderVectorConstants.cVsAmbientLightColor.Set(0, 0, 0, 0);

		for( u32 i = 0; i < lightNum; ++i )
		{
			const scenegraph::instance::LightNode* lightNode = rRenderState.GetLightNode(i);

			u32				lightIndex = pixelShaderIntConstants.cPsObjSpaceLightCount;

			switch (lightNode->GetLightType())
			{
			case LightType::Directional:
				{
					pixelShaderVectorConstants.cPsObjSpaceLightPos[lightIndex] = lightNode->GetWorldDirection();
					pixelShaderVectorConstants.cPsObjSpaceLightPos[lightIndex] *= -1000000.0f;//遠くに配置

					pixelShaderVectorConstants.cPsObjSpaceLightVec[lightIndex] = lightNode->GetWorldDirection();
					pixelShaderVectorConstants.cPsObjSpaceLightColor[lightIndex] = lightNode->GetColor();
					vertexShaderVectorConstants.cVsWorldLightVec = lightNode->GetWorldDirection();
					vertexShaderVectorConstants.cVsWorldLightColor = lightNode->GetColor();

					pixelShaderVectorConstants.cPsObjSpaceLightPos[lightIndex].w = 0.0f;
					pixelShaderVectorConstants.cPsObjSpaceLightVec[lightIndex].w = 0.0f;

					++pixelShaderIntConstants.cPsObjSpaceLightCount;
				}
				break;
			case LightType::Ambient:
				{
					pixelShaderVectorConstants.cPsAmbientLightColor = lightNode->GetColor();
					vertexShaderVectorConstants.cVsAmbientLightColor = lightNode->GetColor();
				}
				break;
			case LightType::Point:
				{
					pixelShaderVectorConstants.cPsObjSpaceLightPos[lightIndex] = objInvMatrix * lightNode->GetWorldPosition();
					pixelShaderVectorConstants.cPsObjSpaceLightColor[lightIndex] = lightNode->GetColor();

					f32		intensity = lightNode->GetIntensity();
					intensity = pow( intensity, 2 );
					pixelShaderVectorConstants.cPsObjSpaceLightPos[lightIndex].w = intensity;

					++pixelShaderIntConstants.cPsObjSpaceLightCount;
				}
				break;
			case LightType::Spot:
				{
					pixelShaderVectorConstants.cPsObjSpaceLightPos[lightIndex] = objInvMatrix * lightNode->GetWorldPosition();
					pixelShaderVectorConstants.cPsObjSpaceLightVec[lightIndex] = objInvMatrix * -lightNode->GetWorldDirection();
					pixelShaderVectorConstants.cPsObjSpaceLightVec[lightIndex] = pixelShaderVectorConstants.cPsObjSpaceLightVec[lightIndex].Normalize();
					pixelShaderVectorConstants.cPsObjSpaceLightColor[lightIndex] = lightNode->GetColor();

					f32		intensity = lightNode->GetIntensity();
					intensity = pow( intensity, 2 );
					pixelShaderVectorConstants.cPsObjSpaceLightPos[lightIndex].w = intensity;
					pixelShaderVectorConstants.cPsObjSpaceLightVec[lightIndex].w = 1.0f;

					++pixelShaderIntConstants.cPsObjSpaceLightCount;
				}
				break;

			default:
				GFL_ASSERT(0);
				break;
			}
		}
	}

	vertexShaderVectorConstants.Set( rRenderState, m_pMaterial, pMeshDrawTag );
	vertexShaderJointMatrixConstants.Set( pMeshDrawTag, pMeshDrawTag->GetFaceData(), rRenderState.GetJointInstanceNode(), rRenderState.GetJointNum() );
	vertexShaderBoolConstants.Set( rRenderState, m_pMaterial );
	vertexShaderIntArrayConstants.Set( rRenderState, m_pMaterial );
	if ( m_pGeometryShader )
		geometryShaderVectorConstants.Set( rRenderState, m_pMaterial, pMeshDrawTag );
  colorShaderConstants.Set( rRenderState, m_pMaterial );
	pixelShaderVectorConstants.Set( rRenderState, m_pMaterial );
	pixelShaderFloatArrayConstants.Set( rRenderState, m_pMaterial );
	pixelShaderBoolConstants.Set( rRenderState, m_pMaterial );
	pixelShaderBoolArrayConstants.Set( rRenderState, m_pMaterial );
	pixelShaderIntConstants.Set( rRenderState, m_pMaterial );
	pixelShaderIntArrayConstants.Set( rRenderState, m_pMaterial );


	m_VertexShaderVectorConstantsBuffer->Update( &vertexShaderVectorConstants, sizeof(VertexShaderVectorConstants) );
	m_VertexShaderJointMatrixConstantsBuffer->Update( &vertexShaderJointMatrixConstants, sizeof(VertexShaderJointMatrixConstants) );
	m_VertexShaderBoolConstantsBuffer->Update( &vertexShaderBoolConstants, sizeof(VertexShaderBoolConstants) );
	m_VertexShaderIntArrayConstantsBuffer->Update( &vertexShaderIntArrayConstants, sizeof(VertexShaderIntArrayConstants) );
	if ( m_pGeometryShader )
		m_GeometryShaderVectorConstantsBuffer->Update( &geometryShaderVectorConstants, sizeof(GeometryShaderVectorConstants) );
	m_PixelShaderVectorConstantsBuffer->Update( &pixelShaderVectorConstants, sizeof(PixelShaderVectorConstants) );
	m_PixelShaderFloatArrayConstantsBuffer->Update( &pixelShaderFloatArrayConstants, sizeof(PixelShaderFloatArrayConstants) );
	m_PixelShaderBoolConstantsBuffer->Update( &pixelShaderBoolConstants, sizeof(PixelShaderBoolConstants) );
	m_PixelShaderBoolArrayConstantsBuffer->Update( &pixelShaderBoolArrayConstants, sizeof(PixelShaderBoolArrayConstants) );
	m_PixelShaderIntConstantsBuffer->Update( &pixelShaderIntConstants, sizeof(PixelShaderIntConstants) );
	m_PixelShaderIntArrayConstantsBuffer->Update( &pixelShaderIntArrayConstants, sizeof(PixelShaderIntArrayConstants) );
  m_pColorShaderConstantsBuffer->Update(&colorShaderConstants, sizeof(ColorShaderConstants));

	GFGL::SetBlendState( m_pMaterial->GetBlendStateObject() );
	GFGL::SetRasterizerState( m_pMaterial->GetRasterizerStateObject() );
	GFGL::SetDepthStencilState( m_pMaterial->GetDepthStencilStateObject() );

	GFGL::SetVertexShader( m_pVertexShader->GetVertexShader() );
	GFGL::SetVertexShaderConstant( 0, m_VertexShaderVectorConstantsBuffer );
	GFGL::SetVertexShaderConstant( 1, m_VertexShaderJointMatrixConstantsBuffer );
	GFGL::SetVertexShaderConstant( 3, m_VertexShaderBoolConstantsBuffer );
	GFGL::SetVertexShaderConstant( 4, m_VertexShaderIntArrayConstantsBuffer );

	GFGL::SetGeometryShader( m_pGeometryShader ? m_pGeometryShader->GetGeometryShader() : NULL );
	GFGL::SetGeometryShaderConstant( 5, m_GeometryShaderVectorConstantsBuffer );

	GFGL::SetPixelShader( m_pPixelShader->GetFragmentShader() );
	GFGL::SetPixelShaderConstant(7, m_pColorShaderConstantsBuffer);
	GFGL::SetPixelShaderConstant( 8, m_PixelShaderVectorConstantsBuffer );
	GFGL::SetPixelShaderConstant( 9, m_PixelShaderFloatArrayConstantsBuffer );
	GFGL::SetPixelShaderConstant( 10, m_PixelShaderBoolConstantsBuffer );
	GFGL::SetPixelShaderConstant( 11, m_PixelShaderBoolArrayConstantsBuffer );
	GFGL::SetPixelShaderConstant( 12, m_PixelShaderIntConstantsBuffer );
	GFGL::SetPixelShaderConstant( 13, m_PixelShaderIntArrayConstantsBuffer );
#endif
	return true;
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void CombinerShaderDriver::VertexShaderVectorConstants::Set( const RenderState& rRenderState, const scenegraph::instance::MaterialInstanceNode *pMaterial, const MeshDrawTag* pMeshDrawTag )
{
	cVsProjectionMatrix = *rRenderState.GetProjectionMatrix();
	
  bool hasJoint = rRenderState.GetJointNum() != 0;

	cVsModelMatrix = (hasJoint)
    ? math::Matrix44::GetIdentity()
    : (*rRenderState.GetWorldMatrix());

	cVsViewMatrix = (*rRenderState.GetViewMatrix());
	cVsModelViewMatrix = cVsViewMatrix * (*rRenderState.GetWorldMatrix());

	const MaterialResourceNode::UserData	&rUserData = pMaterial->GetMaterialResourceNode()->GetUserData();

	cVsBlendMeshWeight.Set( rRenderState.GetBlendMeshWeight( 0 ),
													rRenderState.GetBlendMeshWeight( 1 ),
													rRenderState.GetBlendMeshWeight( 2 ),
													rRenderState.GetBlendMeshWeight( 3 ) );

	// リムライトは, RimPow, RimScale, PhongPow, PhongScaleの順に格納
	cVsRimParam = math::Vector4(	rUserData.m_RimPow,
															rUserData.m_RimScale,
															rUserData.m_PhongPow,
															rUserData.m_PhongScale);

	if ( pMeshDrawTag )
	{// この値はメガゲンガー以外で使われない
		math::Vector3			scale = pMeshDrawTag->GetWorldMatrix()->GetElemScaling();
		cVsShaderParam = math::Vector4(rUserData.m_ShaderParam0, rUserData.m_ShaderParam1, rUserData.m_ShaderParam2, rUserData.m_ShaderParam3 * scale.x);
	}

	if ( rUserData.m_VertexShaderType == VertexShaderType::FillScrean )
	{
		cVsProjectionMatrix = math::Matrix44::GetIdentity();
		cVsModelViewMatrix = math::Matrix44::GetIdentity();
		cVsModelMatrix = math::Matrix44::GetIdentity();
		cVsViewMatrix = math::Matrix44::GetIdentity();
	}
}

void CombinerShaderDriver::VertexShaderJointMatrixConstants::DumpModelData( const MeshDrawTag* pMeshDrawTag, const scenegraph::resource::MeshResourceNode::FaceData *pFace, const scenegraph::instance::JointInstanceNode* pJointNode, u32 size )
{
  const scenegraph::resource::MaterialResourceNode*   pMaterual = pMeshDrawTag->GetMaterialResourceNode();
  const c8*                                           matName = pMaterual->GetName();
  const c8*                                           matNodeName = pMaterual->GetNodeName();
  const c8*                                           pVsName = pMaterual->GetShaderName( gfx::ShaderType::Vertex );
  const c8*                                           pFsName = pMaterual->GetShaderName( gfx::ShaderType::Pixel );

  {
    const c8*                                   pModelName = NULL;

    GFL_PRINT( "Data Error!!!!!\n" );

    GFL_PRINT( "Material Name = %s : %s : %s\n", matName, pVsName, pFsName );

    for( u32 jointNo = 0; jointNo < pFace->m_JointNum; ++jointNo )
    {
      const c8*                                   pParentJointName = pJointNode[jointNo].GetParentName();
      const c8*                                   pJointName = pJointNode[jointNo].GetName();
      scenegraph::DagNode*                        pParentNode = (scenegraph::DagNode*)( pJointNode[jointNo].GetParent() );
      scenegraph::instance::ModelInstanceNode     *pModelInstance = pParentNode->SafeCast<scenegraph::instance::ModelInstanceNode>();
          
      if ( pModelInstance )
      {
        const scenegraph::resource::GfMdlResourceNode*    pModelResource = pModelInstance->GetModelResourceNode();
        for( s32 meshNo = 0; meshNo < pModelResource->GetMeshNum(); ++meshNo )
        {
          const c8*   pMeshName = pModelResource->GetMeshName( meshNo );

          GFL_PRINT( "mesh[%d]%s\n", meshNo, pMeshName );
        }
        break;
      }
    }

    for( u32 jointNo = 0; jointNo < pFace->m_JointNum; ++jointNo )
    {
      const c8*                                   pParentJointName = pJointNode[jointNo].GetParentName();
      const c8*                                   pJointName = pJointNode[jointNo].GetName();

      GFL_PRINT( "Joint %s -> %s\n", pParentJointName, pJointName );
    }
  }
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void CombinerShaderDriver::VertexShaderJointMatrixConstants::Set( const MeshDrawTag* pMeshDrawTag, const scenegraph::resource::MeshResourceNode::FaceData *pFace, const scenegraph::instance::JointInstanceNode* pNode, u32 size )
{
	u32			pushCnt = 0;

	if ( pNode )
	{
		if ( pFace->m_JointNum == 0xFF )
		{
			for( u32 jointNo = 0; jointNo < size; ++jointNo )
			{
				if ( pNode[jointNo].IsNeedRendering() )
				{
          cVsJointMatrix[jointNo] = pNode[jointNo].GetSkinningMatrix();
				}
			}
		}
		else
		{
			for( u32 i = 0; i < pFace->m_JointNum; ++i )
			{
				u32			jointNo = pFace->m_pJointList[i];
        b32     isNeedRendering = pNode[jointNo].IsNeedRendering();

        #if GFL_DEBUG
        if ( isNeedRendering == false )
          this->DumpModelData( pMeshDrawTag, pFace, &pNode[jointNo], size );
        #endif
				GFL_ASSERT( isNeedRendering );

        cVsJointMatrix[pushCnt] = pNode[jointNo].GetSkinningMatrix();
				++pushCnt;
			}
		}
	}

	cVsSkiningEnable = size ? true : false;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void CombinerShaderDriver::VertexShaderBoolConstants::Set( const RenderState& rRenderState, const scenegraph::instance::MaterialInstanceNode *pMaterial )
{
	const MaterialResourceNode::AttributeParam& rAttributeParam = *pMaterial->GetAttributeParam();
	const MaterialResourceNode::UserData& rUserData = pMaterial->GetMaterialResourceNode()->GetUserData();

	cVsSceneContrastEnable = false;	// この値はMaya上のレンダリングでも使われていないのでfalse
	cVsVertexColorEnable = ( rUserData.m_VertexShaderType != VertexShaderType::Kusa );
	cVsHalfVecLightEnable = (rRenderState.GetLightNum() == 0) ? true : false;
	cVsLightingEnable = rAttributeParam.m_VsLightingEnable;

  bool isPokemonShader = (rAttributeParam.m_IsPokemonShader != 0);
  if (rUserData.m_VertexShaderType.IsPokemonShader() ||
    rUserData.m_VertexShaderType == VertexShaderType::BattleChar)
  {
    isPokemonShader = (rUserData.m_VertexShaderType != VertexShaderType::PokeFire);
  }
  else if (rUserData.m_VertexShaderType == VertexShaderType::VsShading)
  {
    isPokemonShader = true;
  }
	cIsPokemonShader = isPokemonShader;

	cIsMegaGangarShader =( rUserData.m_VertexShaderType == VertexShaderType::MegaGangar );
	cUseObjectSpaceNormalMap = rAttributeParam.m_UseObjectSpaceNormalMap;
	cRotateSphereMapEnable = (rUserData.m_VertexShaderType == VertexShaderType::RotateSphereMap);

	cVsFog = (rUserData.m_VertexShaderType == VertexShaderType::VsFog);
	cVsNormalToUv = (rUserData.m_VertexShaderType == VertexShaderType::NormalToUv);
	cIsMegaYamiramiShader = (rUserData.m_VertexShaderType == VertexShaderType::MegaYamirami);
	cIsReflectShader = (rUserData.m_VertexShaderType == VertexShaderType::Reflect);
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void CombinerShaderDriver::VertexShaderIntArrayConstants::Set( const RenderState& rRenderState, const scenegraph::instance::MaterialInstanceNode *pMaterial )
{
	for( u32 texNo = 0; texNo < CombinerShaderDriver::TextureMax; ++texNo )
	{
		cMappingType[texNo] = 0;
		cUvSetNo[texNo] = 0;
	}

	u32 texNum = pMaterial->GetTextureNum();
	for( u32 texNo = 0; texNo < texNum; ++texNo )
	{
		const MaterialResourceNode::TextureInfo *pInfoOverride = rRenderState.GetTextureInfoOverride( texNo );
		const MaterialResourceNode::TextureInfo *pInfo = pMaterial->GetTextureInfo( texNo );

		if (pInfo->m_SlotNo >= CombinerShaderDriver::UvMax)
		{// スロット番号が3以上だったらカラーテクスチャ以外の用途なので除外
			continue;
		}

		const MaterialResourceNode::TextureInfo::Attribute			*pAttr = &pInfo->m_Attribute;

		if ( pInfoOverride )
			pAttr = &pInfoOverride->m_Attribute;

		cMappingType[ pInfo->m_SlotNo ] = pAttr->m_MappingType;
		cUvSetNo[ pInfo->m_SlotNo ] = pAttr->m_UvSetNo;

		if ( pAttr->m_MappingType != 0 )
		{
			cMappingType[ pInfo->m_SlotNo ] = pAttr->m_MappingType;
		}
	}
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void CombinerShaderDriver::GeometryShaderVectorConstants::Set( const RenderState& rRenderState, const scenegraph::instance::MaterialInstanceNode *pMaterial, const MeshDrawTag* pMeshDrawTag )
{
	if ( pMeshDrawTag )
	{
		cGsProjectionMatrix = *rRenderState.GetProjectionMatrix();
    cGsViewMatrix = *rRenderState.GetViewMatrix();

		const scenegraph::resource::MaterialResourceNode::UserData& rUserData = pMaterial->GetUserData();

		math::Vector3			scale = pMeshDrawTag->GetWorldMatrix()->GetElemScaling();
		cGsShaderParam.Set( rUserData.m_ShaderParam0 * scale.x, rUserData.m_ShaderParam1, rUserData.m_ShaderParam2, rUserData.m_ShaderParam3 );
	}
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void CombinerShaderDriver::PixelShaderVectorConstants::Set( const RenderState& rRenderState, const scenegraph::instance::MaterialInstanceNode *pMaterial )
{
	const MaterialResourceNode::AttributeParam	&rAttributeParam = *pMaterial->GetAttributeParam();

	//-----カラー-------
  cEmission = rAttributeParam.m_MaterialColor.m_Emission.GetColor();
	cAmbient = rAttributeParam.m_MaterialColor.m_Ambient.GetColor();
	cDiffuse = rAttributeParam.m_MaterialColor.m_Diffuse.GetColor();
	cSpecular0 = rAttributeParam.m_MaterialColor.m_Specular0.GetColor();
	cSpecular1 = rAttributeParam.m_MaterialColor.m_Specular1.GetColor();

	for( u32 i = 0; i < CombinerStageMax; ++i )
	{
    cConsntantColor[i] = rAttributeParam.m_ConstantColor[i].GetColor();
	}

  cBufferColor = rAttributeParam.m_EtcColor.m_BufferColor.GetColor();

	//-------- 参照テーブル情報 --------------
	static const math::Vector4   cookTorrance(1.5f, 1.0f, 0.25f, 0.0f);
	cPsCookTorranceParam = cookTorrance;
	cFsScreanSize = math::Vector4(0.0f, 0.0f, 0.0f, 0.0f );

	//-------- Fog(値はマテリアルアトリビュートからではなく外部から受け取る) --------------
	cPsFogColor  = pMaterial->GetFogColor();
	cPsFogParam  = pMaterial->GetFogParam();
	cPsFogType   = pMaterial->GetFogType();
	if( !rAttributeParam.m_FogEnable )
	{
		cPsFogType = 0;
	}

	f32				stencilValue = static_cast<f32>( pMaterial->GetDepthStencilStateObject()->GetStencilReference() );
	cFsParam[0].x = stencilValue / 255.0f;
	cFsParam[0].y = (rAttributeParam.m_PsLightingEnable) ? 1.0f : 0.0f;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void CombinerShaderDriver::PixelShaderFloatArrayConstants::Set( const RenderState& rRenderState, const scenegraph::instance::MaterialInstanceNode *pMaterial )
{
	//-------- ライト情報 --------------
	for( u32 lightNo = 0; lightNo < rRenderState.GetLightNum(); ++lightNo )
	{
		const scenegraph::instance::LightNode* lightNode = rRenderState.GetLightNode(lightNo);
		cPsObjSpaceLightConeAngle[lightNo] = lightNode->GetConeAngle();
	}

	const MaterialResourceNode::AttributeParam	&rAttributeParam = *pMaterial->GetAttributeParam();
	//-------- 参照テーブル情報 --------------
	for( u32 type = 0; type < LookUpTableType::NumberOf; ++type )
	{
		cLutScale[type] = rAttributeParam.m_LookUpTable[type].m_LutScale;
	}

	//-----テクスチャ-------
	for( u32 texNo = 0; texNo < CombinerShaderDriver::TextureMax; ++texNo )
	{
		cScaleU[texNo] = 0.0f;
		cScaleV[texNo] = 0.0f;
		cRotate[texNo] = 0.0f;
		cTranslateU[texNo] = 0.0f;
		cTranslateV[texNo] = 0.0f;
	}

	u32 texNum = pMaterial->GetTextureNum();
	for( u32 texNo = 0; texNo < texNum; ++texNo )
	{
		const MaterialResourceNode::TextureInfo *pInfoOverride = rRenderState.GetTextureInfoOverride( texNo );
		const MaterialResourceNode::TextureInfo *pInfo = pMaterial->GetTextureInfo( texNo );

		if (pInfo->m_SlotNo >= CombinerShaderDriver::UvMax)
		{// スロット番号が3以上だったらカラーテクスチャ以外の用途なので除外
			continue;
		}

		const MaterialResourceNode::TextureInfo::Attribute			*pAttr = &pInfo->m_Attribute;

		if ( pInfoOverride )
			pAttr = &pInfoOverride->m_Attribute;

		cScaleU[pInfo->m_SlotNo] = pAttr->m_ScaleU;
		cScaleV[pInfo->m_SlotNo] = pAttr->m_ScaleV;
		cRotate[pInfo->m_SlotNo] = pAttr->m_Rotate;
		cTranslateU[pInfo->m_SlotNo] = pAttr->m_TranslateU;
    cTranslateV[pInfo->m_SlotNo] = pAttr->m_TranslateV;
	}

	//-----フラグメント-------
	for( u32 i = 0; i < CombinerAttribute::CombinerMax; ++i )
	{
		for( u32 type = 0; type < CombinerColorType::NumberOf; ++type )
		{
			cScale[type][i] = rAttributeParam.m_TextureCombinerAttribute[type][i].m_Scale;
		}
	}

  //-----アルファテスト参照値-----
  cAlphaTestRefValue[0] = pMaterial->GetBlendStateObject()->GetAlphaRef();
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void CombinerShaderDriver::PixelShaderBoolConstants::Set( const RenderState& rRenderState, const scenegraph::instance::MaterialInstanceNode *pMaterial )
{
	const MaterialResourceNode::AttributeParam	&rAttributeParam = *pMaterial->GetAttributeParam();
	const MaterialResourceNode::UserData				&rUserData = pMaterial->GetMaterialResourceNode()->GetUserData();

	cPrimaryFresnelEnable = rAttributeParam.m_PrimaryFresnelEnable;
	cSecondaryFresnelEnable = rAttributeParam.m_SecondaryFresnelEnable;
	cSpecularColorClamp = rAttributeParam.m_SpecularColorClamp;
	cCookTorrance0Enable = rAttributeParam.m_CookTorrance0Enable;
	cCookTorrance1Enable = rAttributeParam.m_CookTorrance1Enable;

	cZRecastEnable = rAttributeParam.m_ZRecastEnable;

	cUseShadowTexture = rAttributeParam.m_UseShadowTexture;
	//cHighLightTextureEnable = rAttributeParam.m_HighLightTextureEnable;
	cHighLightTextureEnable = false;

  bool isPokemonShader = (rAttributeParam.m_IsPokemonShader != 0);
  if (rUserData.m_VertexShaderType.IsPokemonShader() ||
    rUserData.m_VertexShaderType == VertexShaderType::BattleChar)
  {
    isPokemonShader = true;
  }
  else if (rUserData.m_VertexShaderType == VertexShaderType::VsShading)
  {
    isPokemonShader = false;
  }

	cFsIsPokemonShader = isPokemonShader;
  cBattleChara20Enable = rUserData.m_VertexShaderType == (VertexShaderType::BattleFieldShader20);

	// Maya上のレンダラーでも使われていない値なのでfalse
	cFsRenderNormalMap = false;
	cFsDisableNormalColor = false;
	cFsBorderLineMapSampling = false;
	cBlurMask = false;
	cBlurScale = false;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void CombinerShaderDriver::PixelShaderBoolArrayConstants::Set( const RenderState& rRenderState, const scenegraph::instance::MaterialInstanceNode *pMaterial )
{
	const MaterialResourceNode::AttributeParam	&rAttributeParam = *pMaterial->GetAttributeParam();
	//-------- ライト情報 --------------
	for( u32 i = 0; i < rRenderState.GetLightNum(); ++i )
	{
		const scenegraph::instance::LightNode* lightNode = rRenderState.GetLightNode(i);
		cPsIsSpotLight[i] = ( lightNode->GetLightType() == LightType::Spot ) ? true : false;
	}

	//-------- 参照テーブル情報 --------------
	for( u32 i = 0; i < LookUpTableType::NumberOf; ++i )
	{
		cLookUpTableEnable[i] = rAttributeParam.m_LookUpTableEnable[i];
	}

	for( u32 texNo = 0; texNo < CombinerShaderDriver::TextureMax; ++texNo )
	{
		cTextureEnable[texNo] = false;
		cTexture2D[texNo] = false;
	}

	//-----テクスチャ-------
	// 0, 1, 2はカラーマップとして, 3はプロシージャルテクスチャとして割り当てられているが,
	// 3のプロシージャルは使われていないので[3]は未使用にしておく
	u32 texNum = pMaterial->GetTextureNum();
	for( u32 texNo = 0; texNo < texNum; ++texNo )
	{
		const MaterialResourceNode::TextureInfo &rInfo = *pMaterial->GetTextureInfo( texNo );
		if (rInfo.m_SlotNo >= CombinerShaderDriver::UvMax)
		{// スロット番号が3以上だったらカラーテクスチャ以外の用途なので除外
			continue;
		}

		
		cTextureEnable[rInfo.m_SlotNo] = true;
		cTexture2D[rInfo.m_SlotNo] = true;
	}

	//-----フラグメント-------
	for( u32 i = 0; i < CombinerAttribute::CombinerMax; ++i )
	{
		for( u32 type = 0; type < CombinerColorType::NumberOf; ++type )
		{
			cOutPutBuffer[type][i] = rAttributeParam.m_TextureCombinerAttribute[type][i].m_OutPutBuffer;
		}
	}
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void CombinerShaderDriver::PixelShaderIntConstants::Set( const RenderState& rRenderState, const scenegraph::instance::MaterialInstanceNode *pMaterial )
{
	const MaterialResourceNode::AttributeParam	&rAttributeParam = *pMaterial->GetAttributeParam();
	//-----テクスチャ-------
	cBumpMapNo = rAttributeParam.m_BumpMapNo;
	cBumpMapType = rAttributeParam.m_BumpMapType;
	cTangentNo = 0;

	if ( rAttributeParam.m_BumpMapNo >= 0 )
	{
		cTangentNo = rAttributeParam.m_TextureAttribute[ rAttributeParam.m_BumpMapNo ].m_UvSetNo;
	}
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void CombinerShaderDriver::PixelShaderIntArrayConstants::Set( const RenderState& rRenderState, const scenegraph::instance::MaterialInstanceNode *pMaterial )
{
	const MaterialResourceNode::AttributeParam	&rAttributeParam = *pMaterial->GetAttributeParam();
	//-------- 参照テーブル情報 --------------
	for( u32 type = 0; type < LookUpTableType::NumberOf; ++type )
	{
		cLutInputAngle[type] = rAttributeParam.m_LookUpTable[type].m_LutInputAngle;
	}

	//-----フラグメント-------
	for( u32 i = 0; i < CombinerAttribute::CombinerMax; ++i )
	{
		for( u32 type = 0; type < CombinerColorType::NumberOf; ++type )
		{
			cMathFormula[type][i] = rAttributeParam.m_TextureCombinerAttribute[type][i].m_MathFormula;

			for( u32 i2 = 0; i2 < InputSourceType::NumberOf; ++i2 )
			{
				cInputSource[type][i][i2] = rAttributeParam.m_TextureCombinerAttribute[type][i].m_InputSource[i2];
				cSourceOperand[type][i][i2] = rAttributeParam.m_TextureCombinerAttribute[type][i].m_Operand[i2];
			}
		}
		cTextureCombinerConstant[i] = rAttributeParam.m_TextureCombinerConstant[i];
	}
}

}}}

#endif