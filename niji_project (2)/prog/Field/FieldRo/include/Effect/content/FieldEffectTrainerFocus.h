//======================================================================
/**
 * @file FieldEffectTrainerFocus.h
 * @date 2015/09/02 12:09:43
 * @author miyachi_soichi
 * @brief 地名表示
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EFFECT_TRAINERFOCUS_H_INCLUDED__
#define __FIELD_EFFECT_TRAINERFOCUS_H_INCLUDED__
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
 * @class EffectTrainerFocus
 * @brief フェード
 */
class EffectTrainerFocus : public IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectTrainerFocus);

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
  EffectTrainerFocus( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectTrainerFocus( void );

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
   *  @brief  開始
   */
  void StartIn( u32 frame );
  void StartOut( u32 frame );

  // IN状態か
  b32 IsIn( void ) const 
  {
    return ( m_eState == STATE_IN );
  }

private:

  enum
  {
    STATE_NONE,
    STATE_IN,
    STATE_OUT,
  };

  app::util::Heap                           m_appUtilHeap;
  Field::Effect::EffectLayout               *m_pLayout;
  Field::FieldWindow::FieldWindowManager    *m_pWindowManager;
  u32                                       m_eState;

};

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_EFFECT_TRAINERFOCUS_H_INCLUDED__