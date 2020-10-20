/**
 *  GAME FREAK inc.
 *
 *  @file   FieldEffectQuestion.h
 *  @brief  フィールドエフェクト：エクスクラメーション１
 *  @author miyachi_soichi
 *  @date   2015.06.10
 */

#if !defined(__FIELD_EFFECT_QUESTION_H__)
#define __FIELD_EFFECT_QUESTION_H__
#pragma once

// Field
#include "Field/FieldRo/include/Effect/content/FieldEffectBallonIcon.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )

class EffectQuestion : public EffectBallonIcon
{
  //! コピー禁止
  GFL_FORBID_COPY_AND_ASSIGN( EffectQuestion );

public:
  /**
   *  @brief  コンストラクタ
   */
  EffectQuestion( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectQuestion( void );

  /**
   *  @brief  初期化処理：座標指定
   *  @param  setupData 初期化構造体
   */
  virtual void Initialize( EffectBallonIcon::SetupData &setupData );

  /**
   *  @brief  破棄処理
   */
  virtual bool Terminate( void );

  /**
   *  @brief  各エフェクトで必要な破棄作業
   */
  virtual void Delete( void );

  // リソースID取得
  virtual u32 GetModelDataID( void );
  virtual u32 GetMotionDataID( void );

protected:
  /**
   *  @brief  更新処理
   */
  virtual void updateNormal( void );

}; // class EffectQuestion : public IEffectBase

GFL_NAMESPACE_END( Effect );
GFL_NAMESPACE_END( Field );

#endif // !defined(__FIELD_EFFECT_QUESTION_H__)
