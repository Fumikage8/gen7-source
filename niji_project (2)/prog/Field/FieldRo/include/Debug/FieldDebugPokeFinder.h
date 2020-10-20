//======================================================================
/**
 * @file FieldDebugPokeFinder.h
 * @date 2015/10/01 19:04:46
 * @author miyachi_soichi
 * @brief PokeFinderデバッグ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_DEBUG_POKEFINDER_H_INCLUDED__
#define __FIELD_DEBUG_POKEFINDER_H_INCLUDED__
#pragma once
// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <Debug/include/gfl2_DebugWinItemTypes.h>
// param
#include "Field/FieldRo/include/TrialModel/FieldFinderModel_define.h"
#if PM_DEBUG

// 前方宣言
GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Camera)
GFL_NAMESPACE_BEGIN(Controller)
class ControllerTypePokeFinderForPlacement;
GFL_NAMESPACE_END(Controller)
GFL_NAMESPACE_END(Camera)
GFL_NAMESPACE_BEGIN(TrialModel)
class FinderModel;
GFL_NAMESPACE_END(TrialModel)
GFL_NAMESPACE_END(Field)

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

/**
 *  @class  DebugPokeFinder
 *  @brief  PokeFinderデバッグ
 */
class DebugPokeFinder
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugPokeFinder);

public:
  /**
   *  @brief  コンストラクタ
   */
  DebugPokeFinder( void );

  /**
   *  @brief  デストラクタ
   */
  ~DebugPokeFinder( void );

  /**
   *  @brief  初期化
   */
  void Initialize( gfl2::heap::HeapBase *pHeap, gfl2::debug::DebugWinGroup *root, gfl2::str::MsgData* pMsgData );

  /**
   *  @brief  破棄
   */
  void Terminate( void );

  /**
   *  @brief  ヒープの取得
   */
  gfl2::heap::HeapBase *GetHeap( void ){ return m_pHeap; }

  /**
   *  @brief  メッセージデータの取得
   */
  gfl2::str::MsgData* GetMessageData( void ){ return m_pMsgData; }

public:
  // 調整パラメータ
  gfl2::math::Vector3 m_pos;
  f32 m_rot;
  gfl2::math::Vector3 m_poke_pos;
  f32 m_poke_rot;

  s32 m_mons;
  s32 m_form;
  s32 m_sex;

  u32 m_appeal01;
  u32 m_appeal02;
  u32 m_appeal03;
  u32 m_appeal04;
  u32 m_appeal05;

  Field::Camera::Controller::ControllerTypePokeFinderForPlacement *m_pCamera;
  Field::TrialModel::FinderModel *m_pMons1;
  Field::TrialModel::FinderModel *m_pMons2;
  Field::TrialModel::FinderPathData *m_pWalkerA;
  Field::TrialModel::FinderPathData *m_pWalkerB;
  Field::TrialModel::FinderStatueData *m_pStatue;
  Field::TrialModel::FinderOnewayData *m_pOneway;

  f32 m_score_total;
  f32 m_score_basepoint_1;
  f32 m_score_motion_1;
  f32 m_score_dir_1;
  f32 m_score_screen_1;
  f32 m_score_out_1;
  f32 m_score_basepoint_2;
  f32 m_score_motion_2;
  f32 m_score_dir_2;
  f32 m_score_screen_2;
  f32 m_score_out_2;
  f32 m_score_spot;

  f32 m_search_deg;
  f32 m_search_frame;
  f32 m_search_wait;
  u32 m_search_count;
  f32 m_search_posy;
  f32 m_search_posz;

  f32 m_gyro_play;
  f32 m_gyro_power;
  f32 m_gyro_range;

  s32 m_debug_focus;
  f32 m_focus_offset;
  f32 m_focus_width;
  f32 m_focus_fpow;
  f32 m_focus_bpow;
  f32 m_focus_powstep;

  u32 m_cam_ver;
  u32 m_oneway_delay;

  bool m_bExec;

  s32 m_index;
  f32 m_pos_x;
  f32 m_pos_y;
  f32 m_pos_z;
  gfl2::math::Matrix34 m_spotMtx;

  s32 m_overwrite;
  gfl2::math::Vector3 m_rare[6];

private:
  gfl2::heap::HeapBase *m_pHeap;
  gfl2::str::MsgData*  m_pMsgData;

}; // class DebugPokeFinder

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // PM_DEBUG
#endif // __FIELD_DEBUG_POKEFINDER_H_INCLUDED__
