#if PM_DEBUG

//======================================================================
/**
 * @file FieldDebugBattleInst.cpp
 * @date 2016/02/12
 * @author saita_kazuki
 * @brief デバッグ：BattleInst バトル施設
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/Debug/FieldDebugBattleInst.h"

// gfl2
#include <fs/include/gfl2_FsAsyncFileManager.h>
// GameSys
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
// field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
// battleinst
#include "Field/FieldStatic/include/BattleInst/BattleInst.h"
// msg
#include "niji_conv_header/message/debug/msg_debug_menu.h"
// save
#include "Savedata/include/BattleInstSave.h"
#include "Savedata/include/EventWork.h"
// sound
#include "Sound/include/Sound.h"

#include "FieldScript/include/EventScriptCall.h"
#include "niji_conv_header\field\script\inc\battle_inst_scr.inc"


GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

//------------------------------------------------------------------------------
// STATIC
//------------------------------------------------------------------------------

gfl2::ui::Button* DebugBattleInst::GetButton()
{
  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button* pButton = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

  return pButton;
}

s32 DebugBattleInst::UpdateKey( gfl2::ui::Button* pButton, s32 input, s32 value, s32 valueHigh, s32 min, s32 max)
{
  if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
  {
    value = valueHigh;
  }

  s32 ret = input;
  if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    ret = input + value;
    if( ret > max)
    {
      ret -= (max + 1);
    }
  }
  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    ret = input - value;
    if( ret < min)
    {
      ret += (max + 1);
    }
  }

  return ret;
}

// s32表示
wchar_t* DebugBattleInst::DrawFunc_S32( s32 value, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %d ]", item->GetName(), value );

  return workStr;
}

// ロイヤルランク更新
void DebugBattleInst::UpdateFunc_RoyalRank( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* pButton = DebugBattleInst::GetButton();
  DebugBattleInst* pThis = (DebugBattleInst*)userWork;

  pThis->m_royalRank = DebugBattleInst::UpdateKey( pButton, pThis->m_royalRank, 1, 1, Savedata::BATTLE_ROYAL_RANK_NORMAL, Savedata::BATTLE_ROYAL_RANK_MASTER);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A) )
  {
    pThis->m_pSave->SetReleaseRankRoyal( static_cast<Savedata::BattleRoyalRank>(pThis->m_royalRank));
    Sound::PlaySE( SEQ_SE_DECIDE1 );
  }
}

// ロイヤルランク表示
wchar_t* DebugBattleInst::DrawFunc_RoyalRank( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugBattleInst* pThis = (DebugBattleInst*)userWork;
  return DebugBattleInst::DrawFunc_S32( pThis->m_royalRank, item);
}

// ロイヤル連勝数更新
void DebugBattleInst::UpdateFunc_RoyalWinCount( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* pButton = DebugBattleInst::GetButton();
  DebugBattleInst* pThis = (DebugBattleInst*)userWork;

  pThis->m_royalWinCount = DebugBattleInst::UpdateKey( pButton, pThis->m_royalWinCount, 1, 1, 0, Savedata::BATTLE_ROYAL_WIN_COUNT_MAX);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A) )
  {
    pThis->m_pSave->SetWinCountRoyal( pThis->m_royalWinCount);
    Sound::PlaySE( SEQ_SE_DECIDE1 );
  }
}

// ロイヤル連勝数表示
wchar_t* DebugBattleInst::DrawFunc_RoyalWinCount( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugBattleInst* pThis = (DebugBattleInst*)userWork;
  return DebugBattleInst::DrawFunc_S32( pThis->m_royalWinCount, item);
}

// ツリーシングル連勝数更新
void DebugBattleInst::UpdateFunc_TreeSingleWinCount( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* pButton = DebugBattleInst::GetButton();
  DebugBattleInst* pThis = (DebugBattleInst*)userWork;

  pThis->m_treeSingleWin[pThis->m_treeSelectRank] = DebugBattleInst::UpdateKey( pButton, pThis->m_treeSingleWin[pThis->m_treeSelectRank], 1, 1, 0, pThis->m_pSave->GetCountMax());

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A) )
  {
    pThis->m_pSave->SetWinCount( Savedata::BATTLE_TREE_TYPE_SINGLE, static_cast<Savedata::BattleTreeRank>( pThis->m_treeSelectRank), pThis->m_treeSingleWin[pThis->m_treeSelectRank]);
    Sound::PlaySE( SEQ_SE_DECIDE1 );
  }
}

// ツリーシングル連勝数表示
wchar_t* DebugBattleInst::DrawFunc_TreeSingleWinCount( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugBattleInst* pThis = (DebugBattleInst*)userWork;
  return DebugBattleInst::DrawFunc_S32( pThis->m_treeSingleWin[pThis->m_treeSelectRank], item);
}

// ツリーダブル連勝数更新
void DebugBattleInst::UpdateFunc_TreeDoubleWinCount( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* pButton = DebugBattleInst::GetButton();
  DebugBattleInst* pThis = (DebugBattleInst*)userWork;

  pThis->m_treeDoubleWin[pThis->m_treeSelectRank] = DebugBattleInst::UpdateKey( pButton, pThis->m_treeDoubleWin[pThis->m_treeSelectRank], 1, 1, 0, pThis->m_pSave->GetCountMax());

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A) )
  {
    pThis->m_pSave->SetWinCount( Savedata::BATTLE_TREE_TYPE_DOUBLE, static_cast<Savedata::BattleTreeRank>( pThis->m_treeSelectRank), pThis->m_treeDoubleWin[pThis->m_treeSelectRank]);
    Sound::PlaySE( SEQ_SE_DECIDE1 );
  }
}

// ツリーダブル連勝数表示
wchar_t* DebugBattleInst::DrawFunc_TreeDoubleWinCount( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugBattleInst* pThis = (DebugBattleInst*)userWork;
  return DebugBattleInst::DrawFunc_S32( pThis->m_treeDoubleWin[pThis->m_treeSelectRank], item);
}

// ツリーマルチ連勝数更新
void DebugBattleInst::UpdateFunc_TreeMultiWinCount( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* pButton = DebugBattleInst::GetButton();
  DebugBattleInst* pThis = (DebugBattleInst*)userWork;

  pThis->m_treeMultiWin[pThis->m_treeSelectRank] = DebugBattleInst::UpdateKey( pButton, pThis->m_treeMultiWin[pThis->m_treeSelectRank], 1, 1, 0, pThis->m_pSave->GetCountMax());

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A) )
  {
    pThis->m_pSave->SetWinCount( Savedata::BATTLE_TREE_TYPE_MULTI, static_cast<Savedata::BattleTreeRank>( pThis->m_treeSelectRank), pThis->m_treeMultiWin[pThis->m_treeSelectRank]);
    Sound::PlaySE( SEQ_SE_DECIDE1 );
  }
}

// ツリーマルチ連勝数表示
wchar_t* DebugBattleInst::DrawFunc_TreeMultiWinCount( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugBattleInst* pThis = (DebugBattleInst*)userWork;
  return DebugBattleInst::DrawFunc_S32( pThis->m_treeMultiWin[pThis->m_treeSelectRank], item);
}

// ツリーシングル最大連勝数更新
void DebugBattleInst::UpdateFunc_TreeSingleWinCountMax( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* pButton = DebugBattleInst::GetButton();
  DebugBattleInst* pThis = (DebugBattleInst*)userWork;

  pThis->m_treeSingleWinMax[pThis->m_treeSelectRank] = DebugBattleInst::UpdateKey( pButton, pThis->m_treeSingleWinMax[pThis->m_treeSelectRank], 1, 1, 0, pThis->m_pSave->GetCountMax());

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A) )
  {
    pThis->m_pSave->SetWinCountMax( Savedata::BATTLE_TREE_TYPE_SINGLE, static_cast<Savedata::BattleTreeRank>( pThis->m_treeSelectRank), pThis->m_treeSingleWinMax[pThis->m_treeSelectRank]);
    Sound::PlaySE( SEQ_SE_DECIDE1 );
  }
}

// ツリーシングル最大連勝数表示
wchar_t* DebugBattleInst::DrawFunc_TreeSingleWinCountMax( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugBattleInst* pThis = (DebugBattleInst*)userWork;
  return DebugBattleInst::DrawFunc_S32( pThis->m_treeSingleWinMax[pThis->m_treeSelectRank], item);
}

// ツリーダブル最大連勝数更新
void DebugBattleInst::UpdateFunc_TreeDoubleWinCountMax( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* pButton = DebugBattleInst::GetButton();
  DebugBattleInst* pThis = (DebugBattleInst*)userWork;

  pThis->m_treeDoubleWinMax[pThis->m_treeSelectRank] = DebugBattleInst::UpdateKey( pButton, pThis->m_treeDoubleWinMax[pThis->m_treeSelectRank], 1, 1, 0, pThis->m_pSave->GetCountMax());

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A) )
  {
    pThis->m_pSave->SetWinCountMax( Savedata::BATTLE_TREE_TYPE_DOUBLE, static_cast<Savedata::BattleTreeRank>( pThis->m_treeSelectRank), pThis->m_treeDoubleWinMax[pThis->m_treeSelectRank]);
    Sound::PlaySE( SEQ_SE_DECIDE1 );
  }
}

// ツリーダブル最大連勝数表示
wchar_t* DebugBattleInst::DrawFunc_TreeDoubleWinCountMax( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugBattleInst* pThis = (DebugBattleInst*)userWork;
  return DebugBattleInst::DrawFunc_S32( pThis->m_treeDoubleWinMax[pThis->m_treeSelectRank], item);
}

// ツリーマルチ最大連勝数更新
void DebugBattleInst::UpdateFunc_TreeMultiWinCountMax( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* pButton = DebugBattleInst::GetButton();
  DebugBattleInst* pThis = (DebugBattleInst*)userWork;

  pThis->m_treeMultiWinMax[pThis->m_treeSelectRank] = DebugBattleInst::UpdateKey( pButton, pThis->m_treeMultiWinMax[pThis->m_treeSelectRank], 1, 1, 0, pThis->m_pSave->GetCountMax());

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A) )
  {
    pThis->m_pSave->SetWinCountMax( Savedata::BATTLE_TREE_TYPE_MULTI, static_cast<Savedata::BattleTreeRank>( pThis->m_treeSelectRank), pThis->m_treeMultiWinMax[pThis->m_treeSelectRank]);
    Sound::PlaySE( SEQ_SE_DECIDE1 );
  }
}

// ツリーマルチ最大連勝数表示
wchar_t* DebugBattleInst::DrawFunc_TreeMultiWinCountMax( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugBattleInst* pThis = (DebugBattleInst*)userWork;
  return DebugBattleInst::DrawFunc_S32( pThis->m_treeMultiWinMax[pThis->m_treeSelectRank], item);
}


// ご褒美用連勝数連勝数表示
wchar_t* DebugBattleInst::DrawFunc_TreePrizeWinCount( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugBattleInst* pThis = (DebugBattleInst*)userWork;
  return DebugBattleInst::DrawFunc_S32( pThis->m_treePrizeWin, item);
}

// ご褒美用連勝数連勝数更新
void DebugBattleInst::UpdateFunc_TreePrizeWinCount( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* pButton = DebugBattleInst::GetButton();
  DebugBattleInst* pThis = (DebugBattleInst*)userWork;

  pThis->m_treePrizeWin = DebugBattleInst::UpdateKey( pButton, pThis->m_treePrizeWin, 1, 1, 0, pThis->m_pSave->GetCountMax());

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A) )
  {
    pThis->m_pSave->SetPrizeWinCount( pThis->m_treePrizeWin );
    Sound::PlaySE( SEQ_SE_DECIDE1 );
  }
}

// スカウトトレーナーFULL更新
void DebugBattleInst::UpdateFunc_ScoutTrainerFull( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* pButton = DebugBattleInst::GetButton();
  DebugBattleInst* pThis = (DebugBattleInst*)userWork;
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A) )
  {
    u32 registNum = pThis->m_pSave->GetRegistScoutTrainerDataNum();
    u32 addNum = Savedata::BattleInstSave::SCOUT_TRAINER_MAX - registNum;
    for( u32 i = 0; i < addNum; ++i)
    {
      pThis->m_pSave->AddScoutTrainerData( i, 0, 1, 0, 0 );
    }
    Sound::PlaySE( SEQ_SE_DECIDE1 );
  }
}

// スカウトトレーナー追加更新
void DebugBattleInst::UpdateFunc_ScoutTrainerAdd( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* pButton = DebugBattleInst::GetButton();
  DebugBattleInst* pThis = (DebugBattleInst*)userWork;
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A) )
  {
    pThis->m_pSave->AddScoutTrainerData( 0, 0, 1, 0, 0 );
    Sound::PlaySE( SEQ_SE_DECIDE1 );
  }
}

// ツリーシングルスーパーランク開放
void DebugBattleInst::UpdateFunc_TreeSingleReleaseSuper( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* pButton = DebugBattleInst::GetButton();
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A) )
  {
    GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    EventWork* pEventWork = pGameManager->GetGameData()->GetEventWork();
    u16 flag = FE_BINST_CLEAR_SINGLE;
    if( pEventWork->CheckEventFlag( flag))
    {
      pEventWork->ResetEventFlag( flag);
      Sound::PlaySE( SEQ_SE_CANCEL1 );
    }
    else
    {
      pEventWork->SetEventFlag( flag);
      Sound::PlaySE( SEQ_SE_DECIDE1 );
    }
  }
}

// ツリーダブルスーパーランク開放
void DebugBattleInst::UpdateFunc_TreeDoubleReleaseSuper( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* pButton = DebugBattleInst::GetButton();
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A) )
  {
    GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    EventWork* pEventWork = pGameManager->GetGameData()->GetEventWork();
    u16 flag = FE_BINST_CLEAR_DOUBLE;
    if( pEventWork->CheckEventFlag( flag))
    {
      pEventWork->ResetEventFlag( flag);
      Sound::PlaySE( SEQ_SE_CANCEL1 );
    }
    else
    {
      pEventWork->SetEventFlag( flag);
      Sound::PlaySE( SEQ_SE_DECIDE1 );
    }
  }
}

// ツリーマルチスーパーランク開放
void DebugBattleInst::UpdateFunc_TreeMultiReleaseSuper( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* pButton = DebugBattleInst::GetButton();
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A) )
  {
    GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    EventWork* pEventWork = pGameManager->GetGameData()->GetEventWork();
    u16 flag = FE_BINST_CLEAR_MULTI;
    if( pEventWork->CheckEventFlag( flag))
    {
      pEventWork->ResetEventFlag( flag);
      Sound::PlaySE( SEQ_SE_CANCEL1 );
    }
    else
    {
      pEventWork->SetEventFlag( flag);
      Sound::PlaySE( SEQ_SE_DECIDE1 );
    }
  }
}


// DLLサイズチェック
void DebugBattleInst::UpdateFunc_TreeDLLSizeCheck( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* pButton = DebugBattleInst::GetButton();
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A) )
  {
    GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    gfl2::ro::RoManager *pRoManager = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
    gfl2::heap::HeapBase * pDllLoad = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DLL_LOAD );


    struct DllData
    {
      bool isLow; // メモリの後方確保を行なうか
      char* dllName;
    };

    // Roモジュール
    static const int RO_MODULE_NUM = 4;
    const DllData RO_MODULES[RO_MODULE_NUM] =
    {
      {false,"FieldGimmickBattleInst.cro"},
      {true,"FieldBattleInstEvent.cro"},
      //{true,"SeqEditor.cro"},//Object,Background,SeqEditorはmomijiで統合されました。
      {true,"Object.cro"},
      {true,"SequencePlayer.cro"},
    };
    gfl2::ro::Module* pModules[RO_MODULE_NUM];


    for( u32 i = 0; i < RO_MODULE_NUM; ++i)
    {
      if( RO_MODULES[i].isLow )
      {
        pModules[i] = pRoManager->LoadModuleLower( pGameManager->GetAsyncFileManager(), RO_MODULES[i].dllName, pDllLoad);
      }else{
        pModules[i] = pRoManager->LoadModule( pGameManager->GetAsyncFileManager(), RO_MODULES[i].dllName, pDllLoad);
      }
      pRoManager->StartModule( pModules[i],true);
      GFL_RELEASE_PRINT( "load %s\n", RO_MODULES[i].dllName );
      pDllLoad->Dump();
    }
    GFL_RELEASE_PRINT( "BattleTree StartModule end\n" );
    GFL_RELEASE_PRINT( "write heap free size  when had loaded dll free[0x%x] allocatable[0x%x]\n", pDllLoad->GetTotalFreeSize(), pDllLoad->GetTotalAllocatableSize() );
    pDllLoad->Dump();

    for( u32 i = 0; i < RO_MODULE_NUM; ++i)
    {
      pRoManager->DisposeModule( pModules[i] );
    }
    GFL_RELEASE_PRINT( "BattleTree DisposeModule end\n" );

    Sound::PlaySE( SEQ_SE_DECIDE1 );
  }
}


// スーパーのクイックスタート
void DebugBattleInst::UpdateFunc_QuickSuperSingleStart( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();

  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    item->PlaySystemSound(gfl2::debug::SE_CHANGE_VALUE);
    
    Field::EventScriptCall::CallScript( pGameManager, SCRID_BINST_QUICK_SUPERS_INGLE_START, NULL );
    // ウィンドウを閉じる
    gfl2::debug::DebugWin_CloseWindow();
  }
}

// 対戦相手強制指定1
void DebugBattleInst::UpdateFunc_TreeForceTrainerNo1( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* pButton = DebugBattleInst::GetButton();
  DebugBattleInst* pThis = (DebugBattleInst*)userWork;

  pThis->battleInstDebugBtlInstTrainer1 = DebugBattleInst::UpdateKey( pButton, pThis->battleInstDebugBtlInstTrainer1, 1, 10, -1, 206 );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A) )
  {
    Field::Debug::DebugTypes::s_battleInstDebugBtlInstTrainer1 = pThis->battleInstDebugBtlInstTrainer1;
    Sound::PlaySE( SEQ_SE_DECIDE1 );
  }
}

// 対戦相手強制指定1
wchar_t* DebugBattleInst::DrawFunc_TreeForceTrainerNo1( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugBattleInst* pThis = (DebugBattleInst*)userWork;
  return DebugBattleInst::DrawFunc_S32( pThis->battleInstDebugBtlInstTrainer1, item);
}


// 対戦相手強制指定2
void DebugBattleInst::UpdateFunc_TreeForceTrainerNo2( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* pButton = DebugBattleInst::GetButton();
  DebugBattleInst* pThis = (DebugBattleInst*)userWork;

  pThis->battleInstDebugBtlInstTrainer2 = DebugBattleInst::UpdateKey( pButton, pThis->battleInstDebugBtlInstTrainer2, 1, 10, -1, 206 );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A) )
  {
    Field::Debug::DebugTypes::s_battleInstDebugBtlInstTrainer2 = pThis->battleInstDebugBtlInstTrainer2;
    Sound::PlaySE( SEQ_SE_DECIDE1 );
  }
}

// 対戦相手強制指定2
wchar_t* DebugBattleInst::DrawFunc_TreeForceTrainerNo2( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugBattleInst* pThis = (DebugBattleInst*)userWork;
  return DebugBattleInst::DrawFunc_S32( pThis->battleInstDebugBtlInstTrainer2, item);
}

// 通信エラーコード強制指定
void DebugBattleInst::UpdateFunc_TreeForceCommError( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* pButton = DebugBattleInst::GetButton();
  DebugBattleInst* pThis = (DebugBattleInst*)userWork;

  pThis->battleInstDebugBtlInstForceCommErrorNo = DebugBattleInst::UpdateKey( pButton, pThis->battleInstDebugBtlInstForceCommErrorNo, 1, 1, -1, 99);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A) )
  {
    Field::Debug::DebugTypes::s_battleInstDebugBtlInstForceCommError = pThis->battleInstDebugBtlInstForceCommErrorNo;
    Sound::PlaySE( SEQ_SE_DECIDE1 );
  }
}

// 対戦相手強制指定2
wchar_t* DebugBattleInst::DrawFunc_TreeForceForceCommError( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugBattleInst* pThis = (DebugBattleInst*)userWork;
  return DebugBattleInst::DrawFunc_S32( pThis->battleInstDebugBtlInstForceCommErrorNo, item);
}


/**
 *  @brief  Group create
 */
void DebugBattleInst::CreateDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugBattleInst *pDebug = reinterpret_cast<DebugBattleInst*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->m_pHeap;
    gfl2::str::MsgData* pMsgData = pDebug->m_pMsgData;

    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_battle_inst_royal_rank_set, pDebug, DebugBattleInst::UpdateFunc_RoyalRank, DebugBattleInst::DrawFunc_RoyalRank );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_battle_inst_royal_win_count_set, pDebug, DebugBattleInst::UpdateFunc_RoyalWinCount, DebugBattleInst::DrawFunc_RoyalWinCount );

    gfl2::debug::DebugWin_AddItemValueU8( pGroup, pHeap, pMsgData, msg_dmenu_battle_inst_tree_rank, &pDebug->m_treeSelectRank, 0, 1, 1 );

    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_battle_inst_tree_win_count_set_s_n, pDebug, DebugBattleInst::UpdateFunc_TreeSingleWinCount, DebugBattleInst::DrawFunc_TreeSingleWinCount );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_battle_inst_tree_win_count_set_d_n, pDebug, DebugBattleInst::UpdateFunc_TreeDoubleWinCount, DebugBattleInst::DrawFunc_TreeDoubleWinCount );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_battle_inst_tree_win_count_set_m_n, pDebug, DebugBattleInst::UpdateFunc_TreeMultiWinCount, DebugBattleInst::DrawFunc_TreeMultiWinCount );

    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_battle_inst_tree_max_win_count_set_s_n, pDebug, DebugBattleInst::UpdateFunc_TreeSingleWinCountMax, DebugBattleInst::DrawFunc_TreeSingleWinCountMax );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_battle_inst_tree_max_win_count_set_d_n, pDebug, DebugBattleInst::UpdateFunc_TreeDoubleWinCountMax, DebugBattleInst::DrawFunc_TreeDoubleWinCountMax );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_battle_inst_tree_max_win_count_set_m_n, pDebug, DebugBattleInst::UpdateFunc_TreeMultiWinCountMax, DebugBattleInst::DrawFunc_TreeMultiWinCountMax );

    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_battle_inst_tree_prize_win_count_set, pDebug, DebugBattleInst::UpdateFunc_TreePrizeWinCount, DebugBattleInst::DrawFunc_TreePrizeWinCount );

    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_battle_inst_tree_scout_trainer_full, pDebug, DebugBattleInst::UpdateFunc_ScoutTrainerFull, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_battle_inst_tree_win_scout_trainer_add, pDebug, DebugBattleInst::UpdateFunc_ScoutTrainerAdd, NULL );

    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_battle_inst_tree_release_s_s, pDebug, DebugBattleInst::UpdateFunc_TreeSingleReleaseSuper, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_battle_inst_tree_release_d_s, pDebug, DebugBattleInst::UpdateFunc_TreeDoubleReleaseSuper, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_battle_inst_tree_release_m_s, pDebug, DebugBattleInst::UpdateFunc_TreeMultiReleaseSuper, NULL );
    
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_battle_inst_tree_dll_size_check, pDebug, DebugBattleInst::UpdateFunc_TreeDLLSizeCheck, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_battle_inst_quick_supers_ingle_start, pDebug, DebugBattleInst::UpdateFunc_QuickSuperSingleStart, NULL );
    
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_battle_inst_force_set_tr_id_1, pDebug, DebugBattleInst::UpdateFunc_TreeForceTrainerNo1, DebugBattleInst::DrawFunc_TreeForceTrainerNo1 );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_battle_inst_force_set_tr_id_2, pDebug, DebugBattleInst::UpdateFunc_TreeForceTrainerNo2, DebugBattleInst::DrawFunc_TreeForceTrainerNo2 );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_battle_inst_force_net_error, pDebug, DebugBattleInst::UpdateFunc_TreeForceCommError, DebugBattleInst::DrawFunc_TreeForceForceCommError );

    gfl2::debug::DebugWin_AddItemValueInt( pGroup, pHeap, pMsgData, msg_dmenu_battle_inst_royal_rank_set, &Field::Debug::DebugTypes::s_battleRoyalInstDebugForceVsTrainerNo[0], -1, 51, 1 );
    gfl2::debug::DebugWin_AddItemValueInt( pGroup, pHeap, pMsgData, msg_dmenu_battle_inst_royal_rank_set, &Field::Debug::DebugTypes::s_battleRoyalInstDebugForceVsTrainerNo[1], -1, 51, 1 );
    gfl2::debug::DebugWin_AddItemValueInt( pGroup, pHeap, pMsgData, msg_dmenu_battle_inst_royal_rank_set, &Field::Debug::DebugTypes::s_battleRoyalInstDebugForceVsTrainerNo[2], -1, 51, 1 );
  }

  pDebug->m_royalRank = pDebug->m_pSave->GetReleaseRankRoyal();
  pDebug->m_royalWinCount = pDebug->m_pSave->GetWinCountRoyal();

  for( s32 i = 0; i < Savedata::BATTLE_TREE_RANK_MAX; ++i)
  {
    pDebug->m_treeSingleWin[i] = pDebug->m_pSave->GetWinCount( Savedata::BATTLE_TREE_TYPE_SINGLE, static_cast<Savedata::BattleTreeRank>( i));
    pDebug->m_treeDoubleWin[i] = pDebug->m_pSave->GetWinCount( Savedata::BATTLE_TREE_TYPE_DOUBLE, static_cast<Savedata::BattleTreeRank>( i));
    pDebug->m_treeMultiWin[i] = pDebug->m_pSave->GetWinCount( Savedata::BATTLE_TREE_TYPE_MULTI, static_cast<Savedata::BattleTreeRank>( i));

    pDebug->m_treeSingleWinMax[i] = pDebug->m_pSave->GetMaxWinCount( Savedata::BATTLE_TREE_TYPE_SINGLE, static_cast<Savedata::BattleTreeRank>( i));
    pDebug->m_treeDoubleWinMax[i] = pDebug->m_pSave->GetMaxWinCount( Savedata::BATTLE_TREE_TYPE_DOUBLE, static_cast<Savedata::BattleTreeRank>( i));
    pDebug->m_treeMultiWinMax[i] = pDebug->m_pSave->GetMaxWinCount( Savedata::BATTLE_TREE_TYPE_MULTI, static_cast<Savedata::BattleTreeRank>( i));
  }

  pDebug->m_treePrizeWin = pDebug->m_pSave->GetPrizeWinCount();


  pDebug->battleInstDebugBtlInstTrainer1 = Field::Debug::DebugTypes::s_battleInstDebugBtlInstTrainer1;
  pDebug->battleInstDebugBtlInstTrainer2 = Field::Debug::DebugTypes::s_battleInstDebugBtlInstTrainer2;
  
  pDebug->battleInstDebugBtlInstForceCommErrorNo = Field::Debug::DebugTypes::s_battleInstDebugBtlInstForceCommError;
}

/**
 *  @brief  Group destroy
 */
void DebugBattleInst::DestroyDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pGroup )
  {
    pGroup->RemoveAllItem();
  }
}

//------------------------------------------------------------------------------
// DebugBattleInst
//------------------------------------------------------------------------------

/**
 * @class DebugBattleInst
 * @brief クラスの説明
 */
/**
 *  @brief  コンストラクタ
 */
DebugBattleInst::DebugBattleInst( void )
  : m_pHeap( NULL)
  , m_pFieldmap( NULL)
  , m_pMsgData( NULL)
  , m_pSave( NULL)
  , m_royalRank( 0)
  , m_royalWinCount( 0)
  , m_treeSelectRank( 0)
  , battleInstDebugBtlInstTrainer1(-1)
  , battleInstDebugBtlInstTrainer2(-1)
{
  for( s32 i = 0; i < Savedata::BATTLE_TREE_RANK_MAX; ++i)
  {
    m_treeSingleWin[i] = 0;
    m_treeDoubleWin[i] = 0;
    m_treeMultiWin[i] = 0;
    m_treeSingleWinMax[i] = 0;
    m_treeDoubleWinMax[i] = 0;
    m_treeMultiWinMax[i] = 0;
  }
}

/**
 *  @brief  デストラクタ
 */
DebugBattleInst::~DebugBattleInst( void )
{
  Terminate();
}

//------------------------------------------------------------------------------
// Public Functions
//------------------------------------------------------------------------------

/**
 *  @brief  初期化
 */
void DebugBattleInst::Initialize(
  gfl2::heap::HeapBase *pHeap,
  Fieldmap* pFieldmap,
  gfl2::str::MsgData* pMsgData,
  Savedata::BattleInstSave* pSave
  )
{
  m_pHeap = pHeap;
  m_pFieldmap = pFieldmap;
  m_pMsgData = pMsgData;
  m_pSave = pSave;
}

/**
 *  @brief  破棄
 */
void DebugBattleInst::Terminate( void )
{
}

/**
 *  @brief  メニューの作成
 */
void DebugBattleInst::CreateDebugMenu( gfl2::debug::DebugWinGroup *root )
{
  gfl2::debug::DebugWinGroup *pGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_battle_inst_title, root );
  pGroup->SetCreateDestroyGroupFunc( this, DebugBattleInst::CreateDebugGroupFunc, DebugBattleInst::DestroyDebugGroupFunc );
}

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // PM_DEBUG
