//=============================================================================
/**
 * @file pml_WazaTarget.h
 * @date 2016/05/30 15:15:26
 * @author obata_toshihiro
 * @brief 技の対象範囲
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 *
 * @note Pawn等のスクリプトから参照できるように単体ヘッダとしている
 */
//=============================================================================
#if !defined PML_WAZATARGET_H_INCLUDED
#define PML_WAZATARGET_H_INCLUDED
#pragma once

#if !defined PAWN_COMPILER
#include <macro/include/gfl2_Macros.h>
#endif

#if !defined PAWN_COMPILER
GFL_NAMESPACE_BEGIN( pml )
GFL_NAMESPACE_BEGIN( wazadata )
#endif


//-------------------------------------------------------------------------
/**
 * @brief ワザの効果範囲
 */
//-------------------------------------------------------------------------
enum WazaTarget {

  // ポケモン対象
  TARGET_OTHER_SELECT,       ///< 通常ポケ（１体選択）
  TARGET_FRIEND_USER_SELECT, ///< 自分を含む味方ポケ（１体選択）
  TARGET_FRIEND_SELECT,      ///< 自分以外の味方ポケ（１体選択）
  TARGET_ENEMY_SELECT,       ///< 相手側ポケ（１体選択）
  TARGET_OTHER_ALL,          ///< 自分以外の全ポケ
  TARGET_ENEMY_ALL,          ///< 相手側全ポケ
  TARGET_FRIEND_ALL,         ///< 味方側全ポケ
  TARGET_USER,               ///< 自分のみ
  TARGET_ALL,                ///< 場に出ている全ポケ
  TARGET_ENEMY_RANDOM,       ///< 相手ポケ１体ランダム

  // ポケモン以外対象
  TARGET_FIELD,              ///< 場全体（天候など）
  TARGET_SIDE_ENEMY,         ///< 敵側陣営
  TARGET_SIDE_FRIEND,        ///< 自分側陣営

  // 特殊
  TARGET_UNKNOWN,            ///< ゆびをふるなど特殊型

  TARGET_MAX,
};


#if !defined PAWN_COMPILER
GFL_NAMESPACE_END( wazadata )
GFL_NAMESPACE_END( pml )
#endif

#endif // PML_WAZATARGET_H_INCLUDED