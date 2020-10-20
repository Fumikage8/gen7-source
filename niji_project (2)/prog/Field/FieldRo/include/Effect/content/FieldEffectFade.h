﻿//======================================================================
/**
 * @file FieldEffectFade.h
 * @date 2015/08/26 18:05:57
 * @author miyachi_soichi
 * @brief イベント用フェード
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EFFECT_FADE_H_INCLUDED__
#define __FIELD_EFFECT_FADE_H_INCLUDED__
#pragma once

// app
#include "AppLib/include/Util/app_util_heap.h"
// Effect
#include "Field/FieldRo/include/Effect/FieldEffectBase.h"
#include "Field/FieldRo/include/Effect/FieldEffectLayout.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(FieldWindow)
class FieldWindowManager;
GFL_NAMESPACE_END(FieldWindow)
GFL_NAMESPACE_BEGIN(Effect)

/**
 * @class EffectFade
 * @brief フェード
 */
class EffectFade : public IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectFade);

public:
  /**
   *  @brief  初期化構造体
   */
  struct SetupData
  {
    void                                      *pResource;
    gfl2::heap::HeapBase                      *pHeap;
    Field::FieldWindow::FieldWindowManager    *pWinMgr;
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  EffectFade( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectFade( void );

  /**
   *  @brief  初期化処理
   */
  virtual void Initialize( SetupData &setupData );


  /**
   *  @brief  破棄処理
   */
  virtual bool Terminate( void );

  /**
   *  @brief  更新処理
   */
  virtual void updateNormal( void );

  /**
   *  @brief  各エフェクトで必要な破棄作業
   */
  virtual void Delete( void );

  /**
   *  @brief  終了しているか.
   */
  virtual bool IsAnimationLastFrame( void );

  /**
   *  @brief  フェード
   */
  void RequestOut( gfl2::math::Vector4 *start, gfl2::math::Vector4 *end, s32 life );
  void RequestIn( s32 life );
  void SetColor( void );

private:
  enum
  {
    STATE_NONE,
    STATE_OUT,
    STATE_IN,
  };

  app::util::Heap                           m_appUtilHeap;
  Field::Effect::EffectLayout               *m_pLayout;
  Field::FieldWindow::FieldWindowManager    *m_pWindowManager;
  u32                                       m_nState;
  s32                                       m_nLifeNow;
  s32                                       m_nLifeMax;
  gfl2::math::Vector4                       m_ColorNow;
  gfl2::math::Vector4                       m_ColorAdd;
  gfl2::math::Vector4                       m_ColorEnd;
};

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_EFFECT_FADE_H_INCLUDED__
