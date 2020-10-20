#if PM_DEBUG
//=============================================================================
/**
 * @file    d_ichiraku.cpp
 * @brief   一楽用ランチャーリスト処理
 * @author  ichiraku katsuhiko
 * @date    2015.03.10
 */
//=============================================================================
#include <Debug/StartMenu/include/StartLauncher.h>
#include <Debug/StartMenu/include/d_ichiraku.h>

#include <arc_index/debug_message.gaix>
#include <niji_conv_header/message/debug/msg_d_ichiraku.h>

#include <GameSys/include/GameProcManager.h>
#include <GameSys/include/GameManager.h>
#include "GameSys/include/GameData.h"
#include <Test/P2PTest/P2PTestProc.h>
#include <Test/P2pConnectionManagerTest/include/P2pConnectionManagerTestProc.h>
#include <Test/TestEvent/include/RomTrainerPokeCheckTest/RomTrainerPokeCheckEvent.h>

// nbr
#include <NetStatic/NetEvent/include/NBREvent.h>

// regulation
#include <heap/include/gfl2_heap_manager.h>
#include "system/include/HeapDefine.h"
#include "Battle/Regulation/include/Regulation.h"
#include "Battle/Regulation/include/PokeRegulation.h"

GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(startmenu)

// 項目文字列データテーブル
const u32 LauncherIchiraku::ItemList[LauncherIchiraku::LIST_MAX] =
{
  IchirakuDebugMenuItem_01,
  IchirakuDebugMenuItem_02,
  IchirakuDebugMenuItem_03,
  IchirakuDebugMenuItem_04,
  IchirakuDebugMenuItem_05,
};


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//-----------------------------------------------------------------------------
LauncherIchiraku::LauncherIchiraku(void) 
{
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
LauncherIchiraku::~LauncherIchiraku()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    GetArcDataID
 * @brief   アークデータID取得
 * @date    2015.02.18
 *
 * @return  アークデータID
 */
//-----------------------------------------------------------------------------
gfl2::fs::ArcFile::ARCDATID LauncherIchiraku::GetArcDataID(void)
{
  return GARC_debug_message_d_ichiraku_DAT;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetListTable
 * @brief   項目文字列データテーブル取得
 * @date    2015.02.18
 *
 * @return  文字列データテーブル
 */
//-----------------------------------------------------------------------------
const u32 * LauncherIchiraku::GetListTable(void)
{
  return ItemList;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetListTable
 * @brief   項目文字列データテーブルの要素数取得
 * @date    2015.02.18
 *
 * @return  文字列データテーブルの要素数
 */
//-----------------------------------------------------------------------------
u32 LauncherIchiraku::GetListTableSize(void)
{
  return LIST_MAX;
}

//-----------------------------------------------------------------------------
/**
 * @func    SelectListener
 * @brief   決定時のリスナー
 * @date    2015.02.18
 *
 * @param   pos       カーソル位置 ( キャンセル時はCURSOR_POS_CANCEL )
 * @param   launcher  呼び出し元のランチャークラス
 *
 * @return  ランチャーへのリクエストID
 */
//-----------------------------------------------------------------------------
launcher::ListData::RequestID LauncherIchiraku::SelectListener( u32 pos, launcher::LauncherProc * launcher )
{
  // キャンセル
  if( pos == launcher::LauncherProc::CURSOR_POS_CANCEL )
  {
    launcher->CreateListData<StartLauncher>();
    return launcher::ListData::REQID_RESET;
  }

  {
    gfl2::heap::HeapBase* heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->SetDebugData( GameSys::GameData::DEBUG_DATA_TYPE_DEFAULTNAME, heap, 0 );
  }

  switch( ItemList[pos] )
  {
  // P2pManagerテスト
  case IchirakuDebugMenuItem_01:
    {
#ifdef  GF_PLATFORM_CTR
      Test::P2PTestProc::Proc * proc = NIJI_PROC_CALL< GameSys::GameProcManager, Test::P2PTestProc::Proc >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() );
#endif
    }
    return launcher::ListData::REQID_CALL;
  // P2pConnectionManagerテスト
  case IchirakuDebugMenuItem_02:
    {
#ifdef  GF_PLATFORM_CTR
      Test::P2pConnectionManagerTestProc::Proc * proc = NIJI_PROC_CALL< GameSys::GameProcManager, Test::P2pConnectionManagerTestProc::Proc >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() );
#endif
    }
    return launcher::ListData::REQID_FINISH;
  // NBRアプリ
  case IchirakuDebugMenuItem_03:
    {
      static NetEvent::NBR::EVENT_PARAM param;
      param.hostNetID = 0;
      param.battleRule = Regulation::BATTLE_RULE_MULTI;
      ICHI_PRINT(" nbr param %x \n", &param);
      NetEvent::NBR::NBREvent::StartEvent( GFL_SINGLETON_INSTANCE( GameSys::GameManager ), &param );
    }
    return launcher::ListData::REQID_FINISH;
  // レギュレーションテスト
  case IchirakuDebugMenuItem_04:
    //RegulationTest();
    RegulationTest2();
    break;
  // バトルスポットアプリ
  case IchirakuDebugMenuItem_05:
    Test::RomTrainerPokeCheckTest::RomTrainerPokeCheckEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager));
    return launcher::ListData::REQID_CALL;
  }

  return launcher::ListData::REQID_NONE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// レギュレーションテスト
void LauncherIchiraku::RegulationTest()
{
  gfl2::heap::HeapBase* heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );


  // レギュレーションクラス
  Regulation regulation( heap );

  // GARCのレギュレーション読み込み
  regulation.LoadData( Regulation::PRESET_SINGLE_NORMAL );

  // DLしたレギュレーションはセットする
  // ↓
  //regulation.SetData( pDlRegulation, DlRegulationSize );

  regulation.DebugPrint();
  regulation.CheckCrc();

  GFL_PRINT("serialize data size = %d\n", regulation.GetSerializeDataSize() );

  // レギュレーション検査クラス
  PokeRegulation pokeRegulation( heap );

  // レギュレーションを設定する
  pokeRegulation.SetRegulation( &regulation );

  pml::PokeParty* checkParty = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();

  // 検査1
  PokeRegulation::ViolationDetails violationDetails; // エラー詳細データ
  pokeRegulation.CheckPokePartyForBtlTeam( checkParty, violationDetails );

  // 検査1
  pokeRegulation.CheckPokePartyForPokelist( checkParty );


  // 自動選択テスト
  {
    pml::PokeParty testParty( heap );
    pml::pokepara::PokemonParam* pp;
    // ピカチュウ
    pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_PIKATYUU, 50, pml::pokepara::INIT_SPEC_ID_RANDOM );
    testParty.AddMember( *pp );
    GFL_DELETE pp;
    // ライチュウ
    pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_RAITYUU, 50, pml::pokepara::INIT_SPEC_ID_RANDOM );
    testParty.AddMember( *pp );
    GFL_DELETE pp;
    // カイリキー
    pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_KAIRIKII, 50, pml::pokepara::INIT_SPEC_ID_RANDOM );
    testParty.AddMember( *pp );
    GFL_DELETE pp;
    // ゴーリキー
    pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_GOORIKII, 50, pml::pokepara::INIT_SPEC_ID_RANDOM );
    testParty.AddMember( *pp );
    GFL_DELETE pp;
    // メタモン
    pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_METAMON, 50, pml::pokepara::INIT_SPEC_ID_RANDOM );
    testParty.AddMember( *pp );
    GFL_DELETE pp;
  }
}

// レギュレーションテスト2
void LauncherIchiraku::RegulationTest2()
{
  gfl2::heap::HeapBase* heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  // レギュレーションクラス
  Regulation regulation( heap );

  // GARCのレギュレーション読み込み
  regulation.LoadData( Regulation::PRESET_SINGLE_NORMAL );

  // レギュレーション書き換え
  regulation.DebugSetValueParam( Regulation::SELECT_NUM_LO, 5 );
  regulation.DebugSetValueParam( Regulation::SELECT_NUM_HI, 5 );
  regulation.DebugSetParamMustPokeAND( 0, MONSNO_PIKATYUU , Regulation::ALL_FORM_SELECT );
  regulation.DebugSetParamMustPokeAND( 1, MONSNO_METAMON , Regulation::ALL_FORM_SELECT );
  regulation.DebugSetParamMustPokeAND( 2, MONSNO_RAITYUU , Regulation::ALL_FORM_SELECT );
  regulation.DebugSetParamMustPokeAND( 3, MONSNO_KAIRIKII , Regulation::ALL_FORM_SELECT );
  regulation.DebugSetParamMustPokeAND( 4, MONSNO_GOORIKII , Regulation::ALL_FORM_SELECT );


  // レギュレーション検査クラス
  PokeRegulation pokeRegulation( heap );

  // レギュレーションを設定する
  pokeRegulation.SetRegulation( &regulation );

  pml::PokeParty testParty( heap );
  pml::pokepara::PokemonParam* pp;
  // ピカチュウ
  pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_PIKATYUU, 50, pml::pokepara::INIT_SPEC_ID_RANDOM );
  testParty.AddMember( *pp );
  GFL_DELETE pp;
  // ライチュウ
  pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_RAITYUU, 50, pml::pokepara::INIT_SPEC_ID_RANDOM );
  testParty.AddMember( *pp );
  GFL_DELETE pp;
  // カイリキー
  pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_KAIRIKII, 50, pml::pokepara::INIT_SPEC_ID_RANDOM );
  testParty.AddMember( *pp );
  GFL_DELETE pp;
  // ゴーリキー
  pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_GOORIKII, 50, pml::pokepara::INIT_SPEC_ID_RANDOM );
  testParty.AddMember( *pp );
  GFL_DELETE pp;
  // メタモン
  pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_METAMON, 50, pml::pokepara::INIT_SPEC_ID_RANDOM );
  testParty.AddMember( *pp );
  GFL_DELETE pp;
  // ナッシー
  pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_NASSII, 50, pml::pokepara::INIT_SPEC_ID_RANDOM );
  testParty.AddMember( *pp );
  GFL_DELETE pp;

  s8 joinPokeIndex[6] = { 0,4,5,1,2,-1};
  s8 retJoinPokeIndex[6] ={0};
  pokeRegulation.SetJoinOrderTimeOutForPokelist( &testParty, joinPokeIndex, retJoinPokeIndex );

  ICHI_PRINT("結果 ");
  for( int i=0;i<6;i++)
  {
    ICHI_PRINT("%d ", retJoinPokeIndex[i]);
  }
  ICHI_PRINT("\n");


}

GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)

#endif  // PM_DEBUG
