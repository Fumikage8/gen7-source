//===================================================================
/**
 *	@file	  StartEvent.cpp
 *	@brief  スタートイベント
 *	@author	k.ohno
 *	@date		2015.5.31
 */
//===================================================================

#include "StartEvent.h"

#include "System/include/HeapDefine.h"
#include <GameSys/include/GameProcManager.h>


#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"

#include "Field/FieldStatic/include/Event/EventGameStart.h"

#include <App/TitleMenu/include/TitleMenuProc.h>
#include <App/LangSelect/include/LangSelectProc.h>
#include <App/HeroSelect/include/HeroSelectProc.h>

#include <App/AppDemo/include/AppDemoProcParam.h>

#include <Savedata/include/MyStatus.h>
#include <ExtSavedata/include/NijiExtSaveSystem.h>
#include <App/LangSelect/include/LangSelectAppParam.h>

#include "NetStatic\NetEvent\include\LiveEvent.h"  //live大会
#include "Netstatic/NetEvent/include/MysteryGiftEvent.h" //不思議な贈り物

#include <Movie/MovieLib/source/MovieProc.h>
#include <App/AppEvent/include/Movie/MovieAppParam.h> // タイトルムービー
#include "Field/FieldStatic/include/Debug/DebugStartSettingAccessor.h"

// arc
#include <arc_index/demo_seq_script.gaix>

#if PM_DEBUG
#if defined(GF_PLATFORM_WIN32)
#include <shlwapi.h>
#include <locale.h>
#pragma comment( lib, "Shlwapi.lib" ) 
#endif
#endif

GFL_NAMESPACE_BEGIN( Main )

//-----------------------------------------------------------------------------
/**
 *					定数宣言
 */
//-----------------------------------------------------------------------------
#define DEBUG_INTRO       0    //!< イントロデモの最後にタイトルメニューにつなげるフラグ

#ifndef SKIP_LANG_SELECT       // 外部環境から渡す想定、渡されない場合コード内の定義を有効とする
#define SKIP_LANG_SELECT  0    //!< 言語選択のスキップ
#endif // !SKIP_LANG_SELECT

#define INTRO_DEMO_FIRST    GARC_demo_seq_script_sd9030_gamestart01_BSEQ
#define INTRO_DEMO_SECOND   GARC_demo_seq_script_sd9031_gamestart02_BSEQ
#define INTRO_DEMO_THIRD_S  GARC_demo_seq_script_ev0020_s_escape_BSEQ
#define INTRO_DEMO_THIRD_M  GARC_demo_seq_script_ev0020_m_escape_BSEQ

enum
{
  EVSEQ_CHECK_SAVE,
  EVSEQ_LOAD_START,
  EVSEQ_CHECK_EXT_SAVE,
  EVSEQ_INIT,
  EVSEQ_INIT_TEST,
  EVSEQ_WAIT_TEST,
  EVSEQ_JUMP_TITLE,
  EVSEQ_END,
};

enum
{
  PROC_LANG_SELECT,         //!< 言語選択
  PROC_TITLE,               //!< タイトル画面
  PROC_TITLE_MENU,          //!< タイトルメニュー
  PROC_INTRO_DEMO_FIRST,    //!< イントロデモ：前半
  PROC_HERO_SELECT,         //!< 容姿・性別選択＋名前入力
  PROC_INTRO_DEMO_SECOND,   //!< イントロデモ：後半
  PROC_INTRO_DEMO_THIRD,    //!< イントロデモ：オープニング
  PROC_NUM,
};


//-----------------------------------------------------------------------------
/**
 *					クラス宣言
 */
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
 */
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/**
 *	@brief  イベント起動
 *
 *	@param  p_geventman ゲームイベントマネージャ
 *	@param  initwork    初期化情報
 */
//-----------------------------------------------------------------------------
void StartEvent::FirstEventStart( GameSys::GameManager* p_gameman )
{
  GameSys::GameEventManager * p_geventman = p_gameman->GetGameEventManager();
  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, StartEvent>( p_geventman );
}

//--------------------------------------------------------------
// コンストラクタ
//--------------------------------------------------------------
StartEvent::StartEvent( gfl2::heap::HeapBase* heap )
  : GameSys::GameEvent( heap )
  , mSeq( 0 )
  , mProcIndex( PROC_LANG_SELECT )
{
#if SKIP_LANG_SELECT
  mProcIndex = PROC_TITLE;
  System::SetLang( POKEMON_LANG_JAPAN, heap );
//  System::SetupLangResource( heap );
#endif
}


//--------------------------------------------------------------
// デストラクタ
//--------------------------------------------------------------
StartEvent::~StartEvent()
{
}


//--------------------------------------------------------------
// イベントを動かしてよいかどうかの判定関数 純粋仮想関数
//--------------------------------------------------------------
bool StartEvent::BootChk( GameSys::GameManager*  )
{
  return true;
}



//--------------------------------------------------------------
// bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
//--------------------------------------------------------------
void StartEvent::InitFunc( GameSys::GameManager*  )
{
  mLangSelectAppParam.defaultLang = System::GetLang();   // @fix NMCat[743] 工場出荷状態ではここでしか言語の設定を行わないのでSystemから取得
  mLangSelectAppParam.saveStatus = App::LangSelect::SAVE_STATUS_FALSE;
  mLangSelectAppParam.exSaveStatus = App::LangSelect::EX_SAVE_STATUS_FALSE;
  mLangSelectAppParam.requestFormatSaveData = false;
}


//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT StartEvent::MainFunc( GameSys::GameManager* game_manager )
{
  GameSys::GameProcManager* proc_manager = game_manager->GetProcManager();
  gfl2::heap::HeapBase* devHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  switch( mSeq ) 
  {
  case EVSEQ_CHECK_SAVE:
    {
      GameSys::GameData * gameData    = game_manager->GetGameData();
      Savedata::SaveLoadControl * p_svld = gameData->GetSaveLoadControl();
      if( p_svld )
      {
        Savedata::SaveLoadControl::ARC_STATUS saveArcStatus = p_svld->CheckArchiveStatus( devHeap );
        switch( saveArcStatus )
        {
        case Savedata::SaveLoadControl::ARC_STATUS_NEED_INIT:
          {
            mLangSelectAppParam.saveStatus = App::LangSelect::SAVE_STATUS_NEED_FORMAT;
            mSeq = EVSEQ_CHECK_EXT_SAVE;
          }
        break;
        case Savedata::SaveLoadControl::ARC_STATUS_INCORRECT:
          {
            mLangSelectAppParam.saveStatus = App::LangSelect::SAVE_STATUS_ERROR;
            mSeq = EVSEQ_CHECK_EXT_SAVE;
          }
        break;
        case Savedata::SaveLoadControl::ARC_STATUS_OK:
          {
            mLangSelectAppParam.saveStatus = App::LangSelect::SAVE_STATUS_TRUE;
            mSeq = EVSEQ_LOAD_START;
          }
        break;
        default:
          mSeq = EVSEQ_CHECK_EXT_SAVE;
          break;
        }
      }
      else
      {
        mProcIndex = PROC_TITLE;
        mSeq = EVSEQ_CHECK_EXT_SAVE;
      }
    }
  break;
  case EVSEQ_LOAD_START:
    {
      GameSys::GameData * gameData    = game_manager->GetGameData();
      Savedata::ISaveDataTotal::State state = gameData->LoadAtOnce( devHeap );

		  switch( state ){
		  case Savedata::ISaveDataTotal::STATE_NULL:	// データなし
        {
          GFL_PRINT( "load empty!\n" );
			    {
				    gfl2::heap::HeapBase * heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
				    gameData->ClearSaveData( heap );
			    }
          mLangSelectAppParam.defaultLang = System::GetLang();
          mLangSelectAppParam.saveStatus = App::LangSelect::SAVE_STATUS_FALSE;
          mSeq = EVSEQ_CHECK_EXT_SAVE;
        }
			  break;

		  case Savedata::ISaveDataTotal::STATE_OK:				// ロード成功
        {
			    GFL_PRINT( "load success!\n" );

          //セーブデータが正常な場合はMyStatusの言語IDを反映させる
          u8 lang_id = gameData->GetPlayerStatus()->GetPokeLanguageId();  // niji_project/prog/GameSys/source/GameData.cppのDeserializeSaveData関数へ移すしてもいいと思う。L=Aはそちらへ移しました。言語設定をここに集約する意味で残しましたが。
          System::SetLang( lang_id, devHeap, gameData->GetConfig()->IsKanjiMode() );
          mLangSelectAppParam.defaultLang = System::GetLang();

          if ( gameData->VerifySecureValue() == true )
          { //巻き戻しなし、継続プレイ可能
            mLangSelectAppParam.saveStatus = App::LangSelect::SAVE_STATUS_TRUE;
          }
          else if ( gameData->VerifyPreiousvSecureValue() == true )
          { //巻き戻りあり、復旧可能
            mLangSelectAppParam.saveStatus = App::LangSelect::SAVE_STATUS_REWIND_WARNING;
          }
          else
          { //巻き戻しているので継続プレイは不可能
            mLangSelectAppParam.saveStatus = App::LangSelect::SAVE_STATUS_REWIND_ERROR;
          }

          mSeq = EVSEQ_CHECK_EXT_SAVE;
        }
        
			  break;
      case Savedata::ISaveDataTotal::STATE_NG:				// ロード失敗
        {
			    GFL_PRINT( "load failed!\n");
			    {
				    gfl2::heap::HeapBase * heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
				    gameData->ClearSaveData( heap );
			    }
          mLangSelectAppParam.defaultLang = System::GetLang();
          mLangSelectAppParam.saveStatus = App::LangSelect::SAVE_STATUS_ERROR;
          mSeq = EVSEQ_CHECK_EXT_SAVE;
        }
			  break;

		  }
    }
    break;
  case EVSEQ_CHECK_EXT_SAVE:
    {
      //拡張セーブデータの状態取得
      ExtSavedata::NijiExtSaveSystem * p_extsvld = game_manager->GetNijiExtSaveSystem();
      ExtSavedata::NijiExtSaveSystem::EXTARC_STATUS exsaveArcStatus;
      exsaveArcStatus = p_extsvld->CheckArchiveStatus( devHeap );
#if   defined(GF_PLATFORM_CTR)
#if PM_DEBUG
      nn::Result res = p_extsvld->GetExtSaveSystem()->GetLastNNResult();
      NN_DBG_PRINT_RESULT( res );
      GFL_PRINT("CheckArchiveStatus : result[0x%x]exsaveArcStatus[%d]\n", res.GetPrintableBits() , exsaveArcStatus );
#endif
#endif
      switch ( exsaveArcStatus )
      {
      case ExtSavedata::NijiExtSaveSystem::EXTARC_STATUS_OK:
        GFL_PRINT("EXTARC_STATUS_OK\n");
        mLangSelectAppParam.exSaveStatus = App::LangSelect::EX_SAVE_STATUS_TRUE;
        p_extsvld->BootProcess( devHeap );

        if( mLangSelectAppParam.saveStatus == App::LangSelect::SAVE_STATUS_TRUE )
        {//拡張セーブ、通常セーブ両方あったら言語選択を飛ばす
          mProcIndex = PROC_TITLE;
        }

        break;
      case ExtSavedata::NijiExtSaveSystem::EXTARC_STATUS_NEED_CREATE:
        GFL_PRINT("EXTARC_STATUS_NEED_CREATE\n");
        mLangSelectAppParam.exSaveStatus = App::LangSelect::EX_SAVE_STATUS_FALSE;
        break;
      case ExtSavedata::NijiExtSaveSystem::EXTARC_STATUS_INCORRECT:
        GFL_PRINT("EXTARC_STATUS_INCORRECT\n");
        mLangSelectAppParam.exSaveStatus = App::LangSelect::EX_SAVE_STATUS_BROKEN_DATA;
        break;
      default:
        GFL_PRINT("EXTARC_STATUS:Others\n");
        mLangSelectAppParam.exSaveStatus = App::LangSelect::EX_SAVE_STATUS_NOT_FOUND;
        break;
      }

      mSeq = EVSEQ_INIT;
    }
    break;
  case EVSEQ_INIT:
#if PM_DEBUG
    {
      const Field::Debug::StartSettingAccessor* pStartSettingAccessor = game_manager->GetGameData()->GetDebugStartSettingAccessor();
      
      // 強制言語選択＆タイトル画面スキップ ボタン検知
      if (pStartSettingAccessor->HasTitleSkipEnable())
      {
        //言語をセット
        if (pStartSettingAccessor->HasLang())
        {
          System::SetLang(pStartSettingAccessor->GetLang(), devHeap, pStartSettingAccessor->IsKanji() );
        }
        else
        {
          System::SetLang(1, devHeap, false);
        }

        switch (pStartSettingAccessor->GetTitleSkipMode())
        {
        case Field::Debug::StartSettingAccessor::TitleSkipMode::START_LAUNCHER:   // タイトルデバッグスタートランチャーから開始
        {
          debug::launcher::LauncherProc::InitCallParam(&m_pLauncherParam);
          debug::launcher::LauncherProc * launcher =
            NIJI_PROC_CALL< GameSys::GameProcManager, debug::launcher::LauncherProc, debug::launcher::LauncherProc::CallParam >(proc_manager, &m_pLauncherParam);
          launcher->CreateStartListData<debug::startmenu::StartLauncher>();
          
          return GameSys::GMEVENT_RES_FINISH;
        }
        case Field::Debug::StartSettingAccessor::TitleSkipMode::MODE_DEBUG_FIRST:      // 通常デバッグでフィールド開始
        {
          Field::EventGameStart::Description desc = {
            Field::EventGameStart::GAMEINIT_MODE_DEBUG_FIRST,
            GFL_SINGLETON_INSTANCE(GameSys::GameManager)
          };
          Field::EventGameStart::ChangeEventGameStart(desc);
          mSeq = EVSEQ_END;
          return GameSys::GMEVENT_RES_FINISH;
        }
        case Field::Debug::StartSettingAccessor::TitleSkipMode::MODE_DEBUG_DEFAULTNA:  // デフォルト名でフィールド開始
        {
          Field::EventGameStart::Description desc = {
            Field::EventGameStart::GAMEINIT_MODE_DEBUG_DEFAULTNAME,
            GFL_SINGLETON_INSTANCE(GameSys::GameManager)
          };
          Field::EventGameStart::ChangeEventGameStart(desc);
          mSeq = EVSEQ_END;
          return GameSys::GMEVENT_RES_FINISH;
        }
        case Field::Debug::StartSettingAccessor::TitleSkipMode::MODE_DEBUG_NETTEST:    // ネットテストでフィールド開始
        {
          Field::EventGameStart::Description desc = {
            Field::EventGameStart::GAMEINIT_MODE_DEBUG_NETTEST,
            GFL_SINGLETON_INSTANCE(GameSys::GameManager)
          };
          Field::EventGameStart::ChangeEventGameStart(desc);
          mSeq = EVSEQ_END;
          return GameSys::GMEVENT_RES_FINISH;
        }
        }
      }
  }
#endif
    {
      //GameSys::CallTitleProc(&mAppParam);
      StartProc();  //!< Procの呼び出し
      mSeq = EVSEQ_INIT_TEST;
    }
    break;
  case EVSEQ_INIT_TEST:
    {
      mSeq = EVSEQ_WAIT_TEST;
    }
    break;

  case EVSEQ_WAIT_TEST:
    // プロセスの終了待ち
    if( game_manager->IsProcessExists() == false ){
      GFL_PRINT("process end\n");
      mSeq = EVSEQ_JUMP_TITLE;
    }
    break;
  case EVSEQ_JUMP_TITLE:
//    if( mAppParam.out_param.ret == app::title::PROC_RET_GAME_SEQ )
//    {
//      Field::EventGameStart::Description desc = {
//        Field::EventGameStart::GAMEINIT_MODE_FIRST,
//        GFL_SINGLETON_INSTANCE(GameSys::GameManager)
//        };
//      Field::EventGameStart::ChangeEventGameStart( desc );
//      mSeq = EVSEQ_END;
//    }
//#if PM_DEBUG
//    else if( mAppParam.out_param.ret == app::title::PROC_RET_DEBUG_SEQ )
//    { // ランチャー
//      debug::launcher::LauncherProc::InitCallParam( &m_pLauncherParam );
//      debug::launcher::LauncherProc * launcher = NIJI_PROC_CALL< GameSys::GameProcManager, debug::launcher::LauncherProc, debug::launcher::LauncherProc::CallParam >( proc_manager, &m_pLauncherParam );
//      launcher->CreateStartListData<debug::startmenu::StartLauncher>();
//      mSeq = EVSEQ_END;
//    }
//#endif  // PM_DEBUG
//    return GameSys::GMEVENT_RES_FINISH;
    return Next( proc_manager );
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
void StartEvent::EndFunc( GameSys::GameManager*  )
{
}

//--------------------------------------------------------------
// Procの開始
//--------------------------------------------------------------
void StartEvent::StartProc( void )
{
  switch( mProcIndex )
  {
  case PROC_LANG_SELECT:    //!< 言語選択
    {
      App::LangSelect::Proc* proc = GameSys::CallLangSelectProc();
      proc->SetAppParam( &mLangSelectAppParam );
    } break;

  case PROC_TITLE:    //!< タイトル
    {
      //MovieはCTRでしか動作しない。
      //タイトル中にデバッグメニューやセーブ消去をやる必要があるので
      //PC版はMovie対応前のProcで代用する
#if defined(GF_PLATFORM_CTR)
      ::movie::MovieProc* proc = GameSys::CallMovieProc();
      m_titleReturnCode = movie::RET_NONE;
      proc->SetReturnCodeBuffer( &m_titleReturnCode );
      proc->SetNo( LanguageToMovieNo() );
#else //GF_PLATFORM_CTR
      GameSys::CallTitleProc(&mTitleAppParam);
#endif
    } break;

  case PROC_TITLE_MENU:   //!< タイトルメニュー
    {
      App::TitleMenu::Proc* proc = GameSys::CallTitleMenuProc();
      proc->SetAppParam( &mTitleMenuAppParam );
    } break;

  case PROC_INTRO_DEMO_FIRST:   //!< イントロデモ前半
    {
      App::AppDemo::AppDemoProcParam param( INTRO_DEMO_FIRST );
      GameSys::CallAppDemoProc( &param );
    } break;

  case PROC_HERO_SELECT:    //!< 容姿性別選択
    {
      App::HeroSelect::Proc * proc = GameSys::CallHeroSelectProc();
      proc->SetAppParam( &m_heroSelectParam );
    } break;

  case PROC_INTRO_DEMO_SECOND:   //!< イントロデモ後半
    {
      App::AppDemo::AppDemoProcParam_Intro2nd param( INTRO_DEMO_SECOND, m_heroSelectParam.select_index );
      GameSys::CallAppDemoProc_Intro2nd( &param );
    } break;

  case PROC_INTRO_DEMO_THIRD:
    {
#if PM_VERSION == VERSION_SUN2
      App::AppDemo::AppDemoProcParam param( INTRO_DEMO_THIRD_S );
#elif PM_VERSION == VERSION_MOON2
      App::AppDemo::AppDemoProcParam param( INTRO_DEMO_THIRD_M );
#endif
      GameSys::CallAppDemoProc( &param );
    } break;
  }
}

//--------------------------------------------------------------
// 次へ
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT StartEvent::Next( GameSys::GameProcManager* procMan )
{
  GameSys::GMEVENT_RESULT ( StartEvent::*NEXT_PROC[PROC_NUM] )( GameSys::GameProcManager* procMan ) = {
    &StartEvent::Next_LangSelect,         //!< 言語選択
    &StartEvent::Next_Title,              //!< タイトル
    &StartEvent::Next_TitleMenu,          //!< タイトルメニュー
    &StartEvent::Next_IntroDemoFirst,     //!< イントロデモ：前半
    &StartEvent::Next_HeroSelect,         //!< 容姿・性別選択＋名前入力
    &StartEvent::Next_IntroDemoSecond,    //!< イントロデモ：後半
    &StartEvent::Next_IntroDemoThird,     //!< イントロデモ：オープニング
  };

  return ( this->*NEXT_PROC[ mProcIndex ] )( procMan );
}

//--------------------------------------------------------------
// 次へ：言語選択
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT StartEvent::Next_LangSelect( GameSys::GameProcManager* procMan )
{
  GameSys::GMEVENT_RESULT result = GameSys::GMEVENT_RES_CONTINUE;

  App::LangSelect::APP_PARAM* prm = &mLangSelectAppParam;
  gfl2::heap::HeapBase* devHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  if( prm->requestFormatSaveData )  //!< セーブデータの消去リクエストあり
  {
    //言語をセット
    //System::SetLang( 1, devHeap, false );   // 誰かがセットするまではコンストラクタで設定された「本体の言語」で進む。ここでは「本体の言語」でいいのでセットする必要はないのでコメントアウト。

    // タイトルメニューの削除確認モードを起動
    mTitleMenuAppParam.initMode = App::TitleMenu::INIT_MODE_SAVEDATA_BROKEN;

    mProcIndex = PROC_TITLE_MENU;
    mSeq = EVSEQ_INIT;
  }
#if PM_DEBUG
  else if( prm->debugLangSelect )
  { // ランチャー
    // 選択した言語をセット
    System::SetLang( prm->selectLang, devHeap, prm->kanjiFlg );

    debug::launcher::LauncherProc::InitCallParam( &m_pLauncherParam );
    debug::launcher::LauncherProc * launcher = NIJI_PROC_CALL< GameSys::GameProcManager, debug::launcher::LauncherProc, debug::launcher::LauncherProc::CallParam >( procMan, &m_pLauncherParam );
    launcher->CreateStartListData<debug::startmenu::StartLauncher>();
    mSeq = EVSEQ_END;
    result = GameSys::GMEVENT_RES_FINISH;
  }
#endif  // PM_DEBUG
  else {
/*
    // 言語選択のシーケンスが追加されたので言語選択画面で設定する 2016/03/04 by nakahiro

    // 選択した言語をセット
    System::SetLang( prm->selectLang, devHeap, prm->kanjiFlg );
   // System::SetupLangResource( devHeap );

    GameSys::GameManager* pGameManager = procMan->GetGameManager();
    GameSys::GameData * gameData    = pGameManager->GetGameData();

    gameData->ClearSaveData(devHeap);   // @note sangoはタイトル後にセーブデータをクルアしていたが、nijiはイントロに遷移するのでここでクリアする
    gameData->GetConfig()->SetKanjiMode(prm->kanjiFlg);
*/

    if( prm->saveStatus != App::LangSelect::SAVE_STATUS_TRUE )
    {
      mProcIndex = PROC_INTRO_DEMO_FIRST;   //!< セーブデータがない場合はタイトルへ飛ばさずイントロを開始する
    }
    else {
      mProcIndex = PROC_TITLE;   //!< セーブデータがある場合はタイトルへ
    }
    mSeq = EVSEQ_INIT;
  }

  return result;
}

//--------------------------------------------------------------
// 次へ：タイトル
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT StartEvent::Next_Title( GameSys::GameProcManager* procMan )
{
  GameSys::GMEVENT_RESULT result = GameSys::GMEVENT_RES_CONTINUE;

#if SKIP_LANG_SELECT
  if( IsTitleReturnCodeDecide()
    || IsTitleReturnCodeDeleteSave() )
  {
    Field::EventGameStart::Description desc = {
      Field::EventGameStart::GAMEINIT_MODE_FIRST,
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)
      };
    Field::EventGameStart::ChangeEventGameStart( desc );
    mSeq = EVSEQ_END;

    return GameSys::GMEVENT_RES_FINISH;
  }
#endif

  if( IsTitleReturnCodeDecide() )
  {
    GameSys::GameManager* pGameManager = procMan->GetGameManager();
    GameSys::GameData * gameData    = pGameManager->GetGameData();

    if( !gameData->IsSaveDataLoaded() )
    {
      // セーブデータがない場合
      mProcIndex = PROC_INTRO_DEMO_FIRST;
      mSeq = EVSEQ_INIT;
    }
    else {
      mTitleMenuAppParam.initMode = App::TitleMenu::INIT_MODE_NONE;

      mProcIndex = PROC_TITLE_MENU;
      mSeq = EVSEQ_INIT;
    }

    return GameSys::GMEVENT_RES_CONTINUE;
  }
  else if( IsTitleReturnCodeDeleteSave() )
  {
    mTitleMenuAppParam.initMode = App::TitleMenu::INIT_MODE_TITLE;

    //  セーブ破棄への遷移
    mProcIndex = PROC_TITLE_MENU;
    mSeq = EVSEQ_INIT;
  }
#if PM_DEBUG
  else if( IsTitleReturnCodeDebugMode() )
  { // ランチャー
    debug::launcher::LauncherProc::InitCallParam( &m_pLauncherParam );
    debug::launcher::LauncherProc * launcher = NIJI_PROC_CALL< GameSys::GameProcManager, debug::launcher::LauncherProc, debug::launcher::LauncherProc::CallParam >( procMan, &m_pLauncherParam );
    launcher->CreateStartListData<debug::startmenu::StartLauncher>();
    mSeq = EVSEQ_END;
    result = GameSys::GMEVENT_RES_FINISH;
  }
#endif  // PM_DEBUG

  return result;
}

//--------------------------------------------------------------
// 次へ：タイトルメニュー
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT StartEvent::Next_TitleMenu( GameSys::GameProcManager* procMan )
{
  GameSys::GMEVENT_RESULT result = GameSys::GMEVENT_RES_CONTINUE;

  // セーブデータの初期化ではない場合
  if( mTitleMenuAppParam.initMode == App::TitleMenu::INIT_MODE_NONE )
  {
    switch( mTitleMenuAppParam.endMode )
    {
    case App::TitleMenu::END_MODE_CANCEL:   //!< キャンセルされた
      {
        mProcIndex = PROC_TITLE;
        mSeq = EVSEQ_INIT;
      } break;
      
    case App::TitleMenu::END_MODE_SELECT_CONTINUE:    //!< つづきからはじめるが選択された
      {
        // セーブ復帰
        Field::EventGameStart::Description desc = {
          Field::EventGameStart::GAMEINIT_MODE_CONTINUE,
          GFL_SINGLETON_INSTANCE(GameSys::GameManager)
        };
        Field::EventGameStart::ChangeEventGameStart( desc );
        mSeq = EVSEQ_END;
        result = GameSys::GMEVENT_RES_FINISH;
      } break;

    case App::TitleMenu::END_MODE_SELECT_PRESENT:     //!< ふしぎなおくりものが選択された
			{
				// 不思議な贈り物
				NetEvent::MysteryGift::MysteryGiftEvent::StartEvent( procMan->GetGameManager() );
				mSeq = EVSEQ_END;
				result = GameSys::GMEVENT_RES_FINISH;
			}
			break;
    case App::TitleMenu::END_MODE_SELECT_LIVE_CUP:    //!< ライブ大会が選択された
		case App::TitleMenu::END_MODE_SELECT_DLC_SHOP:    //!< DLCショップが選択された
			{
				// ライブカップ
				NetEvent::Live::LiveEvent::StartEvent( procMan->GetGameManager() );
				mSeq = EVSEQ_END;
				result = GameSys::GMEVENT_RES_FINISH;
			}
			break;
    }
  }
  // セーブデータの初期化が行われた
  else {
    if( mTitleMenuAppParam.initMode == App::TitleMenu::INIT_MODE_TITLE
      && mTitleMenuAppParam.endMode == App::TitleMenu::END_MODE_INIT_SAVE_CANCEL )
    {
      // タイトルへ
      mProcIndex = PROC_TITLE;
      mSeq = EVSEQ_INIT;
    }
    else {
      // リセット -> ゲーム再開へ
      System::GflUse::SetSoftResetRequest();    //!< ソフトウェアリセット
    }
  }

  return result;
}

//--------------------------------------------------------------
// 次へ：イントロデモ：前半
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT StartEvent::Next_IntroDemoFirst( GameSys::GameProcManager* procMan )
{
  GameSys::GMEVENT_RESULT result = GameSys::GMEVENT_RES_CONTINUE;

  mProcIndex = PROC_HERO_SELECT;
  mSeq = EVSEQ_INIT;

  return result;
}

//--------------------------------------------------------------
// 次へ：容姿・性別選択＋名前入力
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT StartEvent::Next_HeroSelect( GameSys::GameProcManager* procMan )
{
  GameSys::GMEVENT_RESULT result = GameSys::GMEVENT_RES_CONTINUE;

  mProcIndex = PROC_INTRO_DEMO_SECOND;
  mSeq = EVSEQ_INIT;

  return result;
}

//--------------------------------------------------------------
// 次へ：イントロデモ：後半
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT StartEvent::Next_IntroDemoSecond( GameSys::GameProcManager* procMan )
{
  GameSys::GMEVENT_RESULT result = GameSys::GMEVENT_RES_CONTINUE;

  mProcIndex = PROC_INTRO_DEMO_THIRD;
  mSeq = EVSEQ_INIT;

  return result;
}

//--------------------------------------------------------------
// 次へ：イントロデモ：オープニング
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT StartEvent::Next_IntroDemoThird( GameSys::GameProcManager* procMan )
{
#if DEBUG_INTRO
  mProcIndex = PROC_TITLE_MENU;
  mSeq = EVSEQ_INIT;

  return GameSys::GMEVENT_RES_CONTINUE;
#else
  Field::EventGameStart::Description desc = {
  Field::EventGameStart::GAMEINIT_MODE_FIRST,
  GFL_SINGLETON_INSTANCE(GameSys::GameManager)
  };
  Field::EventGameStart::ChangeEventGameStart( desc );
  mSeq = EVSEQ_END;

  return GameSys::GMEVENT_RES_FINISH;
#endif
}

//--------------------------------------------------------------
// タイトルの戻り値：ゲーム開始
//--------------------------------------------------------------
bool StartEvent::IsTitleReturnCodeDecide()
{
#if defined(GF_PLATFORM_CTR)
  return m_titleReturnCode == movie::RET_DECIDE;
#else
  return mTitleAppParam.out_param.ret == app::title::PROC_RET_GAME_SEQ;
#endif
}
//--------------------------------------------------------------
// タイトルの戻り値：セーブ消去
//--------------------------------------------------------------
bool StartEvent::IsTitleReturnCodeDeleteSave()
{
#if defined(GF_PLATFORM_CTR)
  return m_titleReturnCode == movie::RET_UPBX;
#else
  return mTitleAppParam.out_param.ret == app::title::PROC_RET_DELETE_SAVE;
#endif
}
//--------------------------------------------------------------
// タイトルの戻り値：デバッグメニュー
//--------------------------------------------------------------
#if PM_DEBUG
bool StartEvent::IsTitleReturnCodeDebugMode()
{
#if defined(GF_PLATFORM_CTR)
  return m_titleReturnCode == movie::RET_Y;
#else
  return mTitleAppParam.out_param.ret == app::title::PROC_RET_DEBUG_SEQ;
#endif
}
#endif //PM_DEBUG
//--------------------------------------------------------------
// タイトルの戻り値：デバッグメニュー
//--------------------------------------------------------------
int StartEvent::LanguageToMovieNo()
{
  static const struct
  {
    int lang;
    int movieNo;
  } LANGUAGE_TO_MOVIE[] =
  {
    { POKEMON_LANG_JAPAN,   App::Event::MOVIE_TITLE_JAPAN },
    { POKEMON_LANG_ENGLISH, App::Event::MOVIE_TITLE_ENGLISH },
    { POKEMON_LANG_FRANCE,  App::Event::MOVIE_TITLE_FRANCE },
    { POKEMON_LANG_ITALY,   App::Event::MOVIE_TITLE_ITALY },
    { POKEMON_LANG_GERMANY, App::Event::MOVIE_TITLE_GERMANY },
    { POKEMON_LANG_SPAIN,   App::Event::MOVIE_TITLE_SPAIN },
    { POKEMON_LANG_KOREA,   App::Event::MOVIE_TITLE_KOREA },
    { POKEMON_LANG_CHINA,   App::Event::MOVIE_TITLE_CHINA },
    { POKEMON_LANG_TAIWAN,  App::Event::MOVIE_TITLE_TAIWAN },
  };

  for( int i = 0; i < GFL_NELEMS(LANGUAGE_TO_MOVIE); ++i )
  {
    if( LANGUAGE_TO_MOVIE[i].lang == System::GetLang() )
    {
      return LANGUAGE_TO_MOVIE[i].movieNo;
    }
  }

  GFL_ASSERT(0);
  return App::Event::MOVIE_TITLE_JAPAN;
}


GFL_NAMESPACE_END( Main )
