#include <Animation/include/gfl2_VisibilityAnimationSlot.h>
#include <RenderingEngine/include/SceneGraph/gfl2_Traverser.h>

using namespace gfl2;
using namespace gfl2::renderingengine;

namespace gfl2 { namespace animation { 

/**
 * @brief コンストラクタ
 */
VisibilityAnimationSlot::VisibilityAnimationSlot() :
  AnimationSlotBase(),
	m_pVisibilityDataSetList(NULL)
{
}

/**
 * @brief デストラクタ
 */
VisibilityAnimationSlot::~VisibilityAnimationSlot()
{
}

/**
 * @brief ビジビリティを適用
 *
 * @param meshIndex メッシュインデックス
 */
void VisibilityAnimationSlot::ApplyVisibility(u32 meshIndex)
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
  VisibilityDataSet& visibilityDataSet = m_pVisibilityDataSetList[meshIndex];

  // ビジビリティアニメーションの反映
  if (visibilityDataSet.m_pAnimBody != NULL)
  {
    u32 frame = (u32)m_CurrentFrame;
    if (frame >= m_FrameCount) frame = m_FrameCount;

    s32 index = frame / 8;
    s32 bit = frame % 8;
    bool isVisible = (visibilityDataSet.m_pAnimBody->visibleFlags[index] & (1 << bit)) != 0;
    pMeshLinkData->SetVisible(isVisible);
  }
}

/**
 * @brief 派生クラスでの初期化
 *
 * @param pHeap ヒープ
 */
void VisibilityAnimationSlot::InitializeDerived(gfl2::heap::HeapBase* pHeap)
{
  // メッシュが無いなら終了
  if (m_pModelNode->GetMehsNum() == 0)
  {
    return;
  }

  GFL_ASSERT(pHeap != NULL);
  m_pVisibilityDataSetList = GFL_NEW_ARRAY(pHeap) VisibilityDataSet[m_pModelNode->GetMehsNum()];
}

/**
 * @brief 派生クラスでの終了
 */
void VisibilityAnimationSlot::FinalizeDerived()
{
  // ビジビリティデータセットリスト開放
  GFL_SAFE_DELETE_ARRAY(m_pVisibilityDataSetList);
}

/**
 * @brief 派生クラスでのアニメーションリソース設定
 *
 * @param pHeap ヒープ
 */
void VisibilityAnimationSlot::SetAnimationResourceDerived(gfl2::heap::HeapBase* pHeap)
{
  // 全ビジビリティ初期化
  for (u32 meshNo = 0; meshNo < m_pModelNode->GetMehsNum(); ++meshNo)
  {
    m_pVisibilityDataSetList[meshNo].m_pAnimBody = NULL;
  }

  // ビジビリティデータが無ければ無視
  if (m_pVisibilityDataSetList == NULL)
  {
    return;
  }

  // リソースが無ければ無視
  if (m_pResourceNode == NULL)
  {
    return;
  }
  if (m_pResourceNode->m_pVisibilityAnimHeader == NULL)
  {
    return;
  }

  u32 dataFrameCount = m_FrameCount + 1;
  u32 dataSize = dataFrameCount / 8;
  if ((dataFrameCount % 8) != 0)
  {
    dataSize += sizeof(u8);
  }

  // ビジビリティアニメーション関連付け
  uptr nameAddr = reinterpret_cast<uptr>(m_pResourceNode->m_pVisibilityAnimHeader) + sizeof(scenegraph::resource::AnimationResourceNode::VisibilityAnimHeader);
  uptr bodyAddr = nameAddr + m_pResourceNode->m_pVisibilityAnimHeader->nameBufferSize;
  for (u32 animMeshNo = 0; animMeshNo < m_pResourceNode->m_pVisibilityAnimHeader->meshCount; ++animMeshNo)
  {
    // 名前解決
    const scenegraph::resource::AnimationResourceNode::Name* pName = reinterpret_cast<const scenegraph::resource::AnimationResourceNode::Name*>(nameAddr);
    nameAddr += pName->length + sizeof(char);

    // アニメーションデータ解決
    const scenegraph::resource::AnimationResourceNode::VisibilityAnimBody* pBody = reinterpret_cast<const scenegraph::resource::AnimationResourceNode::VisibilityAnimBody*>(bodyAddr);
    bodyAddr += dataSize;

    // 対応するメッシュを探す
    for (u32 meshNo = 0; meshNo < m_pModelNode->GetMehsNum(); ++meshNo)
    {
      if (!pName->IsSameName(m_pModelNode->GetMeshLinkData(meshNo)->GetName()))
      {
        continue;
      }

      m_pVisibilityDataSetList[meshNo].m_pAnimBody = pBody;

      // 検索終了
      break;
    }
  }
}

}}