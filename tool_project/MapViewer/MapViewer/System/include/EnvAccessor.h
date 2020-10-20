//===================================================================
/**
 * @file    EnvAccessor.h
 * @brief   DrawEnv共通処理
 * @author  saita_kazuki
 * @date    2015.05.11
 */
//===================================================================

#if !defined( ENV_ACCESSOR_H_INCLUDED )
#define ENV_ACCESSOR_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <animation/include/gfl2_DrawEnvAnimationController.h>

GFL_NAMESPACE_BEGIN( System )

/**
 * @class DrawEnv共通処理クラス
 */
class EnvAccessor
{
  /**
   * @briefコピーコンストラクタ＋代入禁止
   */
  GFL_FORBID_COPY_AND_ASSIGN( EnvAccessor );

public:

  /**
   * @brief アニメーション種類
   */
  enum AnimationType
  {
    ANIMATION_TYPE_LIGHT,   ///< ライト
    ANIMATION_TYPE_FOG,     ///< フォグ
    ANIMATION_TYPE_BLOOM,   ///< ブルーム
    ANIMATION_TYPE_MAX,
  };

  /**
   * @brief アニメーション操作用ビット
   */
  enum AnimationBit
  {
    ANIMATION_BIT_LIGHT   = 1 << ANIMATION_TYPE_LIGHT,      ///< ライト
    ANIMATION_BIT_FOG     = 2 << ANIMATION_TYPE_FOG,        ///< フォグ
    ANIMATION_BIT_BLOOM   = 3 << ANIMATION_TYPE_BLOOM       ///< ブルーム
  };

public:

  /**
   * @brief コンストラクタ
   */
  EnvAccessor();

  /**
   * @brief デストラクタ
   */
  virtual ~EnvAccessor();

  /**
   * @brief 初期化
   * @param pHeap DrawEnvアニメーションから使用される(スロット取得時のメモリ確保など)
   * @param pDrawEnvNode DrawEnvノード(保持するだけで破棄はしません)
   */
  void Initialize( gfl2::heap::HeapBase* pHeap, gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode);

  /**
   * @brief 終了
   */
  void Terminate();

  /**
   * @brief 更新
   */
  void Update();

  /**
   * @brief アニメーション変更
   * @param pMotionResourceRootNode DrawEnvのアニメーションリソースノード(保持するだけで破棄はしません)
   * @param slotIndex スロット番号
   * @param animationBit どのアニメーションかを指定するビットフラグ
   */
  void ChangeAnimation( gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionResourceRootNode, u32 slotIndex, AnimationBit animationBit);

  /**
   * @brief アニメーションループ設定
   * @param isLoop trueでループ
   * @param slotIndex スロット番号
   * @param animationBit どのアニメーションかを指定するビットフラグ
   */
  void SetAnimationLoop( bool isLoop, u32 slotIndex, AnimationBit animationBit);

  /**
   * @brief アニメーションステップフレーム設定
   * @param stepFrame ステップフレーム
   * @param slotIndex スロット番号
   * @param animationBit どのアニメーションかを指定するビットフラグ
   */
  void SetStepFrame( f32 stepFrame, u32 slotIndex, AnimationBit animationBit);

  /**
   * @brief アニメーションフレーム設定
   * @param frame フレーム数
   * @param slotIndex スロット番号
   * @param animationBit どのアニメーションかを指定するビットフラグ
   */
  void SetFrame( f32 frame, u32 slotIndex, AnimationBit animationBit);

  /**
   * @brief アニメーションフレーム設定
   * @param hour 時間
   * @param minute 分
   * @param second 秒
   * @param slotIndex スロット番号
   * @param animationBit どのアニメーションかを指定するビットフラグ
   */
  void SetFrame( s32 hour, s32 minute, s32 second, u32 slotIndex, AnimationBit animationBit);

  /**
   * @brief 保持しているDrawEnvNodeの取得
   * @return Initializeで受け取ったDrawEnvNode
   */
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* GetDrawEnvNode() const ;

private:

  gfl2::animation::AnimationSlotBase* GetAnimationSlot( u32 slotIndex, AnimationBit animationBit);
  f32 CalculateAllTimeAnimationFrame( f32 frameMax, s32 hour, s32 minute, s32 second) const ;

private:

  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*   m_pDrawEnvNode;
  gfl2::animation::DrawEnvAnimationController                 m_AnimationController;

};

GFL_NAMESPACE_END( System )

#endif // ENV_ACCESSOR_H_INCLUDED

