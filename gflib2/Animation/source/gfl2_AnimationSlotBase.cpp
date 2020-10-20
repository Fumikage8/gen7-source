#include <Animation/include/gfl2_AnimationSlotBase.h>

using namespace gfl2;
using namespace gfl2::renderingengine;

namespace gfl2 { namespace animation { 

/**
 * @brief コンストラクタ
 */
AnimationSlotBase::AnimationSlotBase() :
  //m_Weight(0.0f),
  m_StepFrame(1.0f),
  m_CurrentFrame(0.0f),
  m_OldFrame(0.0f),
  m_FrameCount(0),
  m_LoopFlag(false),
  m_LoopBeginFrame(-1.0f),
  m_LoopEndFrame(-1.0f),
  m_pHeap(NULL),
  m_pModelNode(NULL),
  m_pResourceNode(NULL),
  m_pReturnResourceNode(NULL)
{
}

/**
 * @brief デストラクタ
 */
AnimationSlotBase::~AnimationSlotBase()
{
}

/**
 * @brief 初期化
 *
 * @param pHeap ヒープ
 * @param pTargetNode ターゲットノード
 */
void AnimationSlotBase::Initialize(gfl2::heap::HeapBase* pHeap, gfl2::renderingengine::scenegraph::instance::TransformNode* pTargetNode)
{
  // 再利用された場合も考えて念の為終了処理を呼んでおく
  this->Finalize();

  // ターゲットノード保存、モデルかシーン環境どちらかのはず
	m_pModelNode = pTargetNode->SafeCast<gfl2::renderingengine::scenegraph::instance::ModelInstanceNode>();
	m_pDrawEnvNode = pTargetNode->SafeCast<gfl2::renderingengine::scenegraph::instance::DrawEnvNode>();

	m_FrameCount = 0;

  // 派生クラスの初期化
  this->InitializeDerived(pHeap);
  m_pHeap = pHeap;
}

/**
 * @brief 終了
 */
void AnimationSlotBase::Finalize()
{
  // 派生クラスでの終了
  this->FinalizeDerived();
}

/**
 * @brief アニメーションリソース設定
 *
 * @param pAnimationHeap メモリを確保するヒープ
 * @param pResourceNode リソースノード
 */
void AnimationSlotBase::SetAnimationResource(gfl2::heap::HeapBase* pAnimationHeap, gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode)
{
  // フレーム初期化
  m_CurrentFrame = 0.0f;
  m_OldFrame = 0.0f;
  m_LastFlag = false;

  // 速度初期化
  m_StepFrame = 1.0f;

  // 返却用にリソースノード保存
  m_pReturnResourceNode = pResourceNode;

  if (pResourceNode == NULL)
  {
    m_pResourceNode = NULL;

    // 派生クラスでのアニメーションリソース設定
    this->SetAnimationResourceDerived(pAnimationHeap);
    return;
  }

  // リソースノード保存
  gfl2::renderingengine::scenegraph::resource::AnimationResourceNode* pAnimResourceNode = NULL;

  // 子供にぶら下がっている可能性がある
  if (pResourceNode->GetChildCount() > 0)
  {
    // 子供がAnimationResourceNode
    pAnimResourceNode = pResourceNode->GetChild()->SafeCast<gfl2::renderingengine::scenegraph::resource::AnimationResourceNode>();
  }
  else
  {
    // 自分がAnimationResourceNode
    pAnimResourceNode = pResourceNode->SafeCast<gfl2::renderingengine::scenegraph::resource::AnimationResourceNode>();
  }

  // キャストに失敗するのはおかしい
  GFL_ASSERT(pAnimResourceNode != NULL);

  if (pAnimResourceNode != m_pResourceNode)
  {
    m_pResourceNode = pAnimResourceNode;

    // フレーム数とループフラグ
    m_FrameCount = m_pResourceNode->m_pAnimTop->frameSize;
    m_LoopFlag = m_pResourceNode->m_pAnimTop->loopFlag;
    m_DataLoopFlag = m_pResourceNode->m_pAnimTop->dataLoopFlag;
    m_LoopBeginFrame = 0.0f;
    m_LoopEndFrame = static_cast<f32>(m_FrameCount);

    // 派生クラスでのアニメーションリソース設定
    this->SetAnimationResourceDerived(pAnimationHeap);
  }
}

/**
 * @brief アニメーションリソース設定：ヒープ指定しない版 削除したい
 *
 * @param pResourceNode リソースノード
 */
void AnimationSlotBase::SetAnimationResource(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode)
{
  this->SetAnimationResource(m_pHeap, pResourceNode);
}

/**
 * @brief フレーム更新
 */
void AnimationSlotBase::UpdateFrame()
{
  // StepFrameを0にしてCurrentFrameを直接操作している場合
  // 終了が検出できなくなる。
  // StepFrameが0の場合、何も更新せずに戻る。
  if (m_StepFrame == 0)
  {
    return;
  }
  // フレームを進める
  m_OldFrame = m_CurrentFrame;
  m_CurrentFrame += m_StepFrame;

  m_LastFlag = false;

  // ループするか？
  if (m_LoopFlag)
  {
    // 順再生
    if ((m_StepFrame > 0.0f) && (m_CurrentFrame >= m_LoopEndFrame))
    {
      m_CurrentFrame = m_LoopBeginFrame;
      m_LastFlag = true;
    }
    // 逆再生
    else if ((m_StepFrame < 0.0f) && (m_CurrentFrame <= m_LoopBeginFrame))
    {
      m_CurrentFrame = m_LoopEndFrame;
      m_LastFlag = true;
    }
  }
  else
  {
    // 順再生
    if (m_CurrentFrame > m_LoopEndFrame)
    {
      m_CurrentFrame = m_LoopEndFrame;
      m_LastFlag = true;
    }
    // 逆再生
    else if (m_CurrentFrame < m_LoopBeginFrame)
    {
      m_CurrentFrame = m_LoopBeginFrame;
      m_LastFlag = true;
    }
  }
}

}}