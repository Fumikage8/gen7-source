//kawa20150514

#include <Animation/include/gfl2_FogAnimationSlot.h>

#include <RenderingEngine/include/SceneGraph/gfl2_Traverser.h>


using namespace gfl2;
using namespace gfl2::renderingengine;

namespace gfl2 { namespace animation { 

/**
 * @brief コンストラクタ
 */
FogAnimationSlot::FogAnimationSlot() :
  AnimationSlotBase()
{
  for (u32 i = 0; i < gfl2::renderingengine::scenegraph::instance::DrawEnvNode::FogMax; ++i)
  {
    m_pFogDataSetList[i] = NULL;
  }
}

/**
 * @brief デストラクタ
 */
FogAnimationSlot::~FogAnimationSlot()
{
}

/**
 * @brief フォグを適用
 *
 * @param out_pValueList 出力する値リスト
 * @param fogIndex 適用するフォグインデックス
 */
void FogAnimationSlot::ApplyFog(f32* out_pValueList, u32 fogIndex)
{
  if(fogIndex >= gfl2::renderingengine::scenegraph::instance::DrawEnvNode::FogMax)
  {
    GFL_ASSERT(0);  // 開発中に気付いてもらうためのASSERT
    return;
  }

  // リソースが無ければ無視
  if (m_pResourceNode == NULL)
  {
    return;
  }

  // フォグが無いなら無視
  if (m_pFogDataSetList[fogIndex] == NULL)
  {
    return;
  }

  // 対応するフォグセット
  FogDataSet* pFogDataSet = m_pFogDataSetList[fogIndex];
  if (pFogDataSet->m_pAnimBody == NULL)
  {
    return;
  }

  for (u32 curveIndex = 0; curveIndex < FogDataSet::INDEX_COUNT; ++curveIndex)
  {
    CurvePlayer* pCurvePlayer = &pFogDataSet->m_CurvePlayerList[curveIndex];
    pCurvePlayer->GetValue(&out_pValueList[curveIndex], m_CurrentFrame);
  }
}

/**
 * @brief 派生クラスでの初期化
 *
 * @param pHeap ヒープ
 */
void FogAnimationSlot::InitializeDerived(gfl2::heap::HeapBase* pHeap)
{
  for (u32 i = 0; i < gfl2::renderingengine::scenegraph::instance::DrawEnvNode::FogMax; ++i)
  {
    m_pFogDataSetList[i] = GFL_NEW(pHeap) FogDataSet();
  }
}

/**
 * @brief 派生クラスでの終了
 */
void FogAnimationSlot::FinalizeDerived()
{
  // フォグデータセットリスト開放
  for (u32 i = 0; i < gfl2::renderingengine::scenegraph::instance::DrawEnvNode::FogMax; ++i)
  {
    GFL_SAFE_DELETE(m_pFogDataSetList[i]);
  }
}

/**
 * @brief 派生クラスでのアニメーションリソース設定
 *
 * @param pHeap ヒープ
 */
void FogAnimationSlot::SetAnimationResourceDerived(gfl2::heap::HeapBase* pHeap)
{
  // 全フォグ初期化
  for (u32 i = 0; i < gfl2::renderingengine::scenegraph::instance::DrawEnvNode::FogMax; ++i)
  {
    FogDataSet* pFogDataSet = m_pFogDataSetList[i];
    pFogDataSet->m_pAnimBody = NULL;
  }

  // リソースが無ければ無視
  if (m_pResourceNode == NULL)
  {
    return;
  }
  if (m_pResourceNode->m_pEnvFogAnimHeader == NULL)
  {
    return;
  }

  // フォグアニメーション関連付け
  uptr bodyAddr = reinterpret_cast<uptr>(m_pResourceNode->m_pEnvFogAnimHeader) + sizeof(scenegraph::resource::AnimationResourceNode::EnvFogAnimHeader);
  for (u32 animFogNo = 0; animFogNo < m_pResourceNode->m_pEnvFogAnimHeader->fogCount; ++animFogNo)
  {
    // アニメーションデータ解決
    const scenegraph::resource::AnimationResourceNode::EnvFogAnimBody* pBody = reinterpret_cast<const scenegraph::resource::AnimationResourceNode::EnvFogAnimBody*>(bodyAddr);
    bodyAddr += pBody->dataSize + sizeof(scenegraph::resource::AnimationResourceNode::EnvFogAnimBody) - sizeof(u32);

    uptr curveAddr = reinterpret_cast<uptr>(pBody->data);


    // 対応するフォグ
    u32 fogNo = pBody->fogNo;  // 配列のインデックスとfogNoが一致するようにしておく。
    
    // フォグインデックスの確認
    if(fogNo >= gfl2::renderingengine::scenegraph::instance::DrawEnvNode::FogMax)
    {
      GFL_ASSERT(0);  // 開発中に気付いてもらうためのASSERT
      continue;
    }

    {
      FogDataSet* pFogDataSet = m_pFogDataSetList[fogNo];
      pFogDataSet->m_pAnimBody = pBody;

      // カーブ読み込み
      for (int curveIndex = 0; curveIndex < FogDataSet::INDEX_COUNT; ++curveIndex)
      {
        CurvePlayer* pCurvePlayer = &pFogDataSet->m_CurvePlayerList[curveIndex];
        pCurvePlayer->Initialize(curveAddr, pBody->flags, curveIndex, m_FrameCount);
      }
    }
  }
}


}}
