//======================================================================
/**
 * @file FieldFinderModelControlWalker.h
 * @date 2015/10/08 15:29:27
 * @author miyachi_soichi
 * @brief 挙動：移動ありファインダーモデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_FINDERMODEL_CONTROL_WALKER_H_INCLUDED__
#define __FIELD_FINDERMODEL_CONTROL_WALKER_H_INCLUDED__
#pragma once
// gfl2
#include <math/include/gfl2_PathController.h>
// base
#include "Field/FieldRo/include/TrialModel/FieldFinderModelControl.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(TrialModel)

/**
 *  @class  FinderModelControlWalker
 *  @brief  移動なし挙動
 */
class FinderModelControlWalker : public FinderModelControl
{
  GFL_FORBID_COPY_AND_ASSIGN(FinderModelControlWalker);

public:
  enum
  {
    TARGET_CURRENT = 0,    // 現在
    TARGET_NEXT,           // その次
    TARGET_NEXTNEXT,       // 次の次
    TARGET_MAX,
  };
  enum State
  {
    STATE_WALK,
    STATE_APPEAL,
    STATE_CUSTOM,
  };
  enum
  {
    CALL_NONE,
    CALL_REQUEST,
    CALL_EXEC,
    CALL_WAIT,
    CALL_RESET,
  };
public:
  /**
   *  @brief  コンストラクタ
   */
  FinderModelControlWalker( const FinderPathData *pPathData );

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
   *  @brief  ネクネクを設定する
   */
  void calcNextNext( void );

  /**
   *  @brief  パスを生成
   */
  void createPath( void );

  /**
   *  @brief  移動：パス仕様
   */
  void updateMoveTypePath_Before( void );
  void updateMoveTypePath_After( void );

  /**
   *  @brief  アピール：モーション
   */
  void updateAppeal( void );

  /**
   *  @brief  アピール：カスタム
   */
  void updateCustom( void );

  /**
   *  @brief  アピール地点の計算
   *  @param  old_pos   更新前の座標
   *  @param  new_pos   更新後の座標
   *  @return アピール地点の有無
   */
  bool checkAppealPoint( const gfl2::math::Vector3 &old_pos, const gfl2::math::Vector3 &new_pos );

  /**
   *  @brief  アピール挙動へ
   */
  void changeAppeal( void );

  /**
   *  @brief  カスタム挙動：うたたね
   */
  void updateCustomDrowse( void );

  /**
   *  @brief  カメラ注視
   */
  void updateCustomCamera( void );

  /**
   *  @brief  キョロキョロ
   */
  void updateCustomSearch( void );

  /**
   *  @brief  キョロキョロ位置計算
   */
  void calcSearchPosition( void );

  /**
   *  @brief  ポケモンを呼ぶ
   */
  void updateCall( void );

private:
  gfl2::heap::HeapBase        *m_pHeap;
  gfl2::math::PathController  *m_pPathControl;
  const FinderPathData        *m_pPathData;

  gfl2::math::Vector3   m_PathPos[TARGET_MAX];

  State m_eState;
  s32 m_nStep;
  s32 m_nTarget[TARGET_MAX];
  u32 m_nAppealAnimNo;
  f32 m_fSpdRatio;
  f32 m_fProgress;
  u32 m_SearchCnt;
  u32 m_nWait;
  u32 m_eCall;
  u32 m_nCallWait;

  gfl2::math::Vector3 m_vSearchPos;
  f32 m_searchRad;
  f32 m_searchRad_Max;
  f32 m_searchRad_add;
  f32 m_searchLife;

  bool m_bAppeal;
  bool m_bCall;

#if PM_DEBUG
public:
  s32 debug_getPrevWayPoint1( s32 wp1 );
  s32 debug_getNextWayPoint1( s32 wp1 );
  s32 debug_getPrevWayPoint2( s32 wp1, s32 wp2 );
  s32 debug_getNextWayPoint2( s32 wp1, s32 wp2 );
  s32 debug_getPrevWayPoint3( s32 wp1, s32 wp2, s32 wp3 );
  s32 debug_getNextWayPoint3( s32 wp1, s32 wp2, s32 wp3 );
  void debug_createPath( s32 wp1, s32 wp2, s32 wp3 );

#endif
}; // class FinderModelControlWalker

GFL_NAMESPACE_END(TrialModel)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_FINDERMODEL_CONTROL_WALKER_H_INCLUDED__
