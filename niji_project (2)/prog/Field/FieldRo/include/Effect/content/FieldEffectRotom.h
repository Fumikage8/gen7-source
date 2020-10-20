//======================================================================
/**
 * @file FieldEffectRotom.h
 * @date 2015/11/09 12:56:19
 * @author saita_kazuki
 * @brief ロトム図鑑説明1枚絵表示
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EFFECT_ROTOM_H_INCLUDED__
#define __FIELD_EFFECT_ROTOM_H_INCLUDED__
#pragma once

// app
#include "AppLib/include/Util/app_util_heap.h"
// Effect
#include "Field/FieldRo/include/Effect/FieldEffectBase.h"
#include "Field/FieldRo/include/Effect/FieldEffectLayout.h"

class WordSetLoader;
GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(FieldWindow)
class FieldWindowManager;
GFL_NAMESPACE_END(FieldWindow)
GFL_NAMESPACE_BEGIN(Effect)

/**
 * @class EffectRotom
 * @brief フェード
 */
class EffectRotom : public IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectRotom);

public:
  /**
   *  @brief  初期化構造体
   */
  struct SetupData
  {
    void                                      *pResource;
    gfl2::heap::HeapBase                      *pHeap;
    Field::FieldWindow::FieldWindowManager    *pWinMgr;
    WordSetLoader                             *pWordSetLoader;
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  EffectRotom( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectRotom( void );

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
  void Start();

  /**
   *  @brief  終了
   */
  void End( void );

  /**
   *  @brief  アニメーションを次の状態に進める
   */
  void NextAnimation();

private:

  app::util::Heap                           m_appUtilHeap;
  Field::Effect::EffectLayout               *m_pLayout;
  Field::FieldWindow::FieldWindowManager    *m_pWindowManager;

};

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_EFFECT_ROTOM_H_INCLUDED__

