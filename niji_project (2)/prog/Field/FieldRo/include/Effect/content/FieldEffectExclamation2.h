/**
 *  GAME FREAK inc.
 *
 *  @file   FieldEffectExclamation2.h
 *  @brief  フィールドエフェクト：エクスクラメーション２
 *  @author miyachi_soichi
 *  @date   2015.06.10
 */

#if !defined(__FIELD_EFFECT_EXCLAMATION2_H__)
#define __FIELD_EFFECT_EXCLAMATION2_H__
#pragma once

// Field
#include "Field/FieldRo/include/Effect/content/FieldEffectBallonIcon.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )

class EffectExclamation2 : public EffectBallonIcon
{
  //! コピー禁止
  GFL_FORBID_COPY_AND_ASSIGN( EffectExclamation2 );

public:

  enum Mode
  {
    MODE_NORMAL,
    MODE_WAIT,
    MODE_KEEP, // 保持され続ける
  };
  enum State
  {
    STATE_NONE,
    STATE_OPEN,
    STATE_END,
  };
public:
  /**
   *  @brief  コンストラクタ
   */
  EffectExclamation2( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectExclamation2( void );

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
   *  @brief  Closeリクエスト
   */
  void RequestClose( void ){ m_bWait = false; }

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

  // 再生速度変更
  void SetStepFrame( f32 stepFrame );

protected:
  /**
   *  @brief  更新処理
   */
  virtual void updateNormal( void );

  /**
   *  @brief  モーション作成
   */
  void createMotion2( void *pOpen, void *pClose );

protected:
  gfl2::renderingengine::scenegraph::resource::ResourceNode       *m_pMotionResourceRootNode_Open;
  gfl2::renderingengine::scenegraph::resource::ResourceNode       *m_pMotionResourceRootNode_Close;

  u32 m_eState;
  u32 m_eMode;
  bool m_bWait;
  f32 m_StepFrame;

}; // class EffectExclamation2 : public IEffectBase

GFL_NAMESPACE_END( Effect );
GFL_NAMESPACE_END( Field );

#endif // !defined(__FIELD_EFFECT_EXCLAMATION2_H__)
