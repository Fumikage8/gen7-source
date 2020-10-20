#if PM_DEBUG

//======================================================================
/**
 * @file FieldDebugBattleInst.h
 * @date 2016/02/12
 * @author saita_kazuki
 * @brief デバッグ：BattleInst バトル施設
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELDDEBUG_BATTLE_INST_H_INCLUDED__
#define __FIELDDEBUG_BATTLE_INST_H_INCLUDED__
#pragma once

#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include "niji_reference_files/script/FieldPawnTypes.h"

// 前方宣言
namespace gfl2 {
  namespace debug {
    class DebugWinGroup;
  }
}

namespace Field {
  class Fieldmap;
}

namespace Savedata {
  class BattleInstSave;
}

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

/**
 * @class DebugBattleInst
 * @brief バトルロイヤル関連の
 */
class DebugBattleInst
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugBattleInst);

public:
  /**
   *  @brief  コンストラクタ
   */
  DebugBattleInst( void );

  /**
   *  @brief  デストラクタ
   */
  ~DebugBattleInst( void );
  
  /**
   *  @brief  初期化
   */
  void Initialize(
    gfl2::heap::HeapBase *pHeap,
    Fieldmap* pFieldmap,
    gfl2::str::MsgData* pMsgData,
    Savedata::BattleInstSave* pSave
    );

  /**
   *  @brief  破棄
   */
  void Terminate( void );

  /**
   *  @brief  メニューの作成
   */
  void CreateDebugMenu( gfl2::debug::DebugWinGroup *root );

public:

  gfl2::heap::HeapBase*             m_pHeap;
  Fieldmap*                         m_pFieldmap;
  gfl2::str::MsgData*               m_pMsgData;
  Savedata::BattleInstSave*         m_pSave;

  s16                               m_royalRank;
  s16                               m_royalWinCount;

  s16                               m_treeSingleWin[Savedata::BATTLE_TREE_RANK_MAX];
  s16                               m_treeDoubleWin[Savedata::BATTLE_TREE_RANK_MAX];
  s16                               m_treeMultiWin[Savedata::BATTLE_TREE_RANK_MAX];

  s16                               m_treeSingleWinMax[Savedata::BATTLE_TREE_RANK_MAX];
  s16                               m_treeDoubleWinMax[Savedata::BATTLE_TREE_RANK_MAX];
  s16                               m_treeMultiWinMax[Savedata::BATTLE_TREE_RANK_MAX];

  s16                               m_treePrizeWin;

  u8                                m_treeSelectRank;

  s32                               battleInstDebugBtlInstTrainer1;
  s32                               battleInstDebugBtlInstTrainer2;
  
  s32                               battleInstDebugBtlInstForceCommErrorNo;
public:

  static gfl2::ui::Button* GetButton();
  static s32 UpdateKey( gfl2::ui::Button* pButton, s32 input, s32 value, s32 valueHigh, s32 min, s32 max);
  static wchar_t* DrawFunc_S32( s32 value, gfl2::debug::DebugWinItem* item );

  static void CreateDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item );
  static void DestroyDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item );

  static void UpdateFunc_RoyalRank( void* userWork, gfl2::debug::DebugWinItem* item );
  static wchar_t* DrawFunc_RoyalRank( void* userWork, gfl2::debug::DebugWinItem* item );
  static void UpdateFunc_RoyalWinCount( void* userWork, gfl2::debug::DebugWinItem* item );
  static wchar_t* DrawFunc_RoyalWinCount( void* userWork, gfl2::debug::DebugWinItem* item );

  static void UpdateFunc_TreeSingleWinCount( void* userWork, gfl2::debug::DebugWinItem* item );
  static wchar_t* DrawFunc_TreeSingleWinCount( void* userWork, gfl2::debug::DebugWinItem* item );
  static void UpdateFunc_TreeDoubleWinCount( void* userWork, gfl2::debug::DebugWinItem* item );
  static wchar_t* DrawFunc_TreeDoubleWinCount( void* userWork, gfl2::debug::DebugWinItem* item );
  static void UpdateFunc_TreeMultiWinCount( void* userWork, gfl2::debug::DebugWinItem* item );
  static wchar_t* DrawFunc_TreeMultiWinCount( void* userWork, gfl2::debug::DebugWinItem* item );

  static void UpdateFunc_TreeSingleWinCountMax( void* userWork, gfl2::debug::DebugWinItem* item );
  static wchar_t* DrawFunc_TreeSingleWinCountMax( void* userWork, gfl2::debug::DebugWinItem* item );
  static void UpdateFunc_TreeDoubleWinCountMax( void* userWork, gfl2::debug::DebugWinItem* item );
  static wchar_t* DrawFunc_TreeDoubleWinCountMax( void* userWork, gfl2::debug::DebugWinItem* item );
  static void UpdateFunc_TreeMultiWinCountMax( void* userWork, gfl2::debug::DebugWinItem* item );
  static wchar_t* DrawFunc_TreeMultiWinCountMax( void* userWork, gfl2::debug::DebugWinItem* item );
  static wchar_t* DrawFunc_TreePrizeWinCount( void* userWork, gfl2::debug::DebugWinItem* item );
  static void UpdateFunc_TreePrizeWinCount( void* userWork, gfl2::debug::DebugWinItem* item );

  static void UpdateFunc_ScoutTrainerFull( void* userWork, gfl2::debug::DebugWinItem* item );
  static void UpdateFunc_ScoutTrainerAdd( void* userWork, gfl2::debug::DebugWinItem* item );

  static void UpdateFunc_TreeSingleReleaseSuper( void* userWork, gfl2::debug::DebugWinItem* item );
  static void UpdateFunc_TreeDoubleReleaseSuper( void* userWork, gfl2::debug::DebugWinItem* item );
  static void UpdateFunc_TreeMultiReleaseSuper( void* userWork, gfl2::debug::DebugWinItem* item );
  static void UpdateFunc_TreeDLLSizeCheck( void* userWork, gfl2::debug::DebugWinItem* item );

  // スーパーのクイックスタート
  static void UpdateFunc_QuickSuperSingleStart( void* userWork, gfl2::debug::DebugWinItem* item );

  static void UpdateFunc_TreeForceTrainerNo1( void* userWork, gfl2::debug::DebugWinItem* item );
  static wchar_t* DrawFunc_TreeForceTrainerNo1( void* userWork, gfl2::debug::DebugWinItem* item );

  static void UpdateFunc_TreeForceTrainerNo2( void* userWork, gfl2::debug::DebugWinItem* item );
  static wchar_t* DrawFunc_TreeForceTrainerNo2( void* userWork, gfl2::debug::DebugWinItem* item );

  static void UpdateFunc_TreeForceCommError( void* userWork, gfl2::debug::DebugWinItem* item );
  static wchar_t* DrawFunc_TreeForceForceCommError( void* userWork, gfl2::debug::DebugWinItem* item );
};

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // __FIELDDEBUG_BATTLE_INST_H_INCLUDED__

#endif // PM_DEBUG
