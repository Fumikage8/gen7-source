//kawa20150514

#include <Animation/include/gfl2_GlareAnimationSlot.h>

#include <RenderingEngine/include/SceneGraph/gfl2_Traverser.h>


using namespace gfl2;
using namespace gfl2::renderingengine;

namespace gfl2 { namespace animation { 

/**
 * @brief コンストラクタ
 */
GlareAnimationSlot::GlareAnimationSlot() :
  AnimationSlotBase(),
  m_pGlareDataSet(NULL)
{
}

/**
 * @brief デストラクタ
 */
GlareAnimationSlot::~GlareAnimationSlot()
{
}

/**
 * @brief グレア(ブルーム)を適用
 *
 * @param out_pValueList 出力する値リスト
 * @note  グレア(ブルーム)は1個しかないのでインデックスは必要ない。
 */
void GlareAnimationSlot::ApplyGlare(f32* out_pValueList)
{
  // リソースが無ければ無視
  if (m_pResourceNode == NULL)
  {
    return;
  }

  // グレア(ブルーム)が無いなら無視
  if (m_pGlareDataSet == NULL)
  {
    return;
  }

  // 対応するグレア(ブルーム)セット
  if (m_pGlareDataSet->m_pAnimBody == NULL)
  {
    return;
  }

  for (u32 curveIndex = 0; curveIndex < GlareDataSet::INDEX_COUNT; ++curveIndex)
  {
    CurvePlayer* pCurvePlayer = &m_pGlareDataSet->m_CurvePlayerList[curveIndex];
    pCurvePlayer->GetValue(&out_pValueList[curveIndex], m_CurrentFrame);
  }
}

/**
 * @brief 派生クラスでの初期化
 *
 * @param pHeap ヒープ
 */
void GlareAnimationSlot::InitializeDerived(gfl2::heap::HeapBase* pHeap)
{
  m_pGlareDataSet = GFL_NEW(pHeap) GlareDataSet();
}

/**
 * @brief 派生クラスでの終了
 */
void GlareAnimationSlot::FinalizeDerived()
{
  // グレア(ブルーム)データセット開放
  GFL_SAFE_DELETE(m_pGlareDataSet);
}

/**
 * @brief 派生クラスでのアニメーションリソース設定
 *
 * @param pHeap ヒープ
 */
void GlareAnimationSlot::SetAnimationResourceDerived(gfl2::heap::HeapBase* pHeap)
{
  // グレア(ブルーム)初期
  m_pGlareDataSet->m_pAnimBody = NULL;

  // リソースが無ければ無視
  if (m_pResourceNode == NULL)
  {
    return;
  }
  if (m_pResourceNode->m_pEnvGlareAnimHeader == NULL)
  {
    return;
  }

  // グレア(ブルーム)アニメーション関連付け
  uptr bodyAddr = reinterpret_cast<uptr>(m_pResourceNode->m_pEnvGlareAnimHeader);
  {
    // アニメーションデータ解決
    const scenegraph::resource::AnimationResourceNode::EnvGlareAnimBody* pBody = reinterpret_cast<const scenegraph::resource::AnimationResourceNode::EnvGlareAnimBody*>(bodyAddr);
    bodyAddr += pBody->dataSize + sizeof(scenegraph::resource::AnimationResourceNode::EnvGlareAnimBody) - sizeof(u32);

    uptr curveAddr = reinterpret_cast<uptr>(pBody->data);

    // 対応するグレア(ブルーム)
    {
      m_pGlareDataSet->m_pAnimBody = pBody;

      // カーブ読み込み
      for (int curveIndex = 0; curveIndex < GlareDataSet::INDEX_COUNT; ++curveIndex)
      {
        CurvePlayer* pCurvePlayer = &m_pGlareDataSet->m_CurvePlayerList[curveIndex];
        pCurvePlayer->Initialize(curveAddr, pBody->flags, curveIndex, m_FrameCount);
      }
    }
  }
}


}}
