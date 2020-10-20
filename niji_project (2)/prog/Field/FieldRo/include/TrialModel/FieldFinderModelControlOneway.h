//======================================================================
/**
 * @file FieldFinderModelControlOneway.h
 * @date 2015/12/08 17:43:49
 * @author miyachi_soichi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_FINDERMODEL_CONTROL_ONEWAY_H_INCLUDED__
#define __FIELD_FINDERMODEL_CONTROL_ONEWAY_H_INCLUDED__
#pragma once
// gfl2
#include <math/include/gfl2_PathController.h>
// base
#include "Field/FieldRo/include/TrialModel/FieldFinderModelControl.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(TrialModel)

/**
 *  @class  FinderModelControlOneway
 *  @brief  一通挙動
 */
class FinderModelControlOneway : public FinderModelControl
{
  GFL_FORBID_COPY_AND_ASSIGN(FinderModelControlOneway);

public:
  /**
   *  @brief  コンストラクタ
   */
  FinderModelControlOneway( const Field::TrialModel::FinderOnewayData *pOnewayData );

  /**
   *  @brief  初期化
   */
  virtual void Initialize( FinderModel *parent, gfl2::heap::HeapBase *pHeap );

  /**
   *  @brief  破棄
   */
  virtual void Terminate( void );

  /**
   *  @brief  更新
   */
  virtual void UpdateBeforeAnimation( void );
  virtual void UpdateAfterAnimation( void );

  /**
   *  @brief  ポケモンを呼ぶ
   */
  virtual void RequestPokeCall( void );

private:
  /**
   *  @brief  パス作成
   */
  void createPath( void );

  /**
   *  @brief  更新：待機
   */
  void updateWait_Before( void );
  void updateWait_After( void );

  /**
   *  @brief  更新：移動
   */
  void updateMove_Before( void );
  void updateMove_After( void );

  /**
   *  @brief  変更：終了
   */
  void changeEnd( void );

  /**
   *  @brief  ポケモンを呼ぶ
   */
  void updateCall( void );

private:
  enum
  {
    STATE_WAIT,
    STATE_MOVE,
    STATE_END,
  };
  enum
  {
    CALL_NONE,
    CALL_REQUEST,
    CALL_EXEC,
    CALL_WAIT,
    CALL_RESET,
  };

  gfl2::heap::HeapBase                        *m_pHeap;
  gfl2::math::PathController                  *m_pPathControl;
  const Field::TrialModel::FinderOnewayData   *m_pOnewayData;

  u32                                         m_eState;
  u32                                         m_Delay;
  u32                                         m_eCall;
  u32                                         m_nCallWait;

  f32                                         m_fProgress;

#if PM_DEBUG
public:
  u32 m_debug_delayMax;
  void debug_onemore( void );
  u32 debug_delay( void );
#endif
}; // class FinderModelControlOneway

GFL_NAMESPACE_END(TrialModel)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_FINDERMODEL_CONTROL_ONEWAY_H_INCLUDED__
