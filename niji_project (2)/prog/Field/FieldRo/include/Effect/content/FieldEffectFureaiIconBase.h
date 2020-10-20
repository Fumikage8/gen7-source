//======================================================================
/**
* @file FieldEffectFureaiIconBase.h
* @date 2017/02/13 13:08:45
* @author kawazoe_shinichi
* @brief ふれあい表現用のアイコンエフェクト
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if !defined(__FIELD_EFFECT_FUREAI_ICON_BASE_H__)
#define __FIELD_EFFECT_FUREAI_ICON_BASE_H__
#pragma once

// Field
#include "Field/FieldRo/include/Effect/content/FieldEffectBallonIcon.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )

class EffectFureaiIconBase : public EffectBallonIcon
{
  //! コピー禁止
  GFL_FORBID_COPY_AND_ASSIGN(EffectFureaiIconBase);

public:

  /**
  *  初期化構造体
  */
  struct SetupData
  {
    EffectBallonIcon::SetupData baseSetup;
    //u32          nSe;            //!< 再生SEID
    u32          type;

    //! ctor
    SetupData()
      : baseSetup()
      //, nSe(0)
      , type(0)
    {
    }
  };

  enum Mode
  {
    MODE_NORMAL = 0,
    MODE_KEEP,      // 保持され続ける
    MODE_CLOSE,     // WAIT中の場合にこのモードに変更されると即時終了処理に入る※OPEN中はWAIT状態に切り替わるまで待機
  };
  enum State
  {
    STATE_NONE,
    STATE_OPEN,
    STATE_WAIT,
    STATE_WAIT_FOR_REQUEST,   // Closeリクエスト待ち（スクリプト目線で見るとCLOSEモードへの切り替わり待ち）
    STATE_WAIT_FOR_ZZZ,
    STATE_END,
  };

  /**
   *  @brief  コンストラクタ
   */
  EffectFureaiIconBase(void);

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectFureaiIconBase(void);

  /**
   *  @brief  初期化処理：座標指定
   *  @param  setupData 初期化構造体
   */
  virtual void Initialize( EffectFureaiIconBase::SetupData &setupData );

  /**
   *  @brief  破棄処理
   */
  virtual bool Terminate( void );

  /**
   *  @brief  各エフェクトで必要な破棄作業
   */
  virtual void Delete( void );


  /**
  *  @brief  スクリプト用のステート取得
  */
  u32 GetStateForScript(void);
  /**
   *  @brief  モード設定
   */
  void SetMode( u32 mode ){ m_eMode = mode; }

  /**
  *  @brief  Visible設定
  */
  void SetVisible(b32 visible){ m_IsVisible = visible; }

  /**
   *  @brief  Closeリクエスト
   */
  void RequestClose( void );

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

  s32 m_type;
  u32 m_eCurrentState;
  u32 m_eMode;

  u32 m_FrameCount;
  u32 m_MaxWaitFrame;
  b8  m_IsVisible;

};

GFL_NAMESPACE_END( Effect );
GFL_NAMESPACE_END( Field );

#endif // __FIELD_EFFECT_FUREAI_ICON_BASE_H__
