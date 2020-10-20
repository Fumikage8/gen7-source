#include <Animation/include/gfl2_BlendShapeAnimationSlot.h>
#include <RenderingEngine/include/SceneGraph/gfl2_Traverser.h>

using namespace gfl2;
using namespace gfl2::renderingengine;

namespace gfl2 { namespace animation { 

/**
 * @brief コンストラクタ
 */
BlendShapeAnimationSlot::BlendShapeAnimationSlot() :
  AnimationSlotBase(),
	m_pVisibilityDataSetList(NULL)
{
}

/**
 * @brief デストラクタ
 */
BlendShapeAnimationSlot::~BlendShapeAnimationSlot()
{
}

/**
 * @brief ブレンドシェイプを適用
 *
 * @param meshIndex メッシュインデックス
 */
void BlendShapeAnimationSlot::ApplyBlendShape(u32 meshIndex)
{
  // リソースが無ければ無視
  if (m_pResourceNode == NULL)
  {
    return;
  }

  // ビジビリティデータが無いなら無視
  if (m_pVisibilityDataSetList == NULL)
  {
    return;
  }

  scenegraph::instance::ModelInstanceNode::MeshLinkData* pMeshLinkData = m_pModelNode->GetMeshLinkData(meshIndex);
  BlendShapeDataSet& blendShapeDataSet = m_pVisibilityDataSetList[meshIndex];

  // ブレンドシェイプアニメーションの反映
  if (blendShapeDataSet.m_pBlendShapeAnimationResourceNode == NULL)
  {
    return;
  }

  blendShapeDataSet.m_pBlendShapeAnimationResourceNode->GetBlendShape(m_CurrentFrame, pMeshLinkData, m_pModelNode);

#if 0
  const scenegraph::resource::BlendShapeAnimationResourceNode::FrameNodeData* pFrameNodeData = blendShapeDataSet.m_pBlendShapeAnimationResourceNode->GetFrameNodeData(m_CurrentFrame);
  bool updateFlag = false;

  for (u32 i = 0; i < Constant::BLEND_MESH_MAX; ++i)
  {
    scenegraph::instance::ModelInstanceNode::MeshLinkData* pBlendMeshLinkData = NULL;

    const c8* meshName = pFrameNodeData->m_BlendMeshName[i];

    // 対象のブレンドシェイプを探す
    for (u32 j = 0; j < m_pModelNode->GetMehsNum(); ++j)
    {
      scenegraph::instance::ModelInstanceNode::MeshLinkData* pTmpMeshLinkData = m_pModelNode->GetMeshLinkData(j);

      const c8* tmpName = pTmpMeshLinkData->GetName();

      if (strcmp(meshName, pTmpMeshLinkData->GetName()) == 0)
      {
        pBlendMeshLinkData = pTmpMeshLinkData;
        break;
      }
    }

    pMeshLinkData->SetBlendMeshData(i, pBlendMeshLinkData, pFrameNodeData->m_BlendWeight[i]);
    updateFlag = true;
  }

  if (updateFlag)
  {
    m_pModelNode->SetUpDrawTag();
  }
#endif
}

/**
 * @brief 派生クラスでの初期化
 *
 * @param pHeap ヒープ
 */
void BlendShapeAnimationSlot::InitializeDerived(gfl2::heap::HeapBase* pHeap)
{
  // メッシュが無いなら終了
  if (m_pModelNode->GetMehsNum() == 0)
  {
    return;
  }

  GFL_ASSERT(pHeap != NULL);
  m_pVisibilityDataSetList = GFL_NEW_ARRAY(pHeap) BlendShapeDataSet[m_pModelNode->GetMehsNum()];
}

/**
 * @brief 派生クラスでの終了
 */
void BlendShapeAnimationSlot::FinalizeDerived()
{
  // ブレンドシェイプデータセットリスト開放
  GFL_SAFE_DELETE_ARRAY(m_pVisibilityDataSetList);
}

/**
 * @brief 派生クラスでのアニメーションリソース設定
 *
 * @param pHeap ヒープ
 */
void BlendShapeAnimationSlot::SetAnimationResourceDerived(gfl2::heap::HeapBase* pHeap)
{
  // リソースが無ければ無視
  if (m_pResourceNode == NULL)
  {
    return;
  }

  // ブレンドシェイプデータが無ければ無視
  if (m_pVisibilityDataSetList == NULL)
  {
    return;
  }

  // ModelInstanceNodeとBlendShapeAnimationResourceNodeを関連付ける。
  scenegraph::Traverser::Visitor<scenegraph::resource::BlendShapeAnimationResourceNode, BlendShapeAnimationResourceNodeVisitor> visitor;
  visitor.SetThisPtr(this);
  scenegraph::Traverser::Traverse<scenegraph::resource::BlendShapeAnimationResourceNode, BlendShapeAnimationResourceNodeVisitor>(m_pResourceNode, &visitor);
}


/**
 * @brief ブレンドシェイプアニメーションリソース巡回
 *
 * @param pResourceNode リソースノード
 */
void BlendShapeAnimationSlot::Visit(gfl2::renderingengine::scenegraph::resource::BlendShapeAnimationResourceNode* pResourceNode)
{
  // 同じ名前のメッシュと関連付ける
  for (u32 i = 0; i < m_pModelNode->GetMehsNum(); ++i)
  {
    const c8*	pMeshName = m_pModelNode->GetMeshLinkData(i)->GetName();
    if (std::strcmp(pMeshName, pResourceNode->GetNodeName()) == 0)
    {
      m_pVisibilityDataSetList[i].m_pBlendShapeAnimationResourceNode = pResourceNode;
      break;
    }
  }
}

#if 0
void AnimationControler::Visit( scenegraph::resource::BlendShapeAnimationResourceNode* pNode )
{
	u32					hashCheck = pNode->GetTargetNameHash();

	for( u32 meshNo = 0; meshNo < m_TargetMeshNum; ++meshNo )
	{
		scenegraph::instance::ModelInstanceNode::MeshLinkData			*pMeshLinkData = m_pBlendShapeDataSetList[meshNo].m_pMeshLinkData;

		if( hashCheck == pMeshLinkData->GetNameHash() )
		{
			m_pBlendShapeDataSetList[meshNo].m_pBlendShapeAnimationResourceNode = pNode;
		}
	}
}
#endif

}}
