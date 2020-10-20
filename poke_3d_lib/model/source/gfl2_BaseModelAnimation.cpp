#include <model/include/gfl2_BaseModelAnimation.h>

using namespace gfl2::renderingengine::scenegraph;

namespace poke_3d { namespace model {

/**
 * @brief コンストラクタ
 */
BaseModelAnimation::BaseModelAnimation(gfl2::animation::AnimationController* pAnimationController, u32 slotIndex, gfl2::animation::AnimationController::AnimationBit animationBit) :
  m_pAnimationController(pAnimationController),
  m_SlotIndex(slotIndex),
  m_AnimationBit(animationBit)
{
}

/**
 * @brief デストラクタ
 */
BaseModelAnimation::~BaseModelAnimation()
{
}

//---------------------------------------------------------------------------------
// ■ アニメーション

/**
 * @brief アニメーションの更新
 */
f32 BaseModelAnimation::UpdateAnimation()
{
  return m_pAnimationController->Update(-1.0f, m_AnimationBit);
}

/**
 * @brief アニメーションの更新
 *
 * @param interpWeight 補間率を指定したい場合は0.0〜1.0の範囲で設定。デフォルトは自動で補間率を更新
 */
f32 BaseModelAnimation::UpdateAnimationManual(f32 interpWeight)
{
  return m_pAnimationController->Update(interpWeight, m_AnimationBit);
}

/**
 * @brief アニメーションリソースノードの取得
 *
 * @return アニメーションリソースノードを返す、設定されてなければNULL
 */
const gfl2::renderingengine::scenegraph::resource::ResourceNode* BaseModelAnimation::GetResourceNode() const
{
  const gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode = this->GetAnimationForeach_<const gfl2::renderingengine::scenegraph::resource::ResourceNode*>(&gfl2::animation::AnimationSlotBase::GetAnimationResource);
  return pResourceNode;
}

/**
 * @brief アニメーション再生用のメモリを明示的に確保する
 *
 * @param pAnimationHeap アニメーションヒープ
 * @param smoothFlag アニメーション補間を使うかどうか、使う場合は補間用のメモリも合わせて確保
 * @param pResourceNode リソースノード
 */
void BaseModelAnimation::Allocate(gfl2::heap::HeapBase* pAnimationHeap, b32 smoothFlag, gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode)
{
  // 通常スロット用の確保
  this->AllocateAnimationResourceForeach_(pAnimationHeap, pResourceNode);

  if (smoothFlag)
  {
    // 補間スロット用の確保
    this->AllocateAnimationResourceForeachInterp_(pAnimationHeap, pResourceNode);
  }
}

/**
 * @brief デフォルトのブレンドフレーム数取得
 *
 * @param pResourceNode リソースノード
 * @param pMotionConfigNode モーションコンフィグノード
 *
 * @return 現在再生中のアニメーションとのデフォルトのブレンドフレーム数を返す
 */
u32 BaseModelAnimation::GetDefaultBlendFrame(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode, const gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode* pMotionConfigNode) const
{
  if (pResourceNode == NULL)
  {
    return 0;
  }
  
  if (pMotionConfigNode == NULL)
  {
    return 0;
  }

  const gfl2::renderingengine::scenegraph::resource::ResourceNode* pCurrentResourceNode = this->GetResourceNode();
  if (pCurrentResourceNode == NULL)
  {
    return 0;
  }

  // ブレンド指定無しならリソースノードからファイルのハッシュ値を取得する
  const gfl2::renderingengine::scenegraph::resource::AnimationResourceNode* pFrom = CastAnimationResourceNode(pCurrentResourceNode);
  const gfl2::renderingengine::scenegraph::resource::AnimationResourceNode* pTo = CastAnimationResourceNode(pResourceNode);
  u32 fromHash = pFrom->GetFileNameHash();
  u32 toHash = pTo->GetFileNameHash();

  return pMotionConfigNode->GetBlendFrame(fromHash, toHash);
}

/**
 * @brief アニメーションの変更
 *
 * @param pResourceNode リソースノード
 * @param pMotionConfigNode モーションコンフィグノード
 */
void BaseModelAnimation::Change(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode, const gfl2::renderingengine::scenegraph::instance::MotionConfigInstanceNode* pMotionConfigNode)
{
  u32 defaultBlendFrame = this->GetDefaultBlendFrame(pResourceNode, pMotionConfigNode);
  this->ChangeSmooth(pResourceNode, defaultBlendFrame);
}

/**
 * @brief 現在の設定で補間有効か？
 *
 * @param pResourceNode リソースノード
 * @param changeFrame 補間フレーム数
 *
 * @return true:補間有効 false:補間無効
 */
bool BaseModelAnimation::IsSmoothEnable(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode, u32 changeFrame)
{
  // 補間フレームが0
  if (changeFrame == 0)
  {
    return false;
  }

  // 補間元のリソースが無い
  if (this->GetResourceNode() == NULL)
  {
    return false;
  }

  // 補間先のリソースが無い
  if (pResourceNode == NULL)
  {
    return false;
  }

  // ジョイントアニメが含まれない
  gfl2::renderingengine::scenegraph::resource::AnimationResourceNode* pAnimResourceNode = CastAnimationResourceNode(pResourceNode);
  if (!pAnimResourceNode->HasJointAnimation())
  {
    return false;
  }

  return true;
}

/**
 * @brief アニメーションの変更
 *
 * @param pResourceNode リソースノード
 * @param changeFrame 補間フレーム数
 */
void BaseModelAnimation::ChangeSmooth(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode, u32 changeFrame)
{
  bool interpEnable = this->IsSmoothEnable(pResourceNode, changeFrame);

  // 補間無し
  if (!interpEnable)
  {
    // 補間打ち切り
    m_pAnimationController->InterpRequest(-1, m_SlotIndex);

    this->SetAnimationResourceForeach_(pResourceNode);
    return;
  }

  // 補間中ならリソースクリア
  if (this->IsAnimationInterp())
  {
    this->SetAnimationResourceForeach_(NULL);
  }

  // 補間
  this->SetAnimationResourceForeachInterp_(pResourceNode);
  m_pAnimationController->InterpRequest(static_cast<f32>(changeFrame), m_SlotIndex);
}

/**
 * @brief アニメーションの変更：補間率の手動設定版
 *        この関数でアニメーションをセットした後UpdateAnimation()の引数に0.0〜1.0の範囲を指定してください
 *
 * @param pResourceNode0 リソースノード0
 * @param pResourceNode1 リソースノード1
 */
void BaseModelAnimation::ChangeSmoothManual(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode0, gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode1)
{
  this->SetAnimationResourceForeach_(pResourceNode0);
  this->SetAnimationResourceForeachInterp_(pResourceNode1);
}

/**
 * @brief アニメーション補間中か？：ChangeSmoothの終了判定用
 *
 * @return 補間中ならtrue, 補間中でなければfalse
 */
bool BaseModelAnimation::IsAnimationInterp() const
{
  return m_pAnimationController->IsInterp();
}

/**
 * @brief ループ設定：デフォルトはアニメーションデータに設定されている値
 *
 * @param enable trueの場合はループする、 falseの場合はループしない
 */
void BaseModelAnimation::SetLoop(b32 enable)
{
  this->SetAnimationForeach_(&gfl2::animation::AnimationSlotBase::SetLoop, enable != false);
}

/**
 * @brief ループ再生されるか？
 *
 * @return trueの場合はループする、 falseの場合はループしない
 */
b32 BaseModelAnimation::IsLoop() const
{
  return this->GetAnimationForeach_<b32>(&gfl2::animation::AnimationSlotBase::IsLoop);
}

/**
 * @brief フレーム設定
 *
 * @param frame フレーム
 */
void BaseModelAnimation::SetFrame(f32 frame)
{
  this->SetAnimationForeach_(&gfl2::animation::AnimationSlotBase::SetFrame, frame);
}

/**
 * @brief フレーム取得
 *
 * @return 現在のフレームを返す
 */
f32 BaseModelAnimation::GetFrame() const
{
  return this->GetAnimationForeach_<f32>(&gfl2::animation::AnimationSlotBase::GetFrame);
}

/**
 * @brief 再生速度設定
 *
 * @param stepFrame 再生速度：1.0fで等速、マイナスの値を指定すると逆再生
 */
void BaseModelAnimation::SetStepFrame(f32 stepFrame)
{
  this->SetAnimationForeach_(&gfl2::animation::AnimationSlotBase::SetStepFrame, stepFrame);
}

/**
 * @brief 再生速度取得
 *
 * @return 再生速度を返す
 */
f32 BaseModelAnimation::GetStepFrame() const
{
  return this->GetAnimationForeach_<f32>(&gfl2::animation::AnimationSlotBase::GetStepFrame);
}

/**
 * @brief ループの開始フレーム設定
 *
 * @param startFrame 開始フレーム
 */
void BaseModelAnimation::SetStartFrame(f32 startFrame)
{
  this->SetAnimationForeach_(&gfl2::animation::AnimationSlotBase::SetLoopBeginFrame, startFrame);
}

/**
 * @brief ループの開始フレーム取得
 *
 * @return 開始フレームを返す
 */
f32 BaseModelAnimation::GetStartFrame() const
{
  return this->GetAnimationForeach_<f32>(&gfl2::animation::AnimationSlotBase::GetLoopBeginFrame);
}

/**
 * @brief ループの終了フレーム設定
 *
 * @param endFrame 終了フレーム
 */
void BaseModelAnimation::SetEndFrame(f32 endFrame)
{
  this->SetAnimationForeach_(&gfl2::animation::AnimationSlotBase::SetLoopEndFrame, endFrame);
}

/**
 * @brief ループの終了フレーム取得
 *
 * @return 終了フレームを返す
 */
f32 BaseModelAnimation::GetEndFrame() const
{
  return this->GetAnimationForeach_<f32>(&gfl2::animation::AnimationSlotBase::GetLoopEndFrame);
}

/**
 * @brief 最終フレームか？ 順再生の場合はEndFrame, 逆再生の場合はStartFrameに到達するかチェック
 *
 * @return このフレームで最終フレームに到達するならtrue, 到達しないならfalse
 */
b32 BaseModelAnimation::IsLastFrame() const
{
  return this->GetAnimationForeach_<b32>(&gfl2::animation::AnimationSlotBase::IsLastFrame);
}

/**
 * @brief キーアニメーションのコールバックを設定
 *
 * @param pUserData ユーザーデータ
 * @param pCallback コールバック
 */
void BaseModelAnimation::SetKeyCallback(void* pUserData, gfl2::animation::KeyAnimationCallback* pCallback)
{
  // キーアニメーション専用
  gfl2::animation::AnimationController::AnimationBit oldAnimationBit = m_AnimationBit;
  m_AnimationBit = (gfl2::animation::AnimationController::AnimationBit)(m_AnimationBit & gfl2::animation::AnimationController::ANIMATION_BIT_KEY);

  this->SetAnimationForeach_<gfl2::animation::KeyAnimationSlot>(&gfl2::animation::KeyAnimationSlot::SetCallback, pUserData, pCallback);

  m_AnimationBit = oldAnimationBit;
}

/**
 * @brief 操作対象を補間スロットに切り替える：ビューアー用
 *
 * @param isTypeInterp 操作対象が補間スロットか？
 */
void BaseModelAnimation::SetControllTargetInterp(bool isTargetInterp)
{
  // 補間中と見せかける
  m_pAnimationController->SetForceInterpMode(isTargetInterp);
}

/**
 * @brief 補間用スロットのフレームと再生速度の同期を取る：ビューアー用
 */
void BaseModelAnimation::SyncFrameAndStepFrame()
{
  // 補間中でなければ問題ない
  if (!m_pAnimationController->IsInterp())
  {
    return;
  }

  gfl2::animation::AnimationController::AnimationSlotList fromSlotList = m_pAnimationController->GetAnimationSlotList(m_SlotIndex);
  gfl2::animation::AnimationController::AnimationSlotList toSlotList = m_pAnimationController->GetAnimationInterpSlotList(m_SlotIndex);

  for (u32 i = 0; i < gfl2::animation::AnimationController::ANIMATION_TYPE_COUNT; ++i)
  {
    if (!(m_AnimationBit & (1 << i)))
    {
      continue;
    }

    gfl2::animation::AnimationSlotBase* pFromSlot = this->GetAnimationSlot_(fromSlotList, static_cast<gfl2::animation::AnimationController::AnimationType>(i));
    if (pFromSlot == NULL)
    {
      continue;
    }
    if (!pFromSlot->HasAnimation())
    {
      continue;
    }

    gfl2::animation::AnimationSlotBase* pToSlot = this->GetAnimationSlot_(toSlotList, static_cast<gfl2::animation::AnimationController::AnimationType>(i));
    if (pToSlot == NULL)
    {
      continue;
    }
    if (!pToSlot->HasAnimation())
    {
      continue;
    }

    pToSlot->SetFrame(pFromSlot->GetFrame());
    pToSlot->SetStepFrame(pFromSlot->GetStepFrame());
  }
}

}}
