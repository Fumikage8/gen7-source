/**
 *  GAME FREAK inc.
 *
 *  @file   FieldEffectExclamation1.h
 *  @brief  フィールドエフェクト：エクスクラメーション１
 *  @author miyachi_soichi
 *  @date   2015.06.10
 */

#if !defined(__FIELD_EFFECT_EXCLAMATION1_H__)
#define __FIELD_EFFECT_EXCLAMATION1_H__
#pragma once

// Field
#include "Field/FieldRo/include/Effect/content/FieldEffectBallonIcon.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )

class EffectExclamation1 : public EffectBallonIcon
{
  //! コピー禁止
  GFL_FORBID_COPY_AND_ASSIGN( EffectExclamation1 );

public:

  enum Mode
  {
    MODE_NORMAL,
    MODE_KEEP, // 保持され続ける
  };
  enum State
  {
    STATE_NONE,
    STATE_OPEN,
    STATE_END,
  };

  /**
   *  @brief  コンストラクタ
   */
  EffectExclamation1( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectExclamation1( void );

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

  /**
   *  @brief  モード設定
   */
  void SetMode( u32 mode ){ m_eMode = mode; }

  /**
   *  @brief  Openリクエスト
   *
   *  @param  anime アニメーション再生を行うならtrue
   */
  void RequestOpenForKeepMode( b32 anime );

  /**
   *  @brief  Closeリクエスト
   *
   *  @param  anime アニメーション再生を行うならtrue
   */
  void RequestCloseForKeepMode( b32 anime );

  // リソースID取得
  virtual u32 GetModelDataID( void );
  virtual u32 GetMotionDataID( void );

protected:
  /**
   *  @brief  更新処理
   */
  virtual void updateNormal( void );

  /**
   *  @brief  モーション作成
   */
  void createMotion2( void *pOpen, void *pClose );

  gfl2::renderingengine::scenegraph::resource::ResourceNode       *m_pMotionResourceRootNode_Open;
  gfl2::renderingengine::scenegraph::resource::ResourceNode       *m_pMotionResourceRootNode_Close;

  u32 m_eState;
  u32 m_eMode;

}; // class EffectExclamation1 : public IEffectBase

GFL_NAMESPACE_END( Effect );
GFL_NAMESPACE_END( Field );

#endif // !defined(__FIELD_EFFECT_EXCLAMATION1_H__)
