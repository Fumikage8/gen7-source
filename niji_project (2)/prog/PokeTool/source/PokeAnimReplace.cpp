//======================================================================
/**
 * @file	PokeAnimReplace.cpp
 * @brief	ポケモン用表情アニメ上書き用のクラス
 * @author	takebe_makoto
 * @data	16/04/05
 */
//======================================================================

#include "PokeTool/include/PokeAnimReplace.h"

GFL_NAMESPACE_BEGIN(PokeTool)

/**
 * @brief コンストラクタ
 */
PokeAnimReplace::PokeAnimReplace() :
  m_pModelNode(NULL),
  m_pJointSlot(NULL),
  m_ReplaceCount(0),
  m_pReplaceIndexList(NULL),
  m_pSRTBlendBufferList(NULL),
  m_SmoothFrameCurrent(0),
  m_SmoothFrameTotal(0),
  m_State(STATE_NONE)
{
}

/**
 * @brief デストラクタ
 */
PokeAnimReplace::~PokeAnimReplace()
{
}

/**
 * @brief 初期化
 *
 * @param pHeap ヒープ
 * @param pModelNode モデルノード
 * @param pResourceNode リソースノード
 */
void PokeAnimReplace::Initialize(gfl2::heap::HeapBase* pHeap, gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelNode, gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode)
{
  // リソースが無ければここで終了
  if (pResourceNode == NULL)
  {
    return;
  }

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

  // ジョイントアニメが含まれていなければ終了
  if (!pAnimResourceNode->HasJointAnimation())
  {
    return;
  }

  m_pModelNode = pModelNode;

  // ジョイントスロット確保
  m_pJointSlot = GFL_NEW(pHeap) gfl2::animation::JointAnimationSlot();
  m_pJointSlot->Initialize(pHeap, pModelNode);
  m_pJointSlot->SetAnimationResource(pResourceNode);

  // 上書きするジョイントの数を確認
  m_ReplaceCount = 0;
  for (u32 i = 0; i < m_pModelNode->GetJointNum(); ++i)
  {
    if (m_pJointSlot->CanGetJointSRT(i))
    {
      ++m_ReplaceCount;
    }
  }

  // 上書きの必要無し
  if (m_ReplaceCount == 0)
  {
    return;
  }

  // バッファを確保して上書きするジョイントのインデックスを保存
  m_pReplaceIndexList = GFL_NEW_ARRAY(pHeap) u32[m_ReplaceCount];
  u32 counter = 0;
  for (u32 i = 0; i < m_pModelNode->GetJointNum(); ++i)
  {
    if (m_pJointSlot->CanGetJointSRT(i))
    {
      m_pReplaceIndexList[counter] = i;
      ++counter;
    }
  }

  // 補間用のバッファを確保
  m_pSRTBlendBufferList = GFL_NEW_ARRAY(pHeap) gfl2::math::SRT[m_ReplaceCount];

  // 初期化完了
  m_State = STATE_STAY;
}

/**
 * @brief 終了
 */
void PokeAnimReplace::Finalize()
{
  if (m_pJointSlot != NULL)
  {
    m_pJointSlot->Finalize();
    GFL_SAFE_DELETE(m_pJointSlot);
  }

  GFL_SAFE_DELETE_ARRAY(m_pSRTBlendBufferList);
  GFL_SAFE_DELETE_ARRAY(m_pReplaceIndexList);
  m_ReplaceCount = 0;
  m_pModelNode = NULL;

  m_State = STATE_NONE;
}

/**
 * @brief 補間しつつ上書き開始
 *
 * @param frame 初期フレーム, マイナスの値を指定すると上書き終了
 * @param stepFrame 再生速度
 * @param smoothFrame 補間フレーム数
 */
void PokeAnimReplace::ReplaceSmooth(f32 frame, f32 stepFrame, u32 smoothFrame)
{
  if (m_pJointSlot == NULL)
  {
    return;
  }

  m_pJointSlot->SetFrame(frame);
  m_pJointSlot->SetStepFrame(stepFrame);

  m_SmoothFrameCurrent = 0;
  m_SmoothFrameTotal = smoothFrame;

  // 現在のSRT保存
  for (u32 i = 0; i < m_ReplaceCount; ++i)
  {
    u32 jointIndex = m_pReplaceIndexList[i];
    gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pJointNode = m_pModelNode->GetJointInstanceNode(jointIndex);

    m_pSRTBlendBufferList[i] = pJointNode->GetLocalSRT();
  }

  if (m_SmoothFrameTotal <= 0)
  {
    if (frame < 0)
    {
      // 上書き終了、待機させる
      m_State = STATE_STAY;
    }
    else
    {
      // 補間無しで上書き
      m_State = STATE_REPLACE;
    }
  }
  else
  {
    if (frame < 0)
    {
      // 終了時の補間
      m_State = STATE_END_SMOOTH;
    }
    else
    {
      // 開始時の補間
      m_State = STATE_BEGIN_SMOOTH;
    }
  }
}

/**
 * @brief フレームを取得
 *
 * @return アニメーションが設定されているなら現在のフレーム数を返す、設定されていないなら-1.0fを返す
 */
f32 PokeAnimReplace::GetFrame() const
{
  if (m_pJointSlot == NULL)
  {
    return -1.0f;
  }

  return m_pJointSlot->GetFrame();
}

/**
 * @brief 上書き更新
 */
void PokeAnimReplace::UpdateReplace()
{
  if (m_pJointSlot == NULL)
  {
    return;
  }

  // 補間フレーム更新
  switch (m_State)
  {
  case STATE_BEGIN_SMOOTH:
    {
      if (this->UpdateBlendFrame())
      {
        m_State = STATE_REPLACE;
      }
    }
    break;

  case STATE_END_SMOOTH:
    {
      if (this->UpdateBlendFrame())
      {
        m_State = STATE_STAY;
      }
    }
    break;
  }

  // SRTを上書き
  switch (m_State)
  {
  case STATE_BEGIN_SMOOTH:
    {
      f32 blendWeight = this->GetBlendWeight();

      // 新しい値と補間しつつジョイントSRTを上書きする
      for (u32 i = 0; i < m_ReplaceCount; ++i)
      {
        u32 jointIndex = m_pReplaceIndexList[i];
        gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pJointNode = m_pModelNode->GetJointInstanceNode(jointIndex);

        gfl2::math::SRT blendOldSRT = m_pSRTBlendBufferList[i];
        gfl2::math::SRT blendNewSRT = pJointNode->GetLocalSRT();
        m_pJointSlot->GetJointSRT(&blendNewSRT, jointIndex);
        blendOldSRT.Slerp(blendNewSRT, blendWeight);

        pJointNode->SetLocalSRT(blendOldSRT);
      }
    }
    break;
  case STATE_END_SMOOTH:
    {
      f32 blendWeight = this->GetBlendWeight();

      // デフォルト値と補間しつつジョイントSRTを上書きする
      for (u32 i = 0; i < m_ReplaceCount; ++i)
      {
        u32 jointIndex = m_pReplaceIndexList[i];
        gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pJointNode = m_pModelNode->GetJointInstanceNode(jointIndex);

        gfl2::math::SRT blendOldSRT = m_pSRTBlendBufferList[i];
        gfl2::math::SRT blendNewSRT = pJointNode->GetLocalSRT();
        blendOldSRT.Slerp(blendNewSRT, blendWeight);

        pJointNode->SetLocalSRT(blendOldSRT);
      }
    }
    break;
  case STATE_REPLACE:
    {
      // 補間なしにそのままSRTを上書きする
      for (u32 i = 0; i < m_ReplaceCount; ++i)
      {
        u32 jointIndex = m_pReplaceIndexList[i];
        gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pJointNode = m_pModelNode->GetJointInstanceNode(jointIndex);

        gfl2::math::SRT srt = pJointNode->GetLocalSRT();
        m_pJointSlot->GetJointSRT(&srt, jointIndex);

        pJointNode->SetLocalSRT(srt);
      }
    }
    break;
  }

}

// 補間フレーム更新
bool PokeAnimReplace::UpdateBlendFrame()
{
  ++m_SmoothFrameCurrent;
  if (m_SmoothFrameCurrent > m_SmoothFrameTotal)
  {
    m_SmoothFrameCurrent = m_SmoothFrameTotal;
    return true;
  }

  return false;
}

// 補間率取得
f32 PokeAnimReplace::GetBlendWeight() const
{
  return (f32)m_SmoothFrameCurrent / (f32)m_SmoothFrameTotal;
}

GFL_NAMESPACE_END(PokeTool)

