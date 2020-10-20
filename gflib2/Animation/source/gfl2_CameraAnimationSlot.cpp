#include <Animation/include/gfl2_CameraAnimationSlot.h>

#include <RenderingEngine/include/SceneGraph/gfl2_Traverser.h>


using namespace gfl2;
using namespace gfl2::renderingengine;

namespace gfl2 { namespace animation { 

/**
 * @brief コンストラクタ
 */
CameraAnimationSlot::CameraAnimationSlot() :
  AnimationSlotBase(),
  m_pCameraDataSetList(NULL)
{
}

/**
 * @brief デストラクタ
 */
CameraAnimationSlot::~CameraAnimationSlot()
{
}

/**
 * @brief カメラを適用
 *
 * @param out_pValueList 出力する値リスト
 * @param cameraIndex カメラインデックス
 */
void CameraAnimationSlot::ApplyCamera(f32* out_pValueList, u32 cameraIndex)
{
  // リソースが無ければ無視
  if (m_pResourceNode == NULL)
  {
    return;
  }

  // カメラが無いなら無視
  if (m_pCameraDataSetList == NULL)
  {
    return;
  }

  // 対応するカメラセット
  CameraDataSet* pCameraDataSet = &m_pCameraDataSetList[cameraIndex];
  if (pCameraDataSet->m_pAnimBody == NULL)
  {
    return;
  }

  for (u32 curveIndex = 0; curveIndex < CameraDataSet::INDEX_COUNT; ++curveIndex)
  {
    CurvePlayer* pCurvePlayer = &pCameraDataSet->m_CurvePlayerList[curveIndex];
    pCurvePlayer->GetValue(&out_pValueList[curveIndex], m_CurrentFrame);
  }
}

/**
 * @brief 派生クラスでの初期化
 *
 * @param pHeap ヒープ
 */
void CameraAnimationSlot::InitializeDerived(gfl2::heap::HeapBase* pHeap)
{
  // カメラが無いなら無視
  if (m_pDrawEnvNode->GetCameraNum() == 0)
  {
    return;
  }

  m_pCameraDataSetList = GFL_NEW_ARRAY(pHeap) CameraDataSet[m_pDrawEnvNode->GetCameraNum()];
}

/**
 * @brief 派生クラスでの終了
 */
void CameraAnimationSlot::FinalizeDerived()
{
  // ライトデータセットリスト開放
  GFL_SAFE_DELETE_ARRAY(m_pCameraDataSetList);
}

/**
 * @brief 派生クラスでのアニメーションリソース設定
 *
 * @param pHeap ヒープ
 */
void CameraAnimationSlot::SetAnimationResourceDerived(gfl2::heap::HeapBase* pHeap)
{
  // 全カメラ初期化
  for (u32 cameraNo = 0; cameraNo < m_pDrawEnvNode->GetCameraNum(); ++cameraNo)
  {
    CameraDataSet* pCameraDataSet = &m_pCameraDataSetList[cameraNo];
    pCameraDataSet->m_pAnimBody = NULL;
  }

  // リソースが無ければ無視
  if (m_pResourceNode == NULL)
  {
    return;
  }
  if (m_pResourceNode->m_pEnvCameraAnimHeader == NULL)
  {
    return;
  }

  // カメラアニメーション関連付け
  uptr nameAddr = reinterpret_cast<uptr>(m_pResourceNode->m_pEnvCameraAnimHeader) + sizeof(scenegraph::resource::AnimationResourceNode::EnvCameraAnimHeader);
  uptr bodyAddr = nameAddr + m_pResourceNode->m_pEnvCameraAnimHeader->nameBufferSize;
  for (u32 animCameraNo = 0; animCameraNo < m_pResourceNode->m_pEnvCameraAnimHeader->cameraCount; ++animCameraNo)
  {
    // 名前解決
    const scenegraph::resource::AnimationResourceNode::Name* pName = reinterpret_cast<const scenegraph::resource::AnimationResourceNode::Name*>(nameAddr);
    nameAddr += pName->length + sizeof(char);

    // アニメーションデータ解決
    const scenegraph::resource::AnimationResourceNode::EnvCameraAnimBody* pBody = reinterpret_cast<const scenegraph::resource::AnimationResourceNode::EnvCameraAnimBody*>(bodyAddr);
    bodyAddr += pBody->dataSize + sizeof(scenegraph::resource::AnimationResourceNode::EnvCameraAnimBody) - sizeof(u32);

    uptr curveAddr = reinterpret_cast<uptr>(pBody->data);

    // 対応するカメラを探す
    for (u32 cameraNo = 0; cameraNo < m_pDrawEnvNode->GetCameraNum(); ++cameraNo)
    {
      scenegraph::instance::CameraNode* pCameraNode = m_pDrawEnvNode->GetCamera(cameraNo);
      if (!pName->IsSameName(pCameraNode->GetNodeName()))
      {
        continue;
      }

      CameraDataSet* pCameraDataSet = &m_pCameraDataSetList[cameraNo];
      pCameraDataSet->m_pAnimBody = pBody;

      // カーブ読み込み
      for (int curveIndex = 0; curveIndex < CameraDataSet::INDEX_COUNT; ++curveIndex)
      {
        CurvePlayer* pCurvePlayer = &pCameraDataSet->m_CurvePlayerList[curveIndex];
        pCurvePlayer->Initialize(curveAddr, pBody->flags, curveIndex, m_FrameCount);
      }

      // 検索終了
      break;
    }
  }
}


}}