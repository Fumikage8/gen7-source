//=============================================================================
/**
 * @file    gfl2_ModelInstanceNode.cpp
 * @brief   ModelInstanceNodeクラス（各種リソース、インスタンスノードより構成されるモデルインスタンスノード。ジョイントインスタンスを持つ。）
 * @author  ishiguro_masateru
 * @date    2014
 *
 *  2016.4.11   ishiguro  SetOptionalRenderKey関数追加
 *  2016.4.8    ishiguro  CopyMaterialDepthStencilStateToStateObject関数追加
 *
 */
//=============================================================================
#include <gfx/include/gfl2_GFGL.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <RenderingEngine/include/Renderer/gfl2_CombinerShaderDriver.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_CombinerAttribute.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <debug/include/gfl2_Assert.h>

using namespace gfl2::gfx;

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  ModelInstanceNodeクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
ModelInstanceNode::ModelInstanceNode(ModelInstanceNode* pJointReferenceModel) : NodeTrait<ModelInstanceNode, DrawableNode>(), renderer::DrawTag::IDrawListener(),
	m_MaterialResourceNum( 0 ),
	m_TextureResourceNum( 0 ),
	m_ShaderResourceNum( 0 ),
	m_MeshResourceNum( 0 ),
	m_JointNum( 0 ),
	m_pModelResourceNode( NULL ),
	m_pMaterialResourceNodeList( NULL ),
	m_pTextureResourceNodeList( NULL ),
  m_pShaderResourceNodeList( NULL ),	
	m_pMeshResourceNodeList( NULL ),
	m_pMaterialInstanceNodeList( NULL ),
	m_pMeshLinkDataList(NULL),
	m_JointInstanceNodeList( NULL ),
  m_pMotionConfigInstanceNode(NULL),
  m_MeshDrawTagListNum(0),
	m_pMeshDrawTagList(NULL),
	m_fPreDraw( false ),
	m_GpuReferenceCnt(0),
	m_SortPriority( s_DefaultPriority ),
  m_RenderKeyType(0),
  m_pJointReferenceModel(pJointReferenceModel)
{

}

ModelInstanceNode::~ModelInstanceNode()
{
	GFL_ASSERT( m_GpuReferenceCnt == 0 );//まだ描画中の可能性あり

	if ( m_pShaderResourceNodeList )
		this->GetInstanceAllocator()->SystemMemoryFree( m_pShaderResourceNodeList );
	if ( m_pMaterialResourceNodeList )
		this->GetInstanceAllocator()->SystemMemoryFree( m_pMaterialResourceNodeList );
	if ( m_pTextureResourceNodeList )
		this->GetInstanceAllocator()->SystemMemoryFree( m_pTextureResourceNodeList );
	if ( m_pMeshResourceNodeList )
		this->GetInstanceAllocator()->SystemMemoryFree( m_pMeshResourceNodeList );

	if ( m_pMeshDrawTagList )
	{
		delete[] m_pMeshDrawTagList;
		m_pMeshDrawTagList = NULL;
	}

	if ( m_pMaterialInstanceNodeList )
	{
		delete[] m_pMaterialInstanceNodeList;
		m_pMaterialInstanceNodeList = NULL;
	}

	if ( m_pMeshLinkDataList )
	{
		delete[] m_pMeshLinkDataList;
		m_pMeshLinkDataList = NULL;
	}

  for( u32 i = 0; i < m_JointNum; ++i )
  {
    // 2重開放になってしまうので削除する前にばらしておく
    JointInstanceNode* pJoint = &m_JointInstanceNodeList[i];
    pJoint->RemoveFromParent();
  }
  m_JointNum = 0;
  if ( m_JointInstanceNodeList )
  {
    delete[] m_JointInstanceNodeList;
    m_JointInstanceNodeList = NULL;
  }

  if (m_pMotionConfigInstanceNode)
  {
    delete m_pMotionConfigInstanceNode;
    m_pMotionConfigInstanceNode = NULL;
  }
}

void ModelInstanceNode::SetMaterialResourceNode( const resource::MaterialResourceNode **pMaterialResourceNodeList, u32 size )
{
	GFL_ASSERT(pMaterialResourceNodeList);
	if ( m_pMaterialResourceNodeList )
		this->GetInstanceAllocator()->SystemMemoryFree( m_pMaterialResourceNodeList );

	m_MaterialResourceNum = size;
	if ( m_MaterialResourceNum )
		m_pMaterialResourceNodeList = reinterpret_cast<const resource::MaterialResourceNode**>( this->GetInstanceAllocator()->SystemMemoryMalloc( sizeof(resource::MaterialResourceNode*) * m_MaterialResourceNum ) );

	for( u32 i = 0; i < m_MaterialResourceNum; ++i )
	{
		m_pMaterialResourceNodeList[i] = pMaterialResourceNodeList[i];
	}
}

void ModelInstanceNode::SetTextureResourceNode( const resource::TextureResourceNode **pTextureResourceNodeList, u32 size )
{
	GFL_ASSERT(pTextureResourceNodeList);
	if ( m_pTextureResourceNodeList )
		this->GetInstanceAllocator()->SystemMemoryFree( m_pTextureResourceNodeList );

	m_TextureResourceNum = size;
	if ( m_TextureResourceNum )
		m_pTextureResourceNodeList = reinterpret_cast<const resource::TextureResourceNode**>( this->GetInstanceAllocator()->SystemMemoryMalloc( sizeof(resource::TextureResourceNode*) * m_TextureResourceNum ) );

	for( u32 i = 0; i < m_TextureResourceNum; ++i )
	{
		m_pTextureResourceNodeList[i] = pTextureResourceNodeList[i];
	}
}

void ModelInstanceNode::SetShaderResourceNode( const resource::ShaderResourceNode **pShaderResourceNodeList, u32 size )
{
	GFL_ASSERT(pShaderResourceNodeList);
	if ( m_pShaderResourceNodeList )
		this->GetInstanceAllocator()->SystemMemoryFree( m_pShaderResourceNodeList );

	m_ShaderResourceNum = size;
	if ( m_ShaderResourceNum )
		m_pShaderResourceNodeList = reinterpret_cast<const resource::ShaderResourceNode**>( this->GetInstanceAllocator()->SystemMemoryMalloc( sizeof(resource::ShaderResourceNode*) * m_ShaderResourceNum ) );

	for( u32 i = 0; i < m_ShaderResourceNum; ++i )
	{
		m_pShaderResourceNodeList[i] = pShaderResourceNodeList[i];
	}
}

void ModelInstanceNode::SetMeshResourceNode( const resource::MeshResourceNode **pMeshResourceNodeList, u32 size )
{
	GFL_ASSERT(pMeshResourceNodeList);
	if ( m_pMeshResourceNodeList )
		this->GetInstanceAllocator()->SystemMemoryFree( m_pMeshResourceNodeList );	

	m_MeshResourceNum = size;
	if ( m_MeshResourceNum )
		m_pMeshResourceNodeList = reinterpret_cast<const resource::MeshResourceNode**>( this->GetInstanceAllocator()->SystemMemoryMalloc( sizeof(resource::MeshResourceNode*) * m_MeshResourceNum ) );

	this->CullingObjectNode::m_AABB.SetZero();
	for( u32 i = 0; i < m_MeshResourceNum; ++i )
	{
		m_pMeshResourceNodeList[i] = pMeshResourceNodeList[i];
		this->CullingObjectNode::m_AABB = this->CullingObjectNode::m_AABB.Merge( m_pMeshResourceNodeList[i]->GetAABB() );
	}
}

void ModelInstanceNode::SetGfMdlResourceNode( const resource::GfMdlResourceNode *pModelResourceNode)
{
	GFL_ASSERT(pModelResourceNode);
	m_pModelResourceNode = pModelResourceNode;

	this->SetAABB( m_pModelResourceNode->GetAABB() );
}

void ModelInstanceNode::LinkNode()
{
	//GFL_ASSERT( m_ShaderResourceNum == m_pModelResourceNode->GetShaderNum() );
	GFL_ASSERT( m_MaterialResourceNum == m_pModelResourceNode->GetMaterialNum() );
	//GFL_ASSERT( m_TextureResourceNum == m_pModelResourceNode->GetTextureNum() );
	GFL_ASSERT( m_MeshResourceNum == m_pModelResourceNode->GetMeshNum() );

	{//マテリアルにテクスチャを関連付ける
		if ( m_pMaterialInstanceNodeList )
		{
			delete[] m_pMaterialInstanceNodeList;
		}

    if ( m_MaterialResourceNum > 0)
    {
      m_pMaterialInstanceNodeList = new MaterialInstanceNode[ m_MaterialResourceNum ];
    }

		for( u32 i = 0; i < m_MaterialResourceNum; ++i )
		{
			m_pMaterialInstanceNodeList[i].LinkData( m_pMaterialResourceNodeList[i],
																					 m_pTextureResourceNodeList, m_TextureResourceNum,
																					 m_pShaderResourceNodeList, m_ShaderResourceNum );
		}
	}

#if defined(GF_PLATFORM_CTR)
	{//マテリアルに共有データを関連付ける
		for( u32 i = 0; i < m_MaterialResourceNum; ++i )
		{
			for( u32 i2 = 0; i2 < resource::MaterialResourceNode::LUT_MAX; ++i2 )
			{
				u32							nameHash = m_pMaterialInstanceNodeList[i].GetLutCommandNameHash( i2 );
				const u32*			pCommand = m_pModelResourceNode->GetLutTableCommand( nameHash );
				u32							commadSize = m_pModelResourceNode->GetLutTableCommandSize();
				m_pMaterialInstanceNodeList[i].SetLutCommand( i2, pCommand, commadSize );
			}
		}
	}
#endif

	{//メッシュにマテリアルを関連付ける
		if ( m_pMeshLinkDataList )
			delete[] m_pMeshLinkDataList;

    if (m_MeshResourceNum > 0)
    {
      m_pMeshLinkDataList = new MeshLinkData[ m_MeshResourceNum ];
    }

		for( u32 i = 0; i < m_MeshResourceNum; ++i )
		{
			m_pMeshLinkDataList[i].LinkData( m_pMeshResourceNodeList[i], m_pMaterialInstanceNodeList, m_MaterialResourceNum );
		}
	}

	{//モデルにメッシュを関連付ける
		m_ModelLinkData.LinkData( m_pModelResourceNode, m_pMeshLinkDataList, m_MeshResourceNum );
	}

  // ジョイント参照するなら最低限ジョイント数が同じかどうかをチェックしておく
  if (m_pJointReferenceModel != NULL)
  {
    if (m_pJointReferenceModel->GetJointNum() != m_pModelResourceNode->GetJointNum())
    {
      // 数が合わないので参照しない
      m_pJointReferenceModel = NULL;
    }
  }

  if (m_pJointReferenceModel == NULL)
  {
    m_JointNum = m_pModelResourceNode->GetJointNum();
    if ( m_JointNum )
    {//ジョイントノードの生成
      if ( m_JointInstanceNodeList )
        delete[] m_JointInstanceNodeList;

      m_JointInstanceNodeList = new JointInstanceNode[ m_JointNum ];

      JointInstanceNode			*pTopNode = NULL;

#ifdef GF_PLATFORM_CTR
      if(m_pModelResourceNode->IsStableJoint())
      {
        for( u32 jointNo = 0; jointNo < m_JointNum; ++jointNo )
        {
          const resource::GfMdlResourceNode::JointData		*pJointData = m_pModelResourceNode->GetJointData( jointNo );
          m_JointInstanceNodeList[jointNo].SetJointData( pJointData );

          if(pJointData->m_ParentIndex >= 0)
          {
            m_JointInstanceNodeList[pJointData->m_ParentIndex].AddChild(&m_JointInstanceNodeList[jointNo]);
          }
          else
          {
            GFL_ASSERT( pTopNode == NULL );
            pTopNode = &m_JointInstanceNodeList[jointNo];
          }
        }
      }
      else
#endif
      {
        for( u32 jointNo = 0; jointNo < m_JointNum; ++jointNo )
        {
          const resource::GfMdlResourceNode::JointData		*pJointData = m_pModelResourceNode->GetJointData( jointNo );
          m_JointInstanceNodeList[jointNo].SetJointData( pJointData );
        }

        for( u32 x = 0; x < m_JointNum; ++x )
        {
          JointInstanceNode			*pParent = &m_JointInstanceNodeList[x];

          if ( std::strlen( pParent->GetParentName() ) == 0 )
          {
            GFL_ASSERT( pTopNode == NULL );
            pTopNode = pParent;
          }

          for( u32 y = 0; y < m_JointNum; ++y )
          {
            JointInstanceNode			*pTarget = &m_JointInstanceNodeList[y];

            if ( std::strcmp( pParent->GetName(), pTarget->GetParentName() ) == 0 )
            {
              pParent->AddChild( pTarget );
            }
          }
        }
      }

      if (pTopNode != NULL)
      {
        // ルートのジョイントだけは親(モデル)のスケールを引き継ぐ
        pTopNode->SetNonInheritanceScale(false);

        // 子供として追加
        this->AddChild( pTopNode );
      }
    }
  }

	const resource::GfMdlResourceNode::DefaultModelUserData*			pDefaultModelUserData = m_pModelResourceNode->GetDefaultUserData();
	if ( pDefaultModelUserData )
		m_BillboardType = pDefaultModelUserData->m_BillboardType;

	InitializeDrawTag();
	SetUpDrawTag();
}

void ModelInstanceNode::InitializeDrawTag()
{
	{//描画タグを生成
		if ( m_pMeshDrawTagList )
		{
			delete[] m_pMeshDrawTagList;
			m_pMeshDrawTagList = NULL;
		}

		m_MeshDrawTagListNum = 0;
		for( u32 meshNo = 0; meshNo < m_pModelResourceNode->GetMeshNum(); ++meshNo )
		{
			const MeshLinkData			*pMeshLinkData = (m_ModelLinkData.m_pMeshLinkData[meshNo]);
			if ( pMeshLinkData->m_pMeshResourceNode->IsBlendShape() )
				continue;
			m_MeshDrawTagListNum += pMeshLinkData->GetFaceNum();
		}

    if( m_MeshDrawTagListNum > 0)
    {
      m_pMeshDrawTagList = renderer::MeshDrawTag::CreateArray(this->GetInstanceAllocator(), m_MeshDrawTagListNum);
#if GFL_DEBUG
      this->SetDrawCheckListener(true);
#endif
    }
	}
}

void ModelInstanceNode::SetDrawCheckListener( b32 enable )
{
	for( u32 i = 0; i < m_MeshDrawTagListNum; ++i )
	{
		m_pMeshDrawTagList[i].SetDrawListener( enable ? this : NULL );
	}
}

void ModelInstanceNode::SetUpDrawTag()
{
	u32			drawTagCnt = 0;

	for( u32 meshNo = 0; meshNo < m_pModelResourceNode->GetMeshNum(); ++meshNo )
	{
		const MeshLinkData														*pMeshLinkData = (m_ModelLinkData.m_pMeshLinkData[meshNo]);

		for( u32 faceNo = 0; faceNo < pMeshLinkData->GetFaceNum(); ++faceNo )
		{
			const resource::MeshResourceNode::FaceData			*pFaceData =  pMeshLinkData->GetFaceData( faceNo );
			const MaterialInstanceNode											*pMaterialInstanceNode = pMeshLinkData->m_pMaterialInstanceNodeList[faceNo];

			if ( pMeshLinkData->m_pMeshResourceNode->IsBlendShape() )
				continue;

			m_pMeshDrawTagList[drawTagCnt].SetMaterialInstanceNode( pMaterialInstanceNode );
			m_pMeshDrawTagList[drawTagCnt].SetEnabledColorShaderDriver( pMaterialInstanceNode->GetShaderDriver() );

			u32 textureCount = 0;
			
			//for( u32 texNo = 0; texNo < pMaterialInstanceNode->GetTextureNum(); ++texNo )
			for( u32 texNo = 0; texNo < MaterialInstanceNode::MATERIAL_TEXTURE_MAX; ++texNo )
			{
				if (pMaterialInstanceNode->GetTextureResourceNode(texNo) != NULL)
				{
					const gfx::Texture								*pTexture = pMaterialInstanceNode->GetTextureResourceNode(texNo)->GetTexture();
					const gfx::SamplerStateObject			*pSampler = pMaterialInstanceNode->GetMaterialResourceNode()->GetSamplerStateObject( texNo );
					m_pMeshDrawTagList[drawTagCnt].SetTexture( texNo, pTexture );
					m_pMeshDrawTagList[drawTagCnt].SetSampler( texNo, pSampler );
					++textureCount;
				}
				/*else
				{
					const gfx::SamplerStateObject			*pSampler = pMaterialLinkData->m_pMaterialResourceNode->GetDummySamplerStateObject();
					m_pMeshDrawTagList[drawTagCnt].SetSampler( texNo, pSampler );
				}*/
			}

			GFL_ASSERT( pMaterialInstanceNode->GetTextureNum() == textureCount );

			m_pMeshDrawTagList[drawTagCnt].SetFaceData( pFaceData );
			m_pMeshDrawTagList[drawTagCnt].SetIndexBuffer( pFaceData->GetIndexBuffer() );

			{
				m_pMeshDrawTagList[drawTagCnt].SetVertexBuffer( 0, pFaceData->GetVertexBuffer() );

				for( u32 blendMeshNo = 0; blendMeshNo < Constant::BLEND_MESH_MAX; ++blendMeshNo)
				{
					const MeshLinkData::AnimationData::BlendMeshData		&rBlendMeshData = pMeshLinkData->m_AnimationData.m_BlendMeshData[blendMeshNo];
					const MeshLinkData																	*pBlendMeshLinkData = rBlendMeshData.m_pBlendMesh;
					f32																									blendWeight = rBlendMeshData.m_BlendWeight;

					if ( pBlendMeshLinkData )
					{
						const resource::MeshResourceNode::FaceData			*pBlendFaceData =  pBlendMeshLinkData->GetFaceData( faceNo );
						m_pMeshDrawTagList[drawTagCnt].SetVertexBuffer( 1 + blendMeshNo, pBlendFaceData->GetVertexBuffer() );
						m_pMeshDrawTagList[drawTagCnt].SetVertexBlendWeight( 1 + blendMeshNo, blendWeight );
					}
					else
					{
						m_pMeshDrawTagList[drawTagCnt].SetVertexBuffer( 1 + blendMeshNo, NULL );
						m_pMeshDrawTagList[drawTagCnt].SetVertexBlendWeight( 1 + blendMeshNo, 0.0f );
					}
				}
			}
			
			m_pMeshDrawTagList[drawTagCnt].SetWorldMatrix( &m_WorldMatrix );
			m_pMeshDrawTagList[drawTagCnt].SetJointInstanceNode( this->GetJointInstanceNode(0), this->GetJointNum() );//ハードに合わせ変更の可能性あり。

			m_pMeshDrawTagList[drawTagCnt].SetCullingAABB( &m_WorldAABB );//!@note スキンモデルはモデルAABB,そうでなければメッシュAABBを用いる。
			m_pMeshDrawTagList[drawTagCnt].SetOcclusionQuery( NULL );

			GFL_ASSERT( drawTagCnt < m_MeshDrawTagListNum );//ありえないがチェック
			++drawTagCnt;
		}		
	}
}

u32 ModelInstanceNode::GetDrawTagNum()
{
	return m_MeshDrawTagListNum;
}

u32 ModelInstanceNode::GetTotalVertexNum() const
{
  u32 totalVertexNum = 0;

  for( u32 meshNo = 0; meshNo < m_pModelResourceNode->GetMeshNum(); ++meshNo )
  {
    const MeshLinkData* pMeshLinkData = ( m_ModelLinkData.m_pMeshLinkData[meshNo] );
		const resource::MeshResourceNode*	pMeshResourceNode = pMeshLinkData->GetMeshResourceNode();
    
    totalVertexNum += pMeshResourceNode->GetTotalVertexNum(); 
  }

  return totalVertexNum;
}

u32 ModelInstanceNode::GetTotalIndexNum() const
{
  u32 totalIndexNum = 0;

  for( u32 meshNo = 0; meshNo < m_pModelResourceNode->GetMeshNum(); ++meshNo )
  {
    const MeshLinkData* pMeshLinkData = ( m_ModelLinkData.m_pMeshLinkData[meshNo] );
		const resource::MeshResourceNode*	pMeshResourceNode = pMeshLinkData->GetMeshResourceNode();
    
    totalIndexNum += pMeshResourceNode->GetTotalIndexNum(); 
  }

  return totalIndexNum;
}

u32 ModelInstanceNode::GetTotalPolygonNum() const
{
  u32 totalPolygonNum = 0;

  for( u32 meshNo = 0; meshNo < m_pModelResourceNode->GetMeshNum(); ++meshNo )
  {
    const MeshLinkData* pMeshLinkData = ( m_ModelLinkData.m_pMeshLinkData[meshNo] );
    const resource::MeshResourceNode*	pMeshResourceNode = pMeshLinkData->GetMeshResourceNode();

    totalPolygonNum += pMeshResourceNode->GetTotalPolygonNum(); 
  }

  return totalPolygonNum;
}

void ModelInstanceNode::UpdateDrawTag(const DrawableNode::UpdateDrawTagInfo& updateDrawTagInfo)
{
	if ( IsMoved() )
	{
		m_WorldAABB = this->GetWorldAABB();
	}
	m_fPreDraw = false;

  // !@note 全てのメッシュ、テクスチャを更新するのは無駄なので後で最適化
  // アニメーション更新
	u32 drawTagIndex = 0;
  u32 meshNum = m_pModelResourceNode->GetMeshNum();
	for (u32 meshNo = 0; meshNo < meshNum; ++meshNo)
	{
		const MeshLinkData											*pMeshLinkData = m_ModelLinkData.m_pMeshLinkData[meshNo];
		const resource::MeshResourceNode				*pMeshResourceNode = pMeshLinkData->GetMeshResourceNode();

    // ブレンドシェイプなら無視
    if (pMeshLinkData->m_pMeshResourceNode->IsBlendShape())
    {
      continue;
    }

    // 全フェイス
    u32 faceNum = pMeshLinkData->GetFaceNum();
		for (u32 faceNo = 0; faceNo < faceNum; ++faceNo)
		{
      // ビジビリティ更新
      b32 visible = pMeshLinkData->m_AnimationData.m_IsVisible;
      if (m_pJointReferenceModel != NULL)
      {
        u32 refMeshNum = m_pJointReferenceModel->GetMehsNum();
        for (u32 refMeshNo = 0; refMeshNo < refMeshNum; ++refMeshNo)
        {
          const MeshLinkData* pRefMeshLinkData = m_pJointReferenceModel->GetMeshLinkData(refMeshNo);
          if (pMeshLinkData->GetNameHash() == pRefMeshLinkData->GetNameHash())
          {
            if (std::strcmp(pMeshLinkData->GetName(), pRefMeshLinkData->GetName()) == 0)
            {
              visible = pRefMeshLinkData->IsVisible();
              break;
            }
          }
        }
      }
      m_pMeshDrawTagList[drawTagIndex].SetVisible(visible);
      

      // テクスチャパターン更新
      const MaterialInstanceNode* pMaterialInstanceNode = pMeshLinkData->m_pMaterialInstanceNodeList[faceNo];
			for( u32 texNo = 0; texNo < MaterialInstanceNode::MATERIAL_TEXTURE_MAX; ++texNo )
			{
				if (pMaterialInstanceNode->GetTextureResourceNode(texNo) == NULL)
        {
          continue;
        }

        const gfx::Texture* pTexture = pMaterialInstanceNode->GetTextureResourceNode(texNo)->GetTexture();
        m_pMeshDrawTagList[drawTagIndex].SetTexture( texNo, pTexture );
      }

      u32 lightSetNo = static_cast<u32>(pMaterialInstanceNode->GetAttributeParam()->m_LightSetNo);
      m_pMeshDrawTagList[drawTagIndex].SetLightSetNo(lightSetNo);

      // 描画順のレンダーキーの作成
      renderer::DrawTag::RenderKey renderKey;
      renderKey.value = 0;
      renderKey.drawLayer = static_cast<u64>(m_pMeshDrawTagList[drawTagIndex].GetDrawLayerNo());
			renderKey.programmableLayer = m_SortPriority;

      f32 fDepth = 0.0f;
      const renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam* attributeParam =
       pMaterialInstanceNode->GetAttributeParam();
      renderKey.priority = static_cast<u64>(attributeParam->m_Priority) + renderer::DrawTag::RenderKey::PRIORITY_MARGIN;
      const renderingengine::scenegraph::resource::MaterialResourceNode::BlendTest& blendTest = 
       pMaterialInstanceNode->GetMaterialResourceNode()->GetBlendTest();
      renderKey.drawLayer = static_cast<u64>(blendTest.m_DrawLayer);

      if (blendTest.m_DrawLayer == scenegraph::resource::DrawLayer::Layer1 || 
					blendTest.m_DrawLayer == scenegraph::resource::DrawLayer::Layer5)
      {// レイヤー1は半透明描画なのでZの深度値を書き込む
				math::Vector4		centerPosition = pMeshResourceNode->GetAABB().GetCenter();
				math::Vector3		centerPosition3( centerPosition.x, centerPosition.y, centerPosition.z );

				centerPosition3 = m_WorldMatrix * centerPosition3;

        centerPosition.x = centerPosition3.x;
        centerPosition.y = centerPosition3.y;
        centerPosition.z = centerPosition3.z;
        centerPosition.w = 1.0f;

        math::Vector4 result = updateDrawTagInfo.m_ViewMatrix * centerPosition;
				fDepth = 1.0f - (result.Length3() / updateDrawTagInfo.m_Far);
      }

      renderKey.depth = static_cast<u64>(0x00FFFFFF * fDepth);
      renderKey.materialHash = static_cast<u64>(attributeParam->m_MaterialHash);

      // 2016.03.28 niji ポケファインダー用 機能拡張
      if ( m_RenderKeyType )
      {
        if (blendTest.m_DepthTest.m_UpDateDepth )
        {
          renderKey.priority -= renderer::DrawTag::RenderKey::PRIORITY_MARGIN; //優先する
        }else{
          renderKey.priority += renderer::DrawTag::RenderKey::PRIORITY_MARGIN; //優先度下げる
        }
      }
      // 描画タグにキーを設定
      m_pMeshDrawTagList[drawTagIndex].SetRenderKey(renderKey.value);

      ++drawTagIndex;
    }
  }
}

renderer::DrawTag* ModelInstanceNode::GetDrawTag( u32 no )
{
	GFL_ASSERT( no < m_MeshDrawTagListNum );
	return &m_pMeshDrawTagList[no];
}

void ModelInstanceNode::PreDraw(renderer::RenderState &state )
{
#if 0
	if ( m_fPreDraw )
		return;
	m_fPreDraw = true;

	if ( CullingObjectNode::m_State & CullingObjectNode::State::OcclusionCulling )
	{
		GFGL::BeginOcclusionQuery( m_pOcclusionQuery );

		//AABBの描画

		GFGL::EndOcclusionQuery( m_pOcclusionQuery );
	}
#endif
}

void ModelInstanceNode::PostDraw(renderer::RenderState &state )
{
	if ( scenegraph::SceneGraphManager::IsDrawingNode( this ) == false )
	{//未登録
		scenegraph::SceneGraphManager::AddDrawingNode( this );
	}
}

void ModelInstanceNode::ReferenceCntInc()
{
	for( u32 i = 0; i < m_MaterialResourceNum; ++i )
	{
		resource::MaterialResourceNode	*pMaterialResourceNodeList = const_cast<resource::MaterialResourceNode*>( m_pMaterialResourceNodeList[i] );
		pMaterialResourceNodeList->ReferenceCntInc();
	}

	for( u32 i = 0; i < m_TextureResourceNum; ++i )
	{
		resource::TextureResourceNode	*pTextureResourceNode = const_cast<resource::TextureResourceNode*>( m_pTextureResourceNodeList[i] );
		pTextureResourceNode->ReferenceCntInc();
	}

	for( u32 i = 0; i < m_ShaderResourceNum; ++i )
	{
		resource::ShaderResourceNode	*pShaderResourceNode = const_cast<resource::ShaderResourceNode*>( m_pShaderResourceNodeList[i] );
		pShaderResourceNode->ReferenceCntInc();
	}

	for( u32 i = 0; i < m_MeshResourceNum; ++i )
	{
		resource::MeshResourceNode	*pMeshResourceNode = const_cast<resource::MeshResourceNode*>( m_pMeshResourceNodeList[i] );
		pMeshResourceNode->ReferenceCntInc();
	}

	++m_GpuReferenceCnt;
}

void ModelInstanceNode::ReferenceCntDec()
{
	GFL_ASSERT( m_GpuReferenceCnt );

	for( u32 i = 0; i < m_MaterialResourceNum; ++i )
	{
		resource::MaterialResourceNode	*pMaterialResourceNodeList = const_cast<resource::MaterialResourceNode*>( m_pMaterialResourceNodeList[i] );
		pMaterialResourceNodeList->ReferenceCntDec();
	}

	for( u32 i = 0; i < m_TextureResourceNum; ++i )
	{
		resource::TextureResourceNode	*pTextureResourceNode = const_cast<resource::TextureResourceNode*>( m_pTextureResourceNodeList[i] );
		pTextureResourceNode->ReferenceCntDec();
	}

	for( u32 i = 0; i < m_ShaderResourceNum; ++i )
	{
		resource::ShaderResourceNode	*pShaderResourceNode = const_cast<resource::ShaderResourceNode*>( m_pShaderResourceNodeList[i] );
		pShaderResourceNode->ReferenceCntDec();
	}

	for( u32 i = 0; i < m_MeshResourceNum; ++i )
	{
		resource::MeshResourceNode	*pMeshResourceNode = const_cast<resource::MeshResourceNode*>( m_pMeshResourceNodeList[i] );
		pMeshResourceNode->ReferenceCntDec();
	}

	--m_GpuReferenceCnt;
}

void ModelInstanceNode::StateChangeCallBack( u32 beforeState )
{
#if 0
	beforeState &= CullingObjectNode::State::OcclusionCulling;
	u32 nowState = m_State & CullingObjectNode::State::OcclusionCulling;

	if ( beforeState != nowState )
	{
		gfx::OcclusionQuery		*pObject = NULL;
		IDrawListener					*pDrawListener = NULL;

		if ( nowState )
		{
			pDrawListener = this;
			pObject = m_pOcclusionQuery;
		}

		for( u32 i = 0; i < m_MeshDrawTagListNum; ++i )
		{
			m_pMeshDrawTagList[i].SetDrawListener( pDrawListener );
			m_pMeshDrawTagList[i].SetOcclusionQuery( pObject );
		}
	}
#endif
}

MaterialInstanceNode* ModelInstanceNode::GetMaterialInstanceNodeByNameHash( u32 nameHash )
{
  for( u32 i = 0; i < m_MaterialResourceNum; i++ )
  {
    if( m_pMaterialInstanceNodeList[ i ].GetNameHash() == nameHash )
    {
      return &m_pMaterialInstanceNodeList[i];
    }
  }
  return NULL;
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  MeshLinkDataクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
ModelInstanceNode::MeshLinkData::MeshLinkData():gfx::GLMemory(),
	m_pMaterialInstanceNodeList( NULL ),
  m_pMeshResourceNode(NULL)
{

}

ModelInstanceNode::MeshLinkData::~MeshLinkData()
{
	if ( m_pMaterialInstanceNodeList )
		this->GetInstanceAllocator()->SystemMemoryFree( (void*)m_pMaterialInstanceNodeList );
}

void ModelInstanceNode::MeshLinkData::LinkData( const resource::MeshResourceNode* pMesh, const MaterialInstanceNode* pMaterials, u32 materialNum )
{
	m_pMeshResourceNode = pMesh;

	u32				faceNum = m_pMeshResourceNode->GetFaceNum();

	if ( m_pMaterialInstanceNodeList )
		this->GetInstanceAllocator()->SystemMemoryFree( (void*)m_pMaterialInstanceNodeList );

	m_pMaterialInstanceNodeList = reinterpret_cast<const MaterialInstanceNode**>( this->GetInstanceAllocator()->SystemMemoryMalloc( sizeof(MaterialInstanceNode*) * faceNum ) );

	for( u32 faceNo = 0; faceNo < faceNum; ++faceNo )
	{
		const resource::MeshResourceNode::FaceData				*pFaceData;

		pFaceData = m_pMeshResourceNode->GetFaceData( faceNo );

		m_pMaterialInstanceNodeList[faceNo] = NULL;

		for( u32 i = 0; i < materialNum; ++i )
		{
			u32			nameHash = pMaterials[i].GetNameHash();

			if ( pFaceData->m_MaterialNameHash == nameHash )
			{
				const c8*			pName = pMaterials[i].GetName();

				if ( std::strcmp( pFaceData->m_MaterialName, pName ) == 0 )
				{
					m_pMaterialInstanceNodeList[faceNo] = &pMaterials[i];
					break;
				}
			}
		}

		GFL_ASSERT( m_pMaterialInstanceNodeList[faceNo] );
	}
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  ModelLinkDataクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
ModelInstanceNode::ModelLinkData::ModelLinkData():
	m_pModelResourceNode( NULL ),
	m_pMeshLinkData( NULL )
{

}

ModelInstanceNode::ModelLinkData::~ModelLinkData()
{
	if ( m_pMeshLinkData )
		this->GetInstanceAllocator()->SystemMemoryFree( (void*)m_pMeshLinkData );
}

void ModelInstanceNode::ModelLinkData::LinkData( const resource::GfMdlResourceNode* pModel, const ModelInstanceNode::MeshLinkData* pMeshs, u32 meshNum )
{
	u32			modelMeshNum = pModel->GetMeshNum();

	if ( m_pMeshLinkData )
		this->GetInstanceAllocator()->SystemMemoryFree( (void*)m_pMeshLinkData );

	m_pMeshLinkData = reinterpret_cast<const MeshLinkData**>( this->GetInstanceAllocator()->SystemMemoryMalloc( sizeof(MeshLinkData*) * modelMeshNum ) );

	for( u32 meshNo = 0; meshNo < modelMeshNum; ++meshNo )
	{
		u32						hash = pModel->GetMeshNameHash( meshNo ).value;
		const c8*			name = pModel->GetMeshName(meshNo);

		m_pMeshLinkData[meshNo] = NULL;

		for( u32 i = 0; i < meshNum; ++i )
		{
			u32			nameHash = pMeshs[i].GetNameHash();
			
			if ( hash == nameHash )
			{
				const c8*	pName = pMeshs[i].GetName();

				if ( std::strcmp( name, pName ) == 0 )
				{
					m_pMeshLinkData[meshNo] = &pMeshs[i];
					break;
				}
			}
		}

		GFL_ASSERT( m_pMeshLinkData[meshNo] );
	}
}

class ModelInstanceNodeReferenceCountVisitor
{
public:

  ModelInstanceNodeReferenceCountVisitor() :
    m_TotalReferenceCount(0)
  {
  }

  void Visit(ModelInstanceNode* pNode)
  {
    m_TotalReferenceCount += pNode->m_GpuReferenceCnt;
  }

  u32 GetTotalReferenceCount() const
  {
    return m_TotalReferenceCount;
  }

private:
  u32 m_TotalReferenceCount;
};

u32 ModelInstanceNode::GetReferenceCnt() const
{
  Traverser::Visitor<ModelInstanceNode, ModelInstanceNodeReferenceCountVisitor> visitor;
  Traverser::Traverse<ModelInstanceNode, ModelInstanceNodeReferenceCountVisitor>(const_cast<ModelInstanceNode*>(this), &visitor);

  return visitor.GetTotalReferenceCount();
}

#if defined(GF_PLATFORM_CTR)
/**
  * @brief マテリアルの初期値をステートオブジェクトにコピーする
  * @@attention ＣＴＲのステートオブジェクトはマテリアルの初期値を引き継いでいません。別途コピーの実行が必要です。
  */
void ModelInstanceNode::CopyMaterialDepthStencilStateToStateObject()
{
  for( u32 i = 0; i < this->GetMaterialNum(); ++i )
  {
    this->GetMaterialInstanceNode(i)->CopyMaterialDepthStencilStateToStateObject();
  }
}
#endif

}}}}

