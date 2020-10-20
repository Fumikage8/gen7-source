//======================================================================
/**
 * @file FieldFinderModelControlStatue.h
 * @date 2015/10/08 14:32:12
 * @author miyachi_soichi
 * @brief 挙動：移動なしファインダーモデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_FINDERMODEL_CONTROL_STATUE_H_INCLUDED__
#define __FIELD_FINDERMODEL_CONTROL_STATUE_H_INCLUDED__
#pragma once

#include "Field/FieldRo/include/TrialModel/FieldFinderModelControl.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(TrialModel)

/**
 *  @class  FinderModelControlStatue
 *  @brief  移動なし挙動
 */
class FinderModelControlStatue : public FinderModelControl
{
  GFL_FORBID_COPY_AND_ASSIGN(FinderModelControlStatue);

public:
  /**
   *  @brief  コンストラクタ
   */
  FinderModelControlStatue( const Field::TrialModel::FinderStatueData *pStatueData );

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
  virtual void UpdateAfterAnimation( void ){}

  /**
   *  @brief  ポケモンを呼ぶ
   */
  virtual void RequestPokeCall( void );

private:
  /**
   *  @brief  待機挙動
   */
  void updateWait( void );

  /**
   *  @brief  アピール
   */
  void updateAppeal( void );

  /**
   *  @brief  カスタム
   */
  void updateCustom( void );

  /**
   *  @brief  アピール挙動へ
   */
  void changeAppeal( void );

  /**
   *  @brief  うたたね
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

  u32 calcCoolTime( void );

  /**
   *  @brief  ポケモンを呼ぶ
   */
  void updateCall( void );

private:
  const Field::TrialModel::FinderStatueData *m_pStatueData;

  enum
  {
    STATE_WAIT,
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
  PokeTool::MODEL_ANIME_POKE_FINDER  m_eWaitAnim;

  u32 m_eState;
  u32 m_nStep;
  u32 m_nWait;
  u32 m_nAppealAnimNo;
  u32 m_CoolTime;
  u32 m_SearchCnt;
  u32 m_Position;
  u32 m_eCall;
  u32 m_nCallWait;

  gfl2::math::Vector3 m_vSearchPos;
  f32 m_searchRad;
  f32 m_searchRad_Max;
  f32 m_searchRad_add;
  f32 m_searchLife;

  bool m_bAppeal;

#if PM_DEBUG
public:
  u32 debug_GetPosition( void ){ return m_Position; }
  void debug_SetPosition( u32 idx );

#endif
}; // class FinderModelControlStatue

GFL_NAMESPACE_END(TrialModel)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_FINDERMODEL_CONTROL_STATUE_H_INCLUDED__
