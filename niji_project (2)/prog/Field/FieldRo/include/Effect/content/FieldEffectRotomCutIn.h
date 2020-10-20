//======================================================================
/**
 * @file FieldEffectRotomCutIn.h
 * @date 2017/02/07 21:25:39
 * @author kawazoe_shinichi
 * @brief ロトムカットインエフェクト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EFFECT_ROTOM_CUT_IN_H_INCLUDED__
#define __FIELD_EFFECT_ROTOM_CUT_IN_H_INCLUDED__
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
 * @class EffectRotomCutIn
 * @brief ロトムカットイン
 */
class EffectRotomCutIn : public IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectRotomCutIn);

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
  EffectRotomCutIn(void);

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectRotomCutIn(void);

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
   *  @brief  終了しているか.
   */
  virtual bool IsAnimationLastFrame( void );

  /**
   *  @brief  各エフェクトで必要な破棄作業
   */
  virtual void Delete( void );

  /**
   *  @brief  開始
   */
  void Start( void );

  /**
   *  @brief  終了
   */
  void End( void );


private:

  bool IsLayoutInitialized();

private:

  app::util::Heap                           m_appUtilHeap;
  Field::Effect::EffectLayout               *m_pLayout;
  Field::FieldWindow::FieldWindowManager    *m_pWindowManager;

};

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_EFFECT_ROTOM_CUT_IN_H_INCLUDED__

