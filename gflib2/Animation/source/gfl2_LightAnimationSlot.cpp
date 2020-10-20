#include <Animation/include/gfl2_LightAnimationSlot.h>

#include <RenderingEngine/include/SceneGraph/gfl2_Traverser.h>


using namespace gfl2;
using namespace gfl2::renderingengine;

namespace gfl2 { namespace animation { 

/**
 * @brief コンストラクタ
 */
LightAnimationSlot::LightAnimationSlot() :
  AnimationSlotBase()
{
  for (u32 i = 0; i < scenegraph::instance::DrawEnvNode::LightSetMax; ++i)
  {
    m_pLightDataSetList[i] = NULL;
  }
}

/**
 * @brief デストラクタ
 */
LightAnimationSlot::~LightAnimationSlot()
{
}

/**
 * @brief ライトを適用
 *
 * @param out_pValueList 出力する値リスト
 * @param lightSetIndex ライトセットインデックス
 * @param lightIndex ライトインデックス
 */
void LightAnimationSlot::ApplyLight(f32* out_pValueList, u32 lightSetIndex, u32 lightIndex)
{
  // リソースが無ければ無視
  if (m_pResourceNode == NULL)
  {
    return;
  }

  LightDataSet* pLightDataSetList = m_pLightDataSetList[lightSetIndex];

  // ライトが無いなら無視
  if (pLightDataSetList == NULL)
  {
    return;
  }

  LightDataSet* pLightDataSet = &pLightDataSetList[lightIndex];

  // @fix GFNMCat[805] アニメーションが存在しないなら無視
  if (pLightDataSet->m_pAnimBody == NULL)
  {
    return;
  }

  // ライトの反映
  for (u32 curveIndex = 0; curveIndex < LightDataSet::INDEX_COUNT; ++curveIndex)
  {
    CurvePlayer* pCurvePlayer = &pLightDataSet->m_CurvePlayerList[curveIndex];
    pCurvePlayer->GetValue(&out_pValueList[curveIndex], m_CurrentFrame);
  }
}

/**
 * @brief 派生クラスでの初期化
 *
 * @param pHeap ヒープ
 */
void LightAnimationSlot::InitializeDerived(gfl2::heap::HeapBase* pHeap)
{
  for (u32 i = 0; i < scenegraph::instance::DrawEnvNode::LightSetMax; ++i)
  {
    scenegraph::instance::LightSetNode* pLightSetNode = m_pDrawEnvNode->GetLightSet(i);

    // ライトが無いなら無視
    if (pLightSetNode == NULL)
    {
      continue;
    }

    if (pLightSetNode->GetLightNum() == 0)
    {
      continue;
    }

    m_pLightDataSetList[i] = GFL_NEW_ARRAY(pHeap) LightDataSet[pLightSetNode->GetLightNum()];
  }
}

/**
 * @brief 派生クラスでの終了
 */
void LightAnimationSlot::FinalizeDerived()
{
  // ライトデータセットリスト開放
  for (u32 i = 0; i < scenegraph::instance::DrawEnvNode::LightSetMax; ++i)
  {
    GFL_SAFE_DELETE_ARRAY(m_pLightDataSetList[i]);
  }
}

/**
 * @brief 派生クラスでのアニメーションリソース設定
 *
 * @param pHeap ヒープ
 */
void LightAnimationSlot::SetAnimationResourceDerived(gfl2::heap::HeapBase* pHeap)
{
  // 全ライト初期化
  for (u32 i = 0; i < scenegraph::instance::DrawEnvNode::LightSetMax; ++i)
  {
    if (m_pLightDataSetList[i] == NULL)
    {
      continue;
    }

    LightDataSet* pLightDataSet = m_pLightDataSetList[i];
    pLightDataSet->m_pAnimBody = NULL;
  }

  // リソースが無ければ無視
  if (m_pResourceNode == NULL)
  {
    return;
  }
  if (m_pResourceNode->m_pEnvLightAnimHeader == NULL)
  {
    return;
  }

  // ライトアニメーション関連付け
  uptr nameAddr = reinterpret_cast<uptr>(m_pResourceNode->m_pEnvLightAnimHeader) + sizeof(scenegraph::resource::AnimationResourceNode::EnvLightAnimHeader);
  uptr bodyAddr = nameAddr + m_pResourceNode->m_pEnvLightAnimHeader->nameBufferSize;
  for (u32 animLightNo = 0; animLightNo < m_pResourceNode->m_pEnvLightAnimHeader->lightCount; ++animLightNo)
  {
    // 名前解決
    const scenegraph::resource::AnimationResourceNode::Name* pName = reinterpret_cast<const scenegraph::resource::AnimationResourceNode::Name*>(nameAddr);
    nameAddr += pName->length + sizeof(char);

    // アニメーションデータ解決
    const scenegraph::resource::AnimationResourceNode::EnvLightAnimBody* pBody = reinterpret_cast<const scenegraph::resource::AnimationResourceNode::EnvLightAnimBody*>(bodyAddr);
    bodyAddr += pBody->dataSize + sizeof(scenegraph::resource::AnimationResourceNode::EnvLightAnimBody) - sizeof(u64);

    uptr curveAddr = reinterpret_cast<uptr>(pBody->data);

    // ライトセットが無いなら無視
    scenegraph::instance::LightSetNode* pLightSetNode = m_pDrawEnvNode->GetLightSet(pBody->lightSetNo);
    if (pLightSetNode == NULL)
    {
      continue;
    }
    LightDataSet* pLightDataSetList = m_pLightDataSetList[pBody->lightSetNo];

    // 対応するライトを探す
    for (u32 lightNo = 0; lightNo < pLightSetNode->GetLightNum(); ++lightNo)
    {
      scenegraph::instance::LightNode* pLightNode = pLightSetNode->GetLightNode(lightNo);
      if (!pName->IsSameName(pLightNode->GetNodeName()))
      {
        continue;
      }

      LightDataSet* pLightDataSet = &pLightDataSetList[lightNo];
      pLightDataSet->m_pAnimBody = pBody;

      // カーブ読み込み
      for (int curveIndex = 0; curveIndex < LightDataSet::INDEX_COUNT; ++curveIndex)
      {
        CurvePlayer* pCurvePlayer = &pLightDataSet->m_CurvePlayerList[curveIndex];
        pCurvePlayer->Initialize(curveAddr, pBody->flags, curveIndex, m_FrameCount);
      }

      // 検索終了
      break;
    }
  }
}


}}