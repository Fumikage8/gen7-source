//===================================================================
/**
 * @file    EnvAccessor.cpp
 * @brief   DrawEnv共通処理
 * @author  saita_kazuki
 * @date    2015.05.11
 */
//===================================================================

#include "System/include/EnvAccessor.h"

GFL_NAMESPACE_BEGIN( System )

/**
 * @brief コンストラクタ
 */
EnvAccessor::EnvAccessor() :
  m_pDrawEnvNode( NULL),
  m_AnimationController()
{
}

/**
 * @brief デストラクタ
 */
EnvAccessor::~EnvAccessor()
{
}

/**
 * @brief 初期化
 * @param pHeap DrawEnvアニメーションから使用される(スロット取得時のメモリ確保など)
 */
void EnvAccessor::Initialize( gfl2::heap::HeapBase* pHeap, gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode)
{
  m_pDrawEnvNode = pDrawEnvNode;
  m_AnimationController.Initialize( pHeap, pDrawEnvNode);
}

/**
 * @brief 終了
 */
void EnvAccessor::Terminate()
{
  m_AnimationController.Finalize();
  m_pDrawEnvNode = NULL;
}

/**
 * @brief 更新
 */
void EnvAccessor::Update()
{
  m_AnimationController.Update();
}

/**
 * @brief アニメーション変更
 * @param pMotionResourceRootNode DrawEnvのアニメーションリソースノード(保持するだけで破棄はしません)
 * @param slotIndex スロット番号
 * @param animationBit どのアニメーションかを指定するビットフラグ
 */
void EnvAccessor::ChangeAnimation( gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionResourceRootNode, u32 slotIndex, AnimationBit animationBit)
{
  gfl2::animation::AnimationSlotBase* pSlot = this->GetAnimationSlot( slotIndex, animationBit);
  pSlot->SetAnimationResource( pMotionResourceRootNode);
}

/**
 * @brief アニメーションループ設定
 * @param isLoop trueでループ
 * @param slotIndex スロット番号
 * @param animationBit どのアニメーションかを指定するビットフラグ
 */
void EnvAccessor::SetAnimationLoop( bool isLoop, u32 slotIndex, AnimationBit animationBit)
{
  gfl2::animation::AnimationSlotBase* pSlot = this->GetAnimationSlot( slotIndex, animationBit);
  pSlot->SetLoop( isLoop);
}

/**
 * @brief アニメーションステップフレーム設定
 * @param stepFrame ステップフレーム
 * @param slotIndex スロット番号
 * @param animationBit どのアニメーションかを指定するビットフラグ
 */
void EnvAccessor::SetStepFrame( f32 stepFrame, u32 slotIndex, AnimationBit animationBit)
{
  gfl2::animation::AnimationSlotBase* pSlot = this->GetAnimationSlot( slotIndex, animationBit);
  pSlot->SetStepFrame( stepFrame);
}

/**
 * @brief アニメーションフレーム設定
 * @param frame フレーム数
 * @param slotIndex スロット番号
 * @param animationBit どのアニメーションかを指定するビットフラグ
 */
void EnvAccessor::SetFrame( f32 frame, u32 slotIndex, AnimationBit animationBit)
{
  gfl2::animation::AnimationSlotBase* pSlot = this->GetAnimationSlot( slotIndex, animationBit);
  pSlot->SetFrame( frame);
}

/**
 * @brief アニメーションフレーム設定
 * @param hour 時間
 * @param minute 分
 * @param second 秒
 * @param slotIndex スロット番号
 * @param animationBit どのアニメーションかを指定するビットフラグ
 */
void EnvAccessor::SetFrame( s32 hour, s32 minute, s32 second, u32 slotIndex, AnimationBit animationBit)
{
  gfl2::animation::AnimationSlotBase* pSlot = this->GetAnimationSlot( slotIndex, animationBit);
  pSlot->SetFrame( this->CalculateAllTimeAnimationFrame( pSlot->GetFrameCount(),hour, minute, second));
}

/**
 * @brief 保持しているDrawEnvNodeの取得
 * @return Initializeで受け取ったDrawEnvNode
 */
gfl2::renderingengine::scenegraph::instance::DrawEnvNode* EnvAccessor::GetDrawEnvNode() const
{
  return m_pDrawEnvNode;
}

/*
 * @brief スロット取得
 * @param slotIndex スロット番号
 * @param animationBit どのアニメーションかを指定するビットフラグ
 * @return 指定したスロット番号、アニメーションのスロット
 */
gfl2::animation::AnimationSlotBase* EnvAccessor::GetAnimationSlot( u32 slotIndex, AnimationBit animationBit)
{
  if( animationBit & ANIMATION_BIT_LIGHT)
  {
    return m_AnimationController.GetLightSlot( slotIndex);
  }
  if( animationBit & ANIMATION_BIT_FOG )
  {
    return m_AnimationController.GetFogSlot( slotIndex);
  }
  if( animationBit & ANIMATION_BIT_BLOOM )
  {
    return m_AnimationController.GetGlareSlot( slotIndex);
  }
  GFL_ASSERT( 0);
  return NULL;
}

/*
 * @brief 時間からアニメーションフレームを計算
 * @param frameMax アニメーションの最大フレーム数
 * @param hour 時間
 * @param minute 分
 * @param second 秒
 * @return 時間から求めたアニメーションフレーム数
 */
f32 EnvAccessor::CalculateAllTimeAnimationFrame( f32 frameMax, s32 hour, s32 minute, s32 second) const
{
  const f32 ONE_HOUR_FRAME   = frameMax / 24.0f;
  const f32 ONE_MINUTE_FRAME = frameMax / 1440.0f;
  const f32 ONE_SECOND_FRAME = frameMax / 86400.0f;
  return (hour * ONE_HOUR_FRAME) + (minute * ONE_MINUTE_FRAME) + (second * ONE_SECOND_FRAME);
}

GFL_NAMESPACE_END( System )
