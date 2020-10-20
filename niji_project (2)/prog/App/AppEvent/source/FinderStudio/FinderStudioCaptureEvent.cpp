//======================================================================
/**
 * @file    FinderStudioCaptureEvent.cpp
 * @date    2016/11/09 16:22:05
 * @author  fukushima_yuya
 * @brief   イベント：ファインダースタジオ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/AppEvent/include/FinderStudio/FinderStudioCaptureEvent.h>

#include <App/FinderStudioCapture/include/FinderStudioCaptureProc.h>
#include <App/Kisekae/include/KisekaeProc.h>

#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include <GameSys/include/GameData.h>
#include <GameSys/include/DllProc.h>
#include <NetStatic/NetEvent/include/FatalErrorEvent.h>
#include <System/include/HeapDefine.h>
#include <Sound/include/Sound.h>

#if PM_DEBUG
#include <Debug/StartMenu/include/StartLauncher.h>
#include <Debug/StartMenu/include/d_fukushima.h>
#endif


GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( event )

//////////////////////////////////////////////////////////////////////////
/// ---[ グローバル変数定義 begin ]---
/// メンバーインデックス
u8 FinderStudioCaptureEvent::m_MemberIndex = 0;
#if PM_DEBUG
/// デバッグ起動フラグ
bool FinderStudioCaptureEvent::m_DebugFlag = false;
#endif
/// ---[ グローバル変数定義 end ]---
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///
/// @brief    イベント起動
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   イベント起動
 *
 * @param   gameMan       ゲームマネージャ
 */
FinderStudioCaptureEvent* FinderStudioCaptureEvent::StartEvent( GameSys::GameManager* gameMan )
{
  m_MemberIndex = _GetEnableMemberIndex( gameMan );

  GameSys::GameEventManager* pGameEventMan = gameMan->GetGameEventManager();
  return GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, FinderStudioCaptureEvent >( pGameEventMan );
}

#if PM_DEBUG
FinderStudioCaptureEvent* FinderStudioCaptureEvent::StartEvent( GameSys::GameManager* gameMan, bool debug/* = false*/ )
{
  m_DebugFlag = debug;

  return StartEvent( gameMan );
}
#endif

/**
 * @brief BGM再生設定
 */
void FinderStudioCaptureEvent::SetPlayBGM(void)
{
  m_isPlayBGM = true;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンストラクタ・デストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 *
 * @param   heap    workをとるためのメモリクラス
 */
FinderStudioCaptureEvent::FinderStudioCaptureEvent( gfl2::heap::HeapBase* heap )
  : GameSys::GameEvent(heap)
  , m_pFinderStudioCaptureProc(NULL)
  , m_pKisekaeProc(NULL)
  , m_Seq(0)
  , m_ProcType( PROC_TYPE_FINDER_STUDIO )
  , m_isPlayBGM( false )
{
  ;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    継承関数群
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 *
 * @return  "true  = 起動してよい"
 * @return  "false = 起動しない"
 */
bool FinderStudioCaptureEvent::BootChk( GameSys::GameManager* )
{
  return true;
}

/**
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 */
void FinderStudioCaptureEvent::InitFunc( GameSys::GameManager* gameMan )
{
  // 共通データの初期化
  {
    GameSys::GameData* gd = gameMan->GetGameData();

    // ポケモンを取得
    pml::pokepara::PokemonParam* pp = gd->GetPlayerParty()->GetMemberPointer( m_MemberIndex );
    GFL_ASSERT( pp != NULL );
    // メンバーインデックスをセット
    m_FinderStudioWork.prepare.member_id = m_MemberIndex;
    // ポケモンパラメータをセット：pml::pokepara::PokemonParam
    m_FinderStudioWork.prepare.pokemon_param = pp;
    // ポケモンパラメータをセット：PokeTool::SimpleParam
    PokeTool::GetSimpleParam( &m_FinderStudioWork.viewer.simple_param, pp );
    m_FinderStudioWork.viewer.dressup_param = gd->GetPlayerStatus()->GetDressup();
    // 進化専用モードをセット
    bool bEvolutionMode = ( gd->GetFieldMenu()->GetRotomRequest() == Savedata::FieldMenu::ROTOM_REQ_EVOLUTION );
    m_FinderStudioWork.app.evolution_mode = bEvolutionMode;
  }

  // 起動PROC作成
  CallProc_FinderStudioCapture();
  m_Seq = EVSEQ_CAPTURE;

  // BGM再生
  if( m_isPlayBGM != false )
  {
    Sound::StartAppBGMReq( STRM_BGM_MJ_S01 );
  }
}

/**
 * @brief   呼び出される関数 純粋仮想関数
 *
 * @return  GMEVENT_RESULT  イベント制御関数の戻り値
 */
GameSys::GMEVENT_RESULT FinderStudioCaptureEvent::MainFunc( GameSys::GameManager* gameMan )
{
  GameSys::GameProcManager* procMan = gameMan->GetProcManager();

  switch( m_Seq )
  {
    /// ファインダースタジオ撮影アプリ
    case EVSEQ_CAPTURE:
    {
      if( procMan->IsRegisterProcess( m_pFinderStudioCaptureProc ) == false )
      {
        // フェイタルエラーかどうか
        if( !m_FinderStudioWork.app.fatal_error )
        {
          // Proc切り替え
          ChangeProc( gameMan );
        }
        else {
          // フェイタルエラーイベント開始
          StartEvent_FatalError( gameMan );
        }
      }
    } break;

    /// 着せ替えアプリ
    case EVSEQ_KISEKAE:
    {
      if( procMan->IsRegisterProcess( m_pKisekaeProc ) == false )
      {
        // Proc切り替え
        ChangeProc( gameMan );
      }
    } break;

    case EVSEQ_JUMP_TITLE:
    {
#if PM_DEBUG
      if( m_DebugFlag )
      { // ランチャー
        debug::startmenu::LauncherFukushima::Delete();

        debug::launcher::LauncherProc::InitCallParam( &m_LauncherParam );
        debug::launcher::LauncherProc* pLauncherProc = NIJI_PROC_CALL< GameSys::GameProcManager, debug::launcher::LauncherProc, debug::launcher::LauncherProc::CallParam >( procMan, &m_LauncherParam );
        pLauncherProc->CreateStartListData< debug::startmenu::StartLauncher >();
      }
#endif
      m_Seq = EVSEQ_END;
      return GameSys::GMEVENT_RES_FINISH;
    } // no break;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

/**
 * @brief   呼び出される関数 純粋仮想関数
 */
void FinderStudioCaptureEvent::EndFunc( GameSys::GameManager* )
{
  // BGM停止
  if( m_isPlayBGM != false )
  {
    Sound::EndAppBGMReq();
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    プロック起動
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   撮影アプリプロックの起動
 */
void FinderStudioCaptureEvent::CallProc_FinderStudioCapture( void )
{
  m_pFinderStudioCaptureProc = GameSys::CallFinderStudioCaptureProc();
  m_pFinderStudioCaptureProc->SetWork( &m_FinderStudioWork );
}

/**
 * @brief   着せ替えプロックの起動
 */
void FinderStudioCaptureEvent::CallProc_Kisekae( GameSys::GameManager* gameMan )
{
  {
    m_KisekaeAppParam.runType             = App::Kisekae::EVENT_RUN_TYPE_FINDER_STUDIO;
    m_KisekaeAppParam.inputDressUpParam   = m_FinderStudioWork.app.dressup_save_param;
    m_KisekaeAppParam.output.dressUpParam = m_FinderStudioWork.app.dressup_save_param;    //!< 着せ替えアプリでキャンセルすると中身が空のままだったのでここで代入をしておく。
  }

  //  proc生成
  m_pKisekaeProc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, App::Kisekae::Proc, App::Kisekae::APP_PARAM>(
    gameMan->GetProcManager(), &m_KisekaeAppParam, gameMan->GetAsyncFileManager(), "Kisekae.cro" );

  //  きせかえデータ参照に必要
  {
    gfl2::ro::RoManager*  roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
    gfl2::ro::Module*     pKisekaeDataModule = roMgr->LoadModule( gameMan->GetAsyncFileManager(), "KisekaeData.cro" );

    roMgr->StartModule( pKisekaeDataModule, true );
    m_pKisekaeProc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DLL_LOAD ), 1 );
    m_pKisekaeProc->SetChildModulePtr( 0, pKisekaeDataModule );
  }
}

/**
 * @brief   プロックの切り替え
 */
void FinderStudioCaptureEvent::ChangeProc( GameSys::GameManager* gameMan )
{
  if( m_ProcType == PROC_TYPE_FINDER_STUDIO )
  {
    if( m_FinderStudioWork.app.call_kisekae )
    {
      CallProc_Kisekae( gameMan );
      GFL_PRINT( "Call Kisekae Proc\n" );
      m_ProcType = PROC_TYPE_KISEKAE;
      m_Seq = EVSEQ_KISEKAE;
    }
    else {
      m_Seq = EVSEQ_JUMP_TITLE;
      GFL_PRINT( "Finish FinderStudio Event\n" );
    }
  }
  else if( m_ProcType == PROC_TYPE_KISEKAE )
  {
    // 着せ替えデータの反映
    _UpdateDressUpParam( gameMan );

    CallProc_FinderStudioCapture();
    GFL_PRINT( "Call FinderStudio Proc\n" );
    m_ProcType = PROC_TYPE_FINDER_STUDIO;
    m_Seq = EVSEQ_CAPTURE;
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    イベント起動
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   フェイタルエラーイベントの起動
 */
void FinderStudioCaptureEvent::StartEvent_FatalError( GameSys::GameManager* gameMan )
{
  NetEvent::FatalError::EventParam eventParam = { NetEvent::FatalError::FATAL_ERROR_NO_SD_CARD_NOT_RECOGNIZED };
  NetEvent::FatalError::FatalErrorEvent::StartEvent( gameMan, eventParam );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    着せ替えデータの反映
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   着せ替えデータの反映
 */
void FinderStudioCaptureEvent::_UpdateDressUpParam( GameSys::GameManager* gameMan )
{
  GameSys::GameData* gameData = gameMan->GetGameData();
  Savedata::FinderStudioSave* finderStudioSave = gameData->GetFinderStudioSave();

  poke_3d::model::DressUpParam dressupParam = m_KisekaeAppParam.output.dressUpParam;

  // 共通データを更新
  m_FinderStudioWork.viewer.dressup_param = dressupParam;

  u8 index = m_FinderStudioWork.app.dressup_save_index;
  // 保存領域のデータを更新
  finderStudioSave->SetDressupParam( index, dressupParam );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    メンバーインデックスを取得
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   表示が可能なメンバーインデックスを設定
 */
u8 FinderStudioCaptureEvent::_GetEnableMemberIndex( GameSys::GameManager* gameMan )
{
  // パーティ情報を取得
  GameSys::GameData* gd = gameMan->GetGameData();
  pml::PokeParty* pokeParty = gd->GetPlayerParty();

  u8 index = pokeParty->GetMemberCount();

  for( u8 i = 0; i < pokeParty->GetMemberCount(); ++i )
  {
    const pml::pokepara::PokemonParam* pp = pokeParty->GetMemberPointerConst( i );

    // タマゴかどうか
    if( _IsPokemonEgg( pp ) ) continue;
    // 瀕死かどうか
    if( _IsPokemonHinshi( pp ) ) continue;

    // タマゴでも瀕死でもなければOK
    index = i;
    break;
  }

  FUKUSHIMA_PRINT( "memberIndex=[%d]\n", index );
  GFL_ASSERT( index < pokeParty->GetMemberCount() );

  return index;
}

/**
 * @brief   ポケモンがタマゴかどうか
 */
bool FinderStudioCaptureEvent::_IsPokemonEgg( const pml::pokepara::PokemonParam* pp )
{
  bool bEgg = false;

  bool fastmode = pp->StartFastMode();
  {
    // タマゴかどうか
    bEgg = pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG );
  }
  pp->EndFastMode( fastmode );

  return bEgg;
}

/**
 * @brief   ポケモンが瀕死かどうか
 */
bool FinderStudioCaptureEvent::_IsPokemonHinshi( const pml::pokepara::PokemonParam* pp )
{
  bool bHinshi = false;

  bool fastmode = pp->StartFastMode();
  {
    // 瀕死かどうか
    bHinshi = ( pp->GetHp() == 0 );
  }
  pp->EndFastMode( fastmode );

  return bHinshi;
}

/**
 * @brief   指定インデックスのポケモンが有効かをチェック
 */
void FinderStudioCaptureEvent::_CheckPokemon( GameSys::GameManager* gameMan, u8 memberIndex )
{
  // パーティ情報を取得
  GameSys::GameData* gd = gameMan->GetGameData();
  pml::PokeParty* pokeParty = gd->GetPlayerParty();

  const pml::pokepara::PokemonParam* pp = pokeParty->GetMemberPointerConst( memberIndex );

  // タマゴかどうか
  GFL_ASSERT( !_IsPokemonEgg( pp ) );
  // 瀕死かどうか
  GFL_ASSERT( !_IsPokemonHinshi( pp ) );
}


GFL_NAMESPACE_END( event )
GFL_NAMESPACE_END( app )
