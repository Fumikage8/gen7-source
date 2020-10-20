#if PM_DEBUG
//=============================================================================
/**
 * @file    StartLauncher.cpp
 * @brief   ゲーム開始時のランチャー用リスト処理
 * @author  Hiroyuki Nakamura
 * @date    2015.02.18
 */
//=============================================================================
#include <Debug/StartMenu/include/StartLauncher.h>
#include <Debug/StartMenu/include/d_nakahiro.h>
#include <Debug/StartMenu/include/d_obata.h>
#include <Debug/DebugBattle/include/DebugBattleProc.h>
#include <Test/MenuFrameTest/include/DebugPartsLauncher.h>

#include <Debug/StartMenu/include/d_uchida.h>
#include <Debug/StartMenu/include/d_endo.h>
#include <debug/StartMenu/include/d_fukushima.h>
#include <debug/StartMenu/include/d_kousaka.h>
#include <Debug/StartMenu/include/d_ichiraku.h>
#include <Debug/StartMenu/include/d_takeda.h>
#include <Debug/StartMenu/include/d_kawada.h>
#include <Debug/StartMenu/include/d_muto.h>
#include <Debug/StartMenu/include/d_takebe.h>
#include <Debug/StartMenu/include/d_kanamaru.h>
#include <Debug/StartMenu/include/d_araki.h>
#include <Debug/StartMenu/include/d_movie.h>
#include <Debug/StartMenu/include/d_nagi.h>

#include <System/include/HeapDefine.h>
#include <GameSys/include/GameProcManager.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/NijiTime.h>
#include <GameSys/include/DllProc.h>
#include "Debug/StartMenu/include/d_ariizumi.h"
#include "App/LunaSolDive/include/Proc.h"
#include "Test/PeteTest/include/PeteTestProc.h"
#include "Test/IshiguroTest/include/IshiguroTestProc.h"
#include "Test/ArakiTest/include/ArakiTestProc.h"
#include "Test/CharaViewer/include/CharaViewerProc.h"
#include "Test/PokeViewer/include/PokeViewerProc.h"
#include "System/include/HeapDefine.h"
#include "Field/FieldStatic/include/Event/EventGameStart.h"
#include "Test/AnaiTest/include/AnaiTestProc.h"
#include "Test/Skelton/include/SkeltonProc.h"
#include <App/MantainSurf/include/MantainSurfProc.h>

#include <arc_index/debug_message.gaix>
#include <niji_conv_header/message/debug/msg_d_startmenu.h>

// DynamicHeader
#include <debug/include/DynamicHeader/gfl2_DebugDynamicHeader.h>
#include <system/include/ArcIdGetter.h>


#define NIJI_START_LAUNCHER_MSG_HEADER_FILE_PATH "arc_index/debug_message.gaix"

#define NIJI_START_LAUNCHER_MSG_TABLE_CAP_NUM 256

#include GFL_DYNAMIC_HEADER(NIJI_START_LAUNCHER_MSG_HEADER_FILE_PATH)


GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(startmenu)

// 項目文字列データテーブル
const u32 StartLauncher::ItemList[StartLauncher::LIST_MAX] =
{
  d_startmenu_label_40,   // Field NetTest
  d_startmenu_label_25,   // Field
  d_startmenu_label_47,   // Field:TopPfmDress
  d_startmenu_label_29,   // FieldDefaultName
  d_startmenu_label_01,   // Battle
  d_startmenu_label_28,   // CharaViewer
#if defined(GF_PLATFORM_WIN32)
  d_startmenu_label_32,   // Skelton
#else
  d_startmenu_label_33,   // dummy SkeltonはWin32版のみ、表示位置をずらさないためDummyを追加
#endif
  d_startmenu_label_34,   // SoundTest
  d_startmenu_label_36,   // LangSelect
  d_startmenu_label_02,   // Tamada
  d_startmenu_label_03,   // Ishiguro
  d_startmenu_label_04,   // Uematsu
  d_startmenu_label_05,   // Takebe
  //d_startmenu_label_16,   // Gomi
  //d_startmenu_label_06,   // Takahashi
  d_startmenu_label_17,   // Saito
  d_startmenu_label_07,   // Ikuta
  d_startmenu_label_08,   // Saita
  d_startmenu_label_09,   // Kanamaru
  d_startmenu_label_18,   // Ikeuchi
  d_startmenu_label_10,   // Iwasawa
  d_startmenu_label_19,   // Takeda
  d_startmenu_label_11,   // Ohno
  d_startmenu_label_12,   // Kawada
  d_startmenu_label_13,   // Nakamura
  d_startmenu_label_20,   // Fukushima
  d_startmenu_label_21,   // Uchida
  d_startmenu_label_14,   // Ariizumi
  d_startmenu_label_22,   // Obata
  d_startmenu_label_15,   // Kousaka
  d_startmenu_label_23,   // Ichiraku
  d_startmenu_label_24,   // Endo
  d_startmenu_label_26,   // Pete
  d_startmenu_label_27,   // Muto
  d_startmenu_label_30,   // Anai
  d_startmenu_label_31,   // Araki
  d_startmenu_label_35,   // PokeViewer
  d_startmenu_label_37,   // WordSetTest
  d_startmenu_label_39,   // Movie
  d_startmenu_label_38,   // Nagi
  d_startmenu_label_41,   // FontViewer
  d_startmenu_label_42,   // ModelViewer
  d_startmenu_label_43,   // Harada
  d_startmenu_label_44,   // MantainSurf
  d_startmenu_label_45,   // LunaSolDive
};

//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//-----------------------------------------------------------------------------
StartLauncher::StartLauncher(void)
{
  GFL_LOAD_HEADER_HASH( NIJI_START_LAUNCHER_MSG_HEADER_FILE_PATH, NIJI_START_LAUNCHER_MSG_TABLE_CAP_NUM );

  // アローラ時間のセットアップ
  s64 alola_offset = GameSys::AlolaTime::GetAlolaOffset( GET_VERSION(),GameSys::AlolaTime::ALOLA_OTHER_WOLRD_TYPE_NORMAL ); //@fix ゲーム開始時間固定にあわせデバッグ開始は通常アローラタイムに変更
  GameSys::GameData *gameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  gameData->GetGameTime()->SetAlolaOffset( alola_offset );
  GameSys::AlolaTime::ClearAlolaOtherWorldOffset( gameData );
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
StartLauncher::~StartLauncher()
{
  GFL_UNLOAD_HEADER_HASH( NIJI_START_LAUNCHER_MSG_HEADER_FILE_PATH );
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
gfl2::fs::ArcFile::ARCDATID StartLauncher::GetArcDataID(void)
{
  return GFL_VALUE_DEFINE_IN_HEADER(GARC_debug_message_d_startmenu_DAT);
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
const u32 * StartLauncher::GetListTable(void)
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
u32 StartLauncher::GetListTableSize(void)
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
 *
 * @note
 *    ・別Procを呼ぶ場合：
 *        NIJI_PROC_CALL<...>(...);
 *        return launcher::ListData::REQID_CALL;
 *
 *    ・項目リストを変更する場合：
 *        launcher->CreateListData<...>();
 *        return launcher::ListData::REQID_RESET;
 */
//-----------------------------------------------------------------------------
launcher::ListData::RequestID StartLauncher::SelectListener( u32 pos, launcher::LauncherProc * launcher )
{
  // キャンセル
  if( pos == launcher::LauncherProc::CURSOR_POS_CANCEL )
  {
#if 0  // @note 止まっちゃうのでなにもしないようにしている
    return launcher::ListData::REQID_FINISH;
#else
    return launcher::ListData::REQID_NONE;
#endif
  }

  switch( ItemList[pos] )
  {
  case d_startmenu_label_25:   // Field
    {
      Field::EventGameStart::Description desc = {
        Field::EventGameStart::GAMEINIT_MODE_DEBUG_FIRST,
        GFL_SINGLETON_INSTANCE(GameSys::GameManager)
      };
      Field::EventGameStart::CallEventGameStart( desc );
    }
    return launcher::ListData::REQID_FINISH;

  case d_startmenu_label_29:   // Field DefName
    {
      Field::EventGameStart::Description desc = {
        Field::EventGameStart::GAMEINIT_MODE_DEBUG_DEFAULTNAME,
        GFL_SINGLETON_INSTANCE(GameSys::GameManager)
      };
      Field::EventGameStart::CallEventGameStart( desc );
    }
    return launcher::ListData::REQID_FINISH;

  case d_startmenu_label_40:
    {
      Field::EventGameStart::Description desc = {
        Field::EventGameStart::GAMEINIT_MODE_DEBUG_NETTEST,
        GFL_SINGLETON_INSTANCE(GameSys::GameManager)
      };
      Field::EventGameStart::CallEventGameStart( desc );
    }
    return launcher::ListData::REQID_FINISH;

  case d_startmenu_label_47:
  {
    Field::EventGameStart::Description desc = {
      Field::EventGameStart::GAMEINIT_MODE_DEBUG_FIRST_TOP_PFM_DRESS,
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)
    };
    Field::EventGameStart::CallEventGameStart(desc);
  }
  return launcher::ListData::REQID_FINISH;

  case d_startmenu_label_01:   // Battle
    this->CallDebugBattleProc();
    return launcher::ListData::REQID_CALL;

#if defined(GF_PLATFORM_WIN32)
  case d_startmenu_label_32:   // Skelton
    NIJI_PROC_CALL< GameSys::GameProcManager, Test::Skelton::SkeltonProc >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() );
    return launcher::ListData::REQID_CALL;
#endif
  
  case d_startmenu_label_34:   // SoundTest
    this->CallSoundTestProc();
    return launcher::ListData::REQID_CALL;

  case d_startmenu_label_36:  //langselect
    launcher->CreateListData<Test::MenuFrameTest::DebugLangSelectLauncher>();
    return launcher::ListData::REQID_RESET;

  case d_startmenu_label_02:   // Tamada
    break;

  case d_startmenu_label_03:   // Ishiguro
		{
			gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
      gfl2::ro::Module *fieldRoModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "FieldRo.cro");
      roMgr->StartModule(fieldRoModule);

			NIJI_RO_PROC_CALL< GameSys::GameProcManager, Test::IshiguroTest::IshiguroTestProc >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "IshiguroTest.cro" );
		}
    break;

  case d_startmenu_label_04:   // Uematsu
    break;

  case d_startmenu_label_05:   // Takebe
    launcher->CreateListData<debug::startmenu::TakebeTestLauncher>();
    return launcher::ListData::REQID_RESET;

  case d_startmenu_label_06:   // Takahashi
    break;

  case d_startmenu_label_07:   // Ikuta
    break;

  case d_startmenu_label_08:   // Saita
    break;

  case d_startmenu_label_09:   // Kanamaru
    launcher->CreateListData<debug::startmenu::KanamaruTestLauncher>();
    return launcher::ListData::REQID_RESET;

  case d_startmenu_label_10:   // Iwasawa
    break;

  case d_startmenu_label_11:   // Ohno
    launcher->CreateListData<Test::MenuFrameTest::DebugPartsLauncher>();
    return launcher::ListData::REQID_RESET;    

  case d_startmenu_label_12:   // Kawada
    launcher->CreateListData<debug::startmenu::KawadaTestLauncher>();
    return launcher::ListData::REQID_RESET;

  case d_startmenu_label_13:   // Nakamura
    launcher->CreateListData<LauncherNakahiro>();
    return launcher::ListData::REQID_RESET;

  case d_startmenu_label_14:   // Ariizumi
    launcher->CreateListData<debug::startmenu::AriTestLauncher>();
    return launcher::ListData::REQID_RESET;
    //break;

  case d_startmenu_label_15:   // Kousaka
    launcher->CreateListData<LauncherKousaka>();
    return launcher::ListData::REQID_RESET;

  case d_startmenu_label_16:   // Gomi
    break;
  case d_startmenu_label_17:   // Saito
    this->CallSoundTestProc();
    return launcher::ListData::REQID_CALL;

  case d_startmenu_label_18:   // Ikeuchi
    break;

  case d_startmenu_label_19:   // Takeda
    launcher->CreateListData<LauncherTakeda>();
    return launcher::ListData::REQID_RESET;

  case d_startmenu_label_20:   // Fukushima
    launcher->CreateListData<LauncherFukushima>();
    return  launcher::ListData::REQID_RESET;    

  case d_startmenu_label_21:   // Uchida
    //  個人作業用のデバッグ画面を実行
    launcher->CreateListData<LauncherUchida>();
    return  launcher::ListData::REQID_RESET;

  case d_startmenu_label_22:   // Obata
    launcher->CreateListData<LauncherObata>();
    return launcher::ListData::REQID_RESET;

  case d_startmenu_label_23:   // Ichiraku
    launcher->CreateListData<LauncherIchiraku>();
    return launcher::ListData::REQID_RESET;

  case d_startmenu_label_24:   // Endo
    launcher->CreateListData<LauncherEndo>();
    return launcher::ListData::REQID_RESET;

  case d_startmenu_label_26:   // Pete
		GameSys::CallPeteTestProc();
		break;
	      
  case d_startmenu_label_27:   // Muto
    launcher->CreateListData<LauncherMuto>();
    return launcher::ListData::REQID_RESET;

  case d_startmenu_label_28:   // CharaViewer
    NIJI_RO_PROC_CALL< GameSys::GameProcManager, test::chara_viewer::CharaViewerProc >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "CharaViewer.cro" );	 
    break;

  case d_startmenu_label_30:   // Anai
//    GameSys::CallAnaiTestProc();
    break;
  case d_startmenu_label_31:   // Araki
    launcher->CreateListData<debug::startmenu::ArakiTestLauncher>();
    return launcher::ListData::REQID_RESET;

  case d_startmenu_label_35:   // PokeViewer
    NIJI_RO_PROC_CALL<GameSys::GameProcManager, test::pokeviewer::PokeViewerProc>(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager(), GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager(), "PokeViewer.cro");
    break;
  case d_startmenu_label_37:   // WordSetTest
    GameSys::CallDebugWordSetTest();
    break;
  case d_startmenu_label_39:   // Movie
    launcher->CreateListData<debug::startmenu::MovieTestLauncher>();
    return launcher::ListData::REQID_RESET;
  case d_startmenu_label_38:  // Nagi
    launcher->CreateListData<debug::startmenu::NagiTestLauncher>();
    return launcher::ListData::REQID_RESET;

  case d_startmenu_label_41:   // FontViewer
    GameSys::CallDebugFontViewer();
    break;

  case  d_startmenu_label_42:   // ModelViewer
    GameSys::CallDebugModelViewer();
    break;

  case d_startmenu_label_43:   // Harada
    GameSys::CallFinderStudioTestProc();
    break;

  case d_startmenu_label_44:   // MantainSurf
    NIJI_RO_PROC_CALL<GameSys::GameProcManager, App::MantainSurf::MantainSurfProc>(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager(), GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager(), "MantainSurf.cro");
    break;
  case d_startmenu_label_45:// LunaSolDive
    {
      LunaSolDive::Proc* const pProc = NIJI_RO_PROC_CALL<GameSys::GameProcManager, LunaSolDive::Proc>(
                  GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager(),
                  GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager(),
                  "LunaSolDive.cro");
      pProc->SwitchToDebugMode();
      break;
    }
  }

  return launcher::ListData::REQID_NONE;
}

/**
 * @brief デバッグバトルプロセスをコールする
 */
void StartLauncher::CallDebugBattleProc()
{
  gfl2::heap::HeapBase* systemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  gfl2::heap::HeapBase* deviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  GameSys::CallDebugBattleProc( systemHeap, deviceHeap );
}

/**
 * @brief サウンドテストプロセスをコールする
 */
void StartLauncher::CallSoundTestProc()
{
  gfl2::heap::HeapBase* systemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  gfl2::heap::HeapBase* deviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  GameSys::CallSoundTestProc( systemHeap, deviceHeap );
}


GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)

#endif  // PM_DEBUG
