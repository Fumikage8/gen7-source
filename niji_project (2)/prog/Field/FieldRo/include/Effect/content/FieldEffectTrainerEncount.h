//======================================================================
/**
 * @file FieldEffectTrainerEncount.h
 * @date 2015/09/11 15:25:33
 * @author miyachi_soichi
 * @brief エフェクト：トレーナーエンカウント
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EFFECT_TRAINER_ENCOUNT_H_INCLUDED__
#define __FIELD_EFFECT_TRAINER_ENCOUNT_H_INCLUDED__
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
 * @class EffectTrainerEncount
 * @brief フェード
 */
class EffectTrainerEncount : public IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectTrainerEncount);

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
  EffectTrainerEncount( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectTrainerEncount( void );

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
   *  @brief  アニメーション開始
   */
  void StartAnimation( gfl2::math::VEC2 pos, f32 fRot,  u32 se_id );

private:
  app::util::Heap                           m_appUtilHeap;
  Field::Effect::EffectLayout               *m_pLayout;
  Field::FieldWindow::FieldWindowManager    *m_pWindowManager;

  u32                                       m_nAnimeID;
};

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_EFFECT_TRAINER_ENCOUNT_H_INCLUDED__
