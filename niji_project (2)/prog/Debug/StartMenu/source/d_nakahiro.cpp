#if PM_DEBUG
//=============================================================================
/**
 * @file    d_nakahiro.cpp
 * @brief   中村用ランチャーリスト処理
 * @author  Hiroyuki Nakamura
 * @date    2015.02.18
 */
//=============================================================================
#include "Debug/StartMenu/include/StartLauncher.h"
#include "Debug/StartMenu/include/d_nakahiro.h"

//#include "niji_conv_header/poke_lib/item/itemsym.h"
#include <pml/include/pmlib.h>

#include "arc_index/debug_message.gaix"
#include "niji_conv_header/message/debug/msg_d_nakahiro.h"
#include "niji_conv_header/message/msg_shop.h"

#include "System/include/HeapDefine.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/DllProc.h"
#include "Savedata/include/MyItemSave.h"
#include "Savedata/include/JoinFestaDataSave.h"

#include "App/FieldBag/include/BagProc.h"
#include "App/AppEvent/include/Bag/BagGamedata.h"
#include "App/FieldMenu/include/FieldMenuProc.h"

#include "App/AppEvent/include/Bag/BagEvent.h"
#include "App/Shop/include/ShopProc.h"
#include "App/AppEvent/include/Shop/ShopSellEvent.h"
#include "App/AppEvent/include/TownMap/TownMapEvent.h"
#include "App/AppEvent/include/TrainerPass/TrainerPassEvent.h"
#include "App/AppEvent/include/WazaOmoidasi/WazaOmoidasiEvent.h"

#include "App/TownMap/include/TownMapAppParam.h"
#include "App/TownMap/include/TownMapProc.h"

#include "App/ParurePokeList/include/ParurePokeListAppParam.h"
#include "App/ParurePokeList/include/ParurePokeListProc.h"

#include "App/WazaOmoidasi/include/WazaOmoidasiAppParam.h"
#include "App/WazaOmoidasi/include/WazaOmoidasiProc.h"

#include "Test/NakahiroTest/include/NakahiroTestProc.h"


GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(startmenu)

// 項目文字列データテーブル
const u32 LauncherNakahiro::ItemList[LauncherNakahiro::LIST_MAX] =
{
  label_nakahiro_test_23,   // PokeIconView
  label_nakahiro_test_10,   // ShopBuy1
  label_nakahiro_test_14,   // ShopBuy2
  label_nakahiro_test_11,   // ShopBuyBP1
  label_nakahiro_test_15,   // ShopBuyBP2
  label_nakahiro_test_12,   // ShopBuyFC1
  label_nakahiro_test_13,   // ShopBuyFC2
  label_nakahiro_test_07,   // ShopSell
  label_nakahiro_test_04,
  label_nakahiro_test_00,
  label_nakahiro_test_01,

  label_nakahiro_test_02,
  label_nakahiro_test_03,
  label_nakahiro_test_08,
  label_nakahiro_test_18,
  label_nakahiro_test_16,   // TownMap
  label_nakahiro_test_17,   // TownMap Fly
  label_nakahiro_test_19,   // TrainerPass
  label_nakahiro_test_20,   // TrainerPassDemo
  label_nakahiro_test_21,   // ParureList
  label_nakahiro_test_22,   // 技思い出し

//  label_nakahiro_test_05,   // ItemMake
};


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//-----------------------------------------------------------------------------
LauncherNakahiro::LauncherNakahiro(void)
{
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
LauncherNakahiro::~LauncherNakahiro()
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
gfl2::fs::ArcFile::ARCDATID LauncherNakahiro::GetArcDataID(void)
{
  return GARC_debug_message_d_nakahiro_DAT;
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
const u32 * LauncherNakahiro::GetListTable(void)
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
u32 LauncherNakahiro::GetListTableSize(void)
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
launcher::ListData::RequestID LauncherNakahiro::SelectListener( u32 pos, launcher::LauncherProc * launcher )
{
  // キャンセル
  if( pos == launcher::LauncherProc::CURSOR_POS_CANCEL )
  {
    launcher->CreateListData<StartLauncher>();
    return launcher::ListData::REQID_RESET;
  }

  switch( ItemList[pos] )
  {
  case label_nakahiro_test_00:
    InitItem();
    InitParty();
    {
      App::Event::BagEvent * ev_bag = App::Event::BagEvent::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager) );
      App::Event::BagEvent::SetupLauncherCall( true );
    }
    return launcher::ListData::REQID_FINISH;

  case label_nakahiro_test_01:
    InitItem();
    InitParty();
    { // @note イベント起動にしたい
      static app::bag::APP_PARAM param;
      static App::PokeList::IntermediateData pokemon;
      pml::PokeParty * party = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();
      const pml::PokeParty * const_party = party;
      pokemon.SetParameter( party );
      pokemon.SetParameter( const_party );
      param.pokemon = &pokemon;
      param.pokemon_index = 3;
      param.call_mode = app::bag::CALL_FIELD_POKELIST;
      param.item_manager = NULL;
      param.pocket[0].table = NULL;
      app::bag::BagProc * proc = GameSys::CallBagProc();
      proc->Setup( &param );
    }
    return launcher::ListData::REQID_CALL;

  case label_nakahiro_test_02:
    InitItem();
    InitParty();
    { // @note イベント起動にしたい
      static app::bag::APP_PARAM param;
      static App::PokeList::IntermediateData pokemon;
      pml::PokeParty * party = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();
      const pml::PokeParty * const_party = party;
      pokemon.SetParameter( party );
      pokemon.SetParameter( const_party );
      param.pokemon = &pokemon;
      param.call_mode = app::bag::CALL_BATTLE_MENU;
      param.item_manager = NULL;
      param.pocket[0].table = NULL;
//      param.pocket_max = BATTLE_BAG_POCKET_MAX;
      param.pocket_max = 2;
      app::bag::BagProc * proc = GameSys::CallBagProc();
      proc->Setup( &param );
    }
    return launcher::ListData::REQID_CALL;

  case label_nakahiro_test_03:
    InitItem();
    InitParty();
    { // @note イベント起動にしたい
      static app::bag::APP_PARAM param;
      static App::PokeList::IntermediateData pokemon;
      pml::PokeParty * party = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();
      const pml::PokeParty * const_party = party;
      pokemon.SetParameter( party );
      pokemon.SetParameter( const_party );
      param.pokemon = &pokemon;
      param.pokemon_index = 5;
      param.call_mode = app::bag::CALL_BOX;
      param.item_manager = NULL;
      param.pocket[0].table = NULL;
      app::bag::BagProc * proc = GameSys::CallBagProc();
      proc->Setup( &param );
    }
    return launcher::ListData::REQID_CALL;

  case label_nakahiro_test_07:  // ショップ：売る
    InitItem();
    {
      App::Event::ShopSellEvent * ev = App::Event::ShopSellEvent::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager) );
      App::Event::ShopSellEvent::SetupLauncherCall( true );
    }
    return launcher::ListData::REQID_FINISH;

  case label_nakahiro_test_10:  // ShopBuy1
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetMisc()->SetGold( 20000 );
    {
      static App::Shop::APP_PARAM param;
      InitShopParam(
        &param,
        App::Shop::TYPE_FR_SHOP,
        0,
        msg_shop_11_01,     // howmany_msg       メッセージ：いくつ買う？
        msg_shop_12_01,     // really_msg        メッセージ：xxコでxx円です
        msg_shop_12_02,     // really_msg2       メッセージ：xxコでxx円です（複数形）
        msg_shop_06_01,     // hereitis_msg      メッセージ：まいど
        msg_shop_06_03,     // youcanttake_msg   メッセージ：持ちきれない
        msg_shop_06_02,     // nomoney_msg       メッセージ：お金が足りない
        msg_jcshop_01_01 ); // selected_msg      メッセージ：今日は終了しました（フェスショップ用）
      App::Shop::ShopProc * proc = GameSys::CallShopProc();
      proc->Setup( &param, gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE) );
    }
    return launcher::ListData::REQID_CALL;

  case label_nakahiro_test_14:  // ShopBuy2
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetMisc()->SetGold( 20000 );
    {
      static App::Shop::APP_PARAM param;
      InitShopParam(
        &param,
        App::Shop::TYPE_FR_SHOP,
        15,
        msg_shop_11_01,     // howmany_msg       メッセージ：いくつ買う？
        msg_wazashop_03_01, // really_msg        メッセージ：xxコでxx円です
        msg_wazashop_03_01, // really_msg2       メッセージ：xxコでxx円です（複数形）
        msg_shop_06_01,     // hereitis_msg      メッセージ：まいど
        msg_shop_06_03,     // youcanttake_msg   メッセージ：持ちきれない
        msg_shop_06_02,     // nomoney_msg       メッセージ：お金が足りない
        msg_jcshop_01_01 ); // selected_msg      メッセージ：今日は終了しました（フェスショップ用）
      App::Shop::ShopProc * proc = GameSys::CallShopProc();
      proc->Setup( &param, gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE) );
    }
    return launcher::ListData::REQID_CALL;

  case label_nakahiro_test_11:  // ShopBuyBP1
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetMisc()->SetBP( 500 );
    {
      static App::Shop::APP_PARAM param;
      InitShopParam(
        &param,
        App::Shop::TYPE_BP_SHOP,
        1,
        msg_bpshop_04_01,   // howmany_msg       メッセージ：いくつ買う？
        msg_bpshop_05_01,   // really_msg        メッセージ：xxコでxx円です
        msg_bpshop_05_02,   // really_msg2       メッセージ：xxコでxx円です（複数形）
        msg_shop_06_01,     // hereitis_msg      メッセージ：まいど
        msg_shop_06_03,     // youcanttake_msg   メッセージ：持ちきれない
        msg_bpshop_03_01,   // nomoney_msg       メッセージ：お金が足りない
        msg_jcshop_01_01 ); // selected_msg      メッセージ：今日は終了しました（フェスショップ用）
      App::Shop::ShopProc * proc = GameSys::CallShopProc();
      proc->Setup( &param, gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE) );
    }
    return launcher::ListData::REQID_CALL;

  case label_nakahiro_test_15:  // ShopBuyBP2
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetMisc()->SetBP( 500 );
    {
      static App::Shop::APP_PARAM param;
      InitShopParam(
        &param,
        App::Shop::TYPE_BP_SHOP,
        1,
        msg_bpshop_04_01,   // howmany_msg       メッセージ：いくつ買う？
        msg_bpshop_06_01,   // really_msg        メッセージ：xxコでxx円です
        msg_bpshop_06_01,   // really_msg2       メッセージ：xxコでxx円です（複数形）
        msg_shop_06_01,     // hereitis_msg      メッセージ：まいど
        msg_shop_06_03,     // youcanttake_msg   メッセージ：持ちきれない
        msg_bpshop_03_01,   // nomoney_msg       メッセージ：お金が足りない
        msg_jcshop_01_01 ); // selected_msg      メッセージ：今日は終了しました（フェスショップ用）
      App::Shop::ShopProc * proc = GameSys::CallShopProc();
      proc->Setup( &param, gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE) );
    }
    return launcher::ListData::REQID_CALL;

  case label_nakahiro_test_12:  // ShopBuyFC1
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData()->SetCoin( 500 );
    {
      static App::Shop::APP_PARAM param;
      InitShopParam(
        &param,
        App::Shop::TYPE_FC_ITEM,
        0,
        msg_bpshop_04_01,   // howmany_msg       メッセージ：いくつ買う？
        msg_jcshop_02_01,   // really_msg        メッセージ：xxコでxx円です
        msg_jcshop_02_01,   // really_msg2       メッセージ：xxコでxx円です（複数形）
        msg_jcshop_03_01,   // hereitis_msg      メッセージ：まいど
        msg_jcshop_07_01,   // youcanttake_msg   メッセージ：持ちきれない
        msg_jcshop_06_01,   // nomoney_msg       メッセージ：お金が足りない
        msg_jcshop_01_01 ); // selected_msg      メッセージ：今日は終了しました（フェスショップ用）
      App::Shop::ShopProc * proc = GameSys::CallShopProc();
      proc->Setup( &param, gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE) );
    }
    return launcher::ListData::REQID_CALL;

  case label_nakahiro_test_13:  // ShopBuyFC2
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData()->SetCoin( 500 );
    {
      static App::Shop::APP_PARAM param;
      InitShopParam(
        &param,
        App::Shop::TYPE_FC_ACTION,
        0,
        msg_bpshop_04_01,   // howmany_msg       メッセージ：いくつ買う？
        msg_jcshop_02_02,   // really_msg        メッセージ：xxコでxx円です
        msg_jcshop_02_02,   // really_msg2       メッセージ：xxコでxx円です（複数形）
        msg_jcshop_03_02,   // hereitis_msg      メッセージ：まいど
        msg_jcshop_07_02,   // youcanttake_msg   メッセージ：持ちきれない
        msg_jcshop_06_02,   // nomoney_msg       メッセージ：お金が足りない
        msg_jcshop_01_02 ); // selected_msg      メッセージ：今日は終了しました（フェスショップ用）
      App::Shop::ShopProc * proc = GameSys::CallShopProc();
      proc->Setup( &param, gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE) );
    }
    return launcher::ListData::REQID_CALL;

  case label_nakahiro_test_04:
    InitParty();
    {
      Savedata::FieldMenu * fieldmenu = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetFieldMenu();

      for( int i=0; i<Savedata::FieldMenu::PAGE_MAX; i++ )
      {
        fieldmenu->SetMenuPageEnable( i );
        for( int j=0; j<Savedata::FieldMenu::PAGE_ICON_MAX; j++ )
        {
          Savedata::FieldMenu::IconID id = static_cast<Savedata::FieldMenu::IconID>( i*Savedata::FieldMenu::PAGE_ICON_MAX+j );
          if( fieldmenu->IsMenuIconEnable(id) == false )
          {
            fieldmenu->SetMenuIconID( i, j, id );
          }
        }
      }

      // 各ボタン表示のためイベントフラグをセット
      Field::EventWork * evwk = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
      evwk->SetEventFlag( SYS_FLAG_BOUKEN_NAVI_GET );
      evwk->SetEventFlag( SYS_FLAG_POKE_FINDER_GET );
      evwk->SetEventFlag( SYS_FLAG_POKE_RIDE_OPEN );
      // ライド設定
      evwk->SetEventFlag( SYS_FLAG_KENTAROSU_RIDE_OPEN );
    }
    GameSys::DebagCallFieldMenu();
    return launcher::ListData::REQID_CALL;

  case label_nakahiro_test_05:
    GameSys::CallDebugItemMakeProc();
    return launcher::ListData::REQID_CALL;

  case label_nakahiro_test_08:
    {
      Test::Nakahiro::TestProc * proc = GameSys::CallNakahiroProc();
      proc->SetupMode( Test::Nakahiro::TestProc::MODE_H_SCROLL );
    }
    return launcher::ListData::REQID_CALL;

  case label_nakahiro_test_18:
    {
      Test::Nakahiro::TestProc * proc = GameSys::CallNakahiroProc();
      proc->SetupMode( Test::Nakahiro::TestProc::MODE_BOX_CHAT_LOW );
    }
    return launcher::ListData::REQID_CALL;

  case label_nakahiro_test_16:   // TownMap
    {
      App::Event::TownMapEvent * ev = App::Event::TownMapEvent::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager) );
      App::Event::TownMapEvent::SetupLauncherCall( true );
      ev->SetParam( App::TownMap::MODE_TOWNMAP );
    }
    return launcher::ListData::REQID_FINISH;
  case label_nakahiro_test_17:   // TownMap Fly
    {
      App::Event::TownMapEvent * ev = App::Event::TownMapEvent::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager) );
      App::Event::TownMapEvent::SetupLauncherCall( true );
      ev->SetParam( App::TownMap::MODE_FLY );
    }
    return launcher::ListData::REQID_FINISH;

  case label_nakahiro_test_19:  // TrainerPass
    {
      App::Event::TrainerPassEvent * ev = App::Event::TrainerPassEvent::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager) );
      ev->SetupLauncherCall( true );
    }
    return launcher::ListData::REQID_FINISH;

  case label_nakahiro_test_20:  // TrainerPassDemo
    {
      App::Event::TrainerPassEvent * ev = App::Event::TrainerPassEvent::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager) );
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetMisc()->SetTrainerPassStamp(static_cast<Savedata::TrainerPassStampID>(1));
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetMisc()->SetTrainerPassStamp(static_cast<Savedata::TrainerPassStampID>(2));
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetMisc()->SetTrainerPassStamp(static_cast<Savedata::TrainerPassStampID>(3));
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetMisc()->SetTrainerPassStamp(static_cast<Savedata::TrainerPassStampID>(4));
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetMisc()->SetTrainerPassStamp(static_cast<Savedata::TrainerPassStampID>(5));
      ev->SetStampMode( 5 );
      ev->SetupLauncherCall( true );
    }
    return launcher::ListData::REQID_FINISH;

  case label_nakahiro_test_21:  // ParureList
    InitParty();
    {
      static App::PokeParure::APP_PARAM param;
      App::PokeParure::ParurePokeListProc * proc = GameSys::CallParurePokeListProc();
      proc->Setup( &param );
    }
    return launcher::ListData::REQID_CALL;

  case label_nakahiro_test_22:  // 技思い出し
    InitParty();
    {
      static App::Event::WazaOmoidasiEvent::RESULT_PARAM result;
      App::Event::WazaOmoidasiEvent * ev = App::Event::WazaOmoidasiEvent::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager) );
//      ev->SetParam( 3, &result );
      ev->SetParam( 4, &result );
      ev->SetupLauncherCall( true );
    }
//    return launcher::ListData::REQID_CALL;
    return launcher::ListData::REQID_FINISH;
//    GameSys::CallStaffListProc();
//    return launcher::ListData::REQID_CALL;

  case label_nakahiro_test_23:  // PokeIconView
    GameSys::CallDebugPokeIconViewProc();
    return launcher::ListData::REQID_CALL;
  }

  return launcher::ListData::REQID_NONE;
}

void LauncherNakahiro::InitItem(void)
{
#if 0
  static const u16 tbl[] =
  {
    ITEM_MASUTAABOORU,
    ITEM_HAIPAABOORU,
    ITEM_SUUPAABOORU,
    ITEM_MONSUTAABOORU,

    ITEM_KIZUGUSURI,
    ITEM_DOKUKESI,
    ITEM_YAKEDONAOSI,
    ITEM_KOORINAOSI,
    ITEM_NEMUKEZAMASI,
    ITEM_MAHINAOSI,
    ITEM_KAIHUKUNOKUSURI,
    ITEM_MANTANNOKUSURI,
    ITEM_SUGOIKIZUGUSURI,
    ITEM_IIKIZUGUSURI,
    ITEM_NANDEMONAOSI,
    ITEM_GENKINOKAKERA,
    ITEM_GENKINOKATAMARI,
    ITEM_OISIIMIZU,
    ITEM_SAIKOSOODA,
    ITEM_MIKKUSUORE,
    ITEM_MOOMOOMIRUKU,
    ITEM_TIKARANOKONA,
    ITEM_TIKARANONEKKO,
    ITEM_BANNOUGONA,
    ITEM_HUKKATUSOU,
    ITEM_PIIPIIEIDO,
    ITEM_PIIPIIRIKABAA,
    ITEM_PIIPIIEIDAA,
    ITEM_PIIPIIMAKKUSU,
    ITEM_HUENSENBEI,
    ITEM_KINOMIZYUUSU,
    ITEM_SEINARUHAI,
    ITEM_MAKKUSUAPPU,

    ITEM_WAZAMASIN01,
    ITEM_WAZAMASIN02,
    ITEM_WAZAMASIN03,
    ITEM_WAZAMASIN04,
    ITEM_WAZAMASIN05,
    ITEM_WAZAMASIN06,
    ITEM_WAZAMASIN07,
    ITEM_WAZAMASIN08,
    ITEM_WAZAMASIN09,
    ITEM_WAZAMASIN10,
    ITEM_WAZAMASIN11,
    ITEM_WAZAMASIN12,
    ITEM_WAZAMASIN13,
    ITEM_WAZAMASIN14,
    ITEM_WAZAMASIN15,
    ITEM_WAZAMASIN16,
    ITEM_WAZAMASIN17,
    ITEM_WAZAMASIN18,
    ITEM_WAZAMASIN19,
    ITEM_WAZAMASIN20,

    ITEM_IDENSINOKUSABI,
    ITEM_HAKKINDAMA,
    ITEM_GURASIDEANOHANA,
  };

  Savedata::MyItem * myitem = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetMyItem();

  for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
  {
    myitem->Add( tbl[i], 100 );
  };
#else
/*
  Savedata::MyItem * myitem = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetMyItem();
  for( u32 i=1; i<=ITEM_DATA_MAX; i++ )
  {
    myitem->Add( i, 100 );
  };
*/
#endif
}

void LauncherNakahiro::InitParty(void)
{
  pml::PokeParty * party = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();

  gfl2::heap::HeapBase * heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

	pml::pokepara::PokemonParam * pp;

//	pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_TORUNEROSU, 5, 456123 );
	pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_KYUREMU, 100, 456123 );
//	pp->SetHp( pp->GetMaxHp()/2 );
	pp->SetHp( 10 );
	pp->SetSick( pml::pokepara::SICK_MAHI );
	party->ReplaceMember( 0, *pp );
	GFL_DELETE pp;

	pp = GFL_NEW_LOW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_TUTININ, 19, 456123 );
//	pp = GFL_NEW_LOW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_TUTININ, 24, 456123 );
//	pp = GFL_NEW_LOW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_TEKKANIN, 19, 456123 );
//	pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_ZEKUROMU, 10, 456123 );
 	pp->SetHp( 0 );
	pp->SetWaza( 1, WAZANO_NAMINORI );
//	pp->SetSick( pml::pokepara::SICK_NEMURI );
  pp->SetDirtType( pml::pokepara::DIRT_TYPE_HAIR );
  pp->SetDirtPos( 0 );
	party->ReplaceMember( 1, *pp );
  GFL_DELETE pp;

	pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_PIKATYUU, 1, 456123 );
//	pp->SetSick( pml::pokepara::SICK_KOORI );
//  pp->SetEggFlag();
	party->ReplaceMember( 2, *pp );
	GFL_DELETE pp;

//	pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_MAAIIKA, 99, 456123 );
	pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_GIRATHINA, 5, 456123 );
	pp->SetSick( pml::pokepara::SICK_DOKU );
  pp->SetItem( ITEM_OBONNOMI );
//  pp->SetEggFlag();
  party->ReplaceMember( 3, *pp );
	GFL_DELETE pp;

//	pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_SHEIMI, 7, 456123 );
//	pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_KYUREMU, 100, 456123 );
	pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_HUSIGIBANA, 32, 456123 );
	pp->SetTamagoWazaNo( 0, WAZANO_AMAERU );
	pp->SetTamagoWazaNo( 1, WAZANO_MAZIKARURIIHU );
	pp->SetHp( 0 );
  pp->ChangeEffortPowerDirect( pml::pokepara::POWER_AGI, 252 );
	party->ReplaceMember( 4, *pp );
	GFL_DELETE pp;
/*
  pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_GIRATHINA, 10, 456123 );
//	pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_ZEKUROMU, 99, 456123 );
//	pp->SetEggFlag();
// 	pp->SetHp( 0 );
//	pp->SetSick( pml::pokepara::SICK_YAKEDO );

  pp->SetWaza( 0, WAZANO_HANERU );
  pp->SetWaza( 1, WAZANO_HATAKU );
  pp->SetWaza( 2, WAZANO_TATAKITUKERU );
  pp->SetWaza( 3, WAZANO_HAKAIKOUSEN );

	party->ReplaceMember( 5, *pp );
	GFL_DELETE pp;
*/
}

void LauncherNakahiro::InitShopParam(
      App::Shop::APP_PARAM * param,
      App::Shop::ShopType type,
      u32 id,
      u32 howmany_msg,
      u32 really_msg,
      u32 really_msg2,
      u32 hereitis_msg,
      u32 youcanttake_msg,
      u32 nomoney_msg,
      u32 selected_msg )
{
  param->type            = type;
  param->id              = id;
  param->howmany_msg     = howmany_msg;
  param->really_msg      = really_msg;
  param->really_msg2     = really_msg2;
  param->hereitis_msg    = hereitis_msg;
  param->youcanttake_msg = youcanttake_msg;
  param->nomoney_msg     = nomoney_msg;
  param->selected_msg    = selected_msg;
}



GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)

#endif  // PM_DEBUG
