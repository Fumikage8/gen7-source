//======================================================================
/**
 * @file FieldFinderModelControl.h
 * @date 2015/10/08 13:00:14
 * @author miyachi_soichi
 * @brief ファインダーモデル用操作コンポーネント
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_FINDERMODEL_CONTROL_H_INCLUDED__
#define __FIELD_FINDERMODEL_CONTROL_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
// Finder
#include "Field/FieldRo/include/TrialModel/FieldFinderModel_define.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(TrialModel)
class FinderModel;

/**
 *  @class  FinderModelControl
 *  @brief  ファインダーモデル用操作コンポーネント
 */
class FinderModelControl
{
  GFL_FORBID_COPY_AND_ASSIGN(FinderModelControl);

public:
  // ポケモンを呼ぶ待機時間2(秒*フレーム変換)
  static const u32 CALL_WAIT_FRAME = 2 * 30;

public:
  /**
   *  @brief  コンストラクタ
   */
  FinderModelControl( void ) : m_pParent( NULL )
  {
  }

  /**
   *  @brief  初期化
   */
  virtual void Initialize( FinderModel *parent, gfl2::heap::HeapBase *pHeap ) = 0;

  /**
   *  @brief  解放
   */
  virtual void Terminate( void ) = 0;

  /**
   *  @brief  更新
   */
  virtual void UpdateBeforeAnimation( void ) = 0;
  virtual void UpdateAfterAnimation( void ) = 0;

  /**
   *  @brief  ポケモンを呼ぶ
   */
  virtual void RequestPokeCall( void ) = 0;

protected:
  FinderModel   *m_pParent;

};

GFL_NAMESPACE_END(TrialModel)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_FINDERMODEL_CONTROL_H_INCLUDED__
