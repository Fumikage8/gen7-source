//===================================================================
/**
 * @file    EnvAccessor.cpp
 * @brief   DrawEnv共通処理
 * @author  saita_kazuki
 * @date    2015.05.11
 */
//===================================================================

#include "System/include/EnvAccessor.h"
#include <math/include/gfl2_Float.h>
#include <math/include/gfl2_MathCommon.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

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
  gfl2::animation::AnimationSlotBase* pSlot = this->GetAnimationSlot( slotIndex, animationBit, false);
  pSlot->SetAnimationResource( pMotionResourceRootNode);

  // 補間を打ち切る
  this->RequestAnimationSmooth(animationBit, -1);
}

/**
 * @brief アニメーション変更
 * @param pMotionResourceRootNode DrawEnvのアニメーションリソースノード(保持するだけで破棄はしません)
 * @param slotIndex スロット番号
 * @param animationBit どのアニメーションかを指定するビットフラグ
 * @param smoothFrame 補間フレーム数
 */
void EnvAccessor::ChangeAnimationSmooth( gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionResourceRootNode, u32 slotIndex, AnimationBit animationBit, u32 smoothFrame)
{
  // 現在補間中
  if (this->IsAnimationSmooth(animationBit))
  {
    // 補間無し
    this->ChangeAnimation(pMotionResourceRootNode, slotIndex, animationBit);
    return;
  }

  // 補間フレームが0
  if (smoothFrame == 0)
  {
    // 補間無し
    this->ChangeAnimation(pMotionResourceRootNode, slotIndex, animationBit);
    return;
  }

  // 補間先のリソースが無い
  if (pMotionResourceRootNode == NULL)
  {
    // 補間無し
    this->ChangeAnimation(pMotionResourceRootNode, slotIndex, animationBit);
    return;
  }

  // 補間する
  gfl2::animation::AnimationSlotBase* pSlot = this->GetAnimationSlot( slotIndex, animationBit, true);
  pSlot->SetAnimationResource( pMotionResourceRootNode);

  // 補間を打ち切る
  this->RequestAnimationSmooth(animationBit, smoothFrame);
}

/**
 * @brief アニメーションループ設定
 * @param isLoop trueでループ
 * @param slotIndex スロット番号
 * @param animationBit どのアニメーションかを指定するビットフラグ
 */
void EnvAccessor::SetAnimationLoop( bool isLoop, u32 slotIndex, AnimationBit animationBit)
{
  this->SetAnimationForeach( &gfl2::animation::AnimationSlotBase::SetLoop, isLoop, slotIndex, animationBit);
}

/**
 * @brief アニメーションステップフレーム設定
 * @param stepFrame ステップフレーム
 * @param slotIndex スロット番号
 * @param animationBit どのアニメーションかを指定するビットフラグ
 */
void EnvAccessor::SetStepFrame( f32 stepFrame, u32 slotIndex, AnimationBit animationBit)
{
  this->SetAnimationForeach( &gfl2::animation::AnimationSlotBase::SetStepFrame, stepFrame, slotIndex, animationBit);
}

/**
 * @brief アニメーションフレーム設定
 * @param frame フレーム数
 * @param slotIndex スロット番号
 * @param animationBit どのアニメーションかを指定するビットフラグ
 */
void EnvAccessor::SetFrame( f32 frame, u32 slotIndex, AnimationBit animationBit)
{
  this->SetAnimationForeach( &gfl2::animation::AnimationSlotBase::SetFrame, frame, slotIndex, animationBit);
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
  for( u32 i = 0; i < ANIMATION_TYPE_MAX; ++i)
  {
    if( (animationBit & (1 << i)) == false )
    {
      continue;
    }

    gfl2::animation::AnimationSlotBase* pSlot;
    pSlot = this->GetAnimationSlot( slotIndex, static_cast<AnimationType>( i), this->IsAnimationSmooth(static_cast<AnimationBit>( 1 << i)));

    if( pSlot == NULL)
    {
      continue;
    }

    if( pSlot->HasAnimation() == false)
    {
      continue;
    }

    f32 calcFrame;
    calcFrame = this->CalculateAllTimeAnimationFrame( pSlot->GetFrameCount(), hour, minute, second);
    f32 fixFrame1 = gfl2::math::Float::Floor( calcFrame, 2);

    calcFrame = pSlot->GetFrame();
    f32 fixFrame2 = gfl2::math::Float::Floor( calcFrame, 2);

    if( gfl2::math::IsSimilar( fixFrame1, fixFrame2) == false)
    {
      pSlot->SetFrame( fixFrame1);
    }
  }
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
 * @param isSmoothSlot true:補間用のスロット取得, false:通常のスロット取得
 * @return 指定したスロット番号、アニメーションのスロット
 */
gfl2::animation::AnimationSlotBase* EnvAccessor::GetAnimationSlot( u32 slotIndex, AnimationBit animationBit, bool isSmoothSlot)
{
  gfl2::animation::DrawEnvAnimationController::SlotType slotType = (isSmoothSlot)
    ? gfl2::animation::DrawEnvAnimationController::SLOT_TYPE_INTERP
    : gfl2::animation::DrawEnvAnimationController::SLOT_TYPE_NORMAL;

  if( animationBit & ANIMATION_BIT_LIGHT)
  {
    return m_AnimationController.GetLightSlot( slotIndex, slotType);
  }
  if( animationBit & ANIMATION_BIT_FOG )
  {
    return m_AnimationController.GetFogSlot( slotIndex, slotType);
  }
  if( animationBit & ANIMATION_BIT_BLOOM )
  {
    return m_AnimationController.GetGlareSlot( slotIndex, slotType);
  }
  GFL_ASSERT( 0);
  return NULL;
}

/*
 * @brief スロット取得
 * @param slotIndex スロット番号
 * @param animationType アニメーション種類
 * @param isSmoothSlot true:補間用のスロット取得, false:通常のスロット取得
 * @return 指定したスロット番号、アニメーションのスロット
 */
gfl2::animation::AnimationSlotBase* EnvAccessor::GetAnimationSlot( u32 slotIndex, AnimationType animationType, bool isSmoothSlot)
{
  gfl2::animation::DrawEnvAnimationController::SlotType slotType = (isSmoothSlot)
    ? gfl2::animation::DrawEnvAnimationController::SLOT_TYPE_INTERP
    : gfl2::animation::DrawEnvAnimationController::SLOT_TYPE_NORMAL;

  if( animationType == ANIMATION_TYPE_LIGHT)
  {
    return m_AnimationController.GetLightSlot( slotIndex, slotType);
  }
  if( animationType == ANIMATION_TYPE_FOG )
  {
    return m_AnimationController.GetFogSlot( slotIndex, slotType);
  }
  if( animationType == ANIMATION_TYPE_BLOOM )
  {
    return m_AnimationController.GetGlareSlot( slotIndex, slotType);
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

/*
 * @brief アニメーション処理高階関数
 * @param func アニメーションスロットが呼ぶ関数
 * @param arg0 funcの引数
 * @param slotIndex スロット番号
 * @param animatinBit アニメーション指定ビット
 */
template <typename F, typename A0>
void EnvAccessor::SetAnimationForeach( F func, A0 arg0, u32 slotIndex, AnimationBit animationBit)
{
  for( u32 i = 0; i < ANIMATION_TYPE_MAX; ++i)
  {
    if( (animationBit & (1 << i)) == false )
    {
      continue;
    }

    gfl2::animation::AnimationSlotBase* pSlot;
    pSlot = this->GetAnimationSlot( slotIndex, static_cast<AnimationType>( i), this->IsAnimationSmooth(static_cast<AnimationBit>( 1 << i)));

    if( pSlot == NULL)
    {
      continue;
    }

    if( pSlot->HasAnimation() == false)
    {
      continue;
    }

    (pSlot->*func)(arg0);
  }
}

/*
 * @brief アニメーション補間リクエスト
 * @param animatinBit アニメーション指定ビット
 * @param smoothFrame 補間フレーム数
 */
void EnvAccessor::RequestAnimationSmooth( AnimationBit animationBit, s32 smoothFrame)
{
  if( animationBit & ANIMATION_BIT_LIGHT)
  {
    m_AnimationController.LightInterpRequest(smoothFrame);
  }
  if( animationBit & ANIMATION_BIT_FOG )
  {
    m_AnimationController.FogInterpRequest(smoothFrame);
  }
  if( animationBit & ANIMATION_BIT_BLOOM )
  {
    m_AnimationController.GlareInterpRequest(smoothFrame);
  }
}

/*
 * @brief アニメーション補間リクエスト
 * @param animatinBit アニメーション指定ビット
 * @param smoothFrame 補間フレーム数
 */
bool EnvAccessor::IsAnimationSmooth( AnimationBit animationBit) const
{
  if( animationBit & ANIMATION_BIT_LIGHT)
  {
    return m_AnimationController.IsLightInterp();
  }
  if( animationBit & ANIMATION_BIT_FOG )
  {
    return m_AnimationController.IsFogInterp();
  }
  if( animationBit & ANIMATION_BIT_BLOOM )
  {
    return m_AnimationController.IsGlareInterp();
  }
  GFL_ASSERT( 0);
  return false;
}

GFL_NAMESPACE_END( System )
