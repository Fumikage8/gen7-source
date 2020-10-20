//======================================================================
/**
 * @file FieldEffectGetItem.h
 * @date 2016/01/19
 * @author saita_kazuki
 * @brief アイテム取得1枚絵
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EFFECT_GET_ITEM_H_INCLUDED__
#define __FIELD_EFFECT_GET_ITEM_H_INCLUDED__
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
 * @class EffectGetItem
 * @brief フェード
 */
class EffectGetItem : public IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectGetItem);

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

  static const u32 INDEX_MAX = 8; ///< 絵の最大数

public:
  /**
   *  @brief  コンストラクタ
   */
  EffectGetItem( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectGetItem( void );

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
   *  @param  index 番号
   */
  void Start( u32 index);

  /**
   *  @brief  終了
   */
  void End( void );

  /**
   *  @brief  絵を変更する
   *  @param  index 番号
   */
  void Change( u32 index);

  /**
   *  @brief  絵の番号を取得する
   *  @return 絵の番号
   */
  u32 GetIndex() const ;

private:

  bool IsLayoutInitialized();

private:

  app::util::Heap                           m_appUtilHeap;
  Field::Effect::EffectLayout               *m_pLayout;
  Field::FieldWindow::FieldWindowManager    *m_pWindowManager;
  u32                                       m_animationIndex;

};

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_EFFECT_GET_ITEM_H_INCLUDED__

