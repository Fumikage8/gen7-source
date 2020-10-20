//==============================================================================
/**
 * @file	RegulationDebug.h
 * @brief	ジョインフェスタ施設システムデバッグ機能
 * @author	ichiraku_katsuhiko
 * @date	2015/10/22　12:47
 */
// =============================================================================
#ifndef __REGULATION_DEBUG_H__
#define __REGULATION_DEBUG_H__
#pragma once

// ライブラリ
#include <base/include/gfl2_Singleton.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

// niji
#include "GameSys/include/GameManager.h"
#include "System/include/HeapDefine.h"

// レギュレーション
#include "Battle/Regulation/include/Regulation.h"

// Debug
#include <Debug/include/gfl2_DebugWinSystem.h>
#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include <Debug/DebugWin/include/DebugWinSystem.h>

// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"


#if PM_DEBUG
class RegulationDebug
{
  GFL_FORBID_COPY_AND_ASSIGN(RegulationDebug);

public:
  //! コンストラクタ
  RegulationDebug( Regulation* pRegulation )
    : m_pRegulation(pRegulation) 
    , m_debugGroup(NULL)
    , m_pDebugButton(NULL)
  {
    GameSys::GameManager*             pGameManager      = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    ::Debug::DebugWin::DebugWinWork*  devWork           = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
    gfl2::str::MsgData*               pMsgData          = devWork->GetMessageData();
    gfl2::heap::HeapBase*             pHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG);

    /*  deviceManager  */
    m_pDebugButton = pGameManager->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

    /*  グループ  */
    m_debugGroup  = gfl2::debug::DebugWin_AddGroup( pHeap, L"Regulation" );

    // レギュレーションなかったら何もしない
    if( pRegulation == NULL )
    {
      return;
    }

    // レギュレーションデータを変更
    Regulation::CoreData *pCoreData = m_pRegulation->m_pCoreData;
    
    // 「version」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGroup, pHeap, L"version", &pCoreData->regulation_version, 0, 99 );
    // 「category」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGroup, pHeap, L"category", &pCoreData->category, 0, Regulation::BATTLE_CATEGORY_MAX-1 );
    // 「rule」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGroup, pHeap, L"rule", &pCoreData->btl_rule, 0, 3 );
    // 「team_num_lo」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGroup, pHeap, L"team_num_lo", &pCoreData->team_num_lo, 0, 6 );
    // 「team_num_hi」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGroup, pHeap, L"team_num_hi", &pCoreData->team_num_hi, 0, 6 );
    // 「select_num_lo」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGroup, pHeap, L"select_num_lo", &pCoreData->select_num_lo, 0, 6 );
    // 「select_num_hi」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGroup, pHeap, L"select_num_hi", &pCoreData->select_num_hi, 0, 6 );
    // 「legend_num_hi」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGroup, pHeap, L"legend_num_hi", &pCoreData->legend_num_hi, 0, 6 );
    // 「level」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGroup, pHeap, L"level", &pCoreData->level, 1, 100 );
    // 「level_range」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGroup, pHeap, L"level_range", &pCoreData->level_range, 0, Regulation::LEVEL_RANGE_MAX );
    // 「reverse」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGroup, pHeap, L"reverse", &pCoreData->reverse, 0, 1 );
    // 「both_poke」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGroup, pHeap, L"both_poke", &pCoreData->both_poke, 0, 1 );
    // 「both_item」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGroup, pHeap, L"both_item", &pCoreData->both_item, 0, 1 );
    // 「huraette_hakai」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGroup, pHeap, L"huraette_hakai", &pCoreData->huraette_hakai, 0, 1 );
    // 「qr_team」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGroup, pHeap, L"qr_team", &pCoreData->qr_team, 0, 1 );
    // 「time_limit_type」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGroup, pHeap, L"time_limit_type", &pCoreData->time_limit_type, 0, 1 );
    // 「time_vs」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGroup, pHeap, L"time_vs", &pCoreData->time_vs, 0, 99 );
    // 「time_command」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGroup, pHeap, L"time_command", &pCoreData->time_command, 0, 99 );
    // 「nickname」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGroup, pHeap, L"nickname", &pCoreData->nickname, 0, 1 );
    // 「background」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGroup, pHeap, L"background", &pCoreData->background, 0, 3 );
    // 「show_poke」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGroup, pHeap, L"show_poke", &pCoreData->show_poke, 0, 1 );
    // 「show_poke_time」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGroup, pHeap, L"show_poke_time", &pCoreData->show_poke_time, 0, 99 );
    // 「btl_video」
    gfl2::debug::DebugWin_AddItemValueU16( m_debugGroup, pHeap, L"btl_video", &pCoreData->btl_video, 0, 9999 );
    // 「watch」
    gfl2::debug::DebugWin_AddItemValueU8( m_debugGroup, pHeap, L"watch", &pCoreData->watch, 0, 1 );
  }

  //! デストラクタ
  virtual ~RegulationDebug()
  {
    // グループ破棄
    gfl2::debug::DebugWin_RemoveGroup( m_debugGroup );
  }

private:
  Regulation*                 m_pRegulation;
  gfl2::debug::DebugWinGroup* m_debugGroup;
  gfl2::ui::Button*           m_pDebugButton;

};


#endif  /*  PM_DEBUG  */



#endif // __REGULATION_DEBUG_H__
