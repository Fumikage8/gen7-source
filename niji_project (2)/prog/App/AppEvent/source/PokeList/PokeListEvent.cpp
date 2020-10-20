//==============================================================================
/**
 * @file    PokeListEvent.cpp
 * @author  fukushima_yuya
 * @date    2015.03.03
 * @brief   ポケモンリスト画面イベント
 */
//==============================================================================

// niji
#include "App/AppEvent/include/PokeList/PokeListEvent.h"

#include <App/FieldBag/include/BagProc.h>
#include <App/PokeList/include/PokeListProc.h>
#include <App/Status/include/StatusProc.h>
#include <App/StrInput/include/StrInputFrame.h>
#include <Debug/StartMenu/include/StartLauncher.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/GameProcManager.h>
#include <System/include/HeapDefine.h>
#include <System/include/PokemonVersion.h>
#include <System/include/PokemonLanguage.h>
#include <GameSys/include/DllProc.h>
#include <App/AppEvent/include/Bag/BagEvent.h>
#include <App/AppEvent/include/Bag/BagGamedata.h>

// gflib2
#include <Fade/include/gfl2_FadeManager.h>

#if PM_DEBUG
#include <Debug/StartMenu/include/d_fukushima.h>
#endif


GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( event )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace {
  enum
  {
    EVSEQ_INIT,
    EVSEQ_FRAME_INIT,
    EVSEQ_WAIT,
    EVSEQ_JUMP_TITLE,
    EVSEQ_END,
  };

  enum {
    PROC_POKELIST,
    PROC_BAG,
    PROC_STATUS,
    PROC_WAZAOSHIE,
    PROC_EVOLUTION,
    PROC_END,
  };
}

u32 PokeListEvent::m_Index = 0;
bool PokeListEvent::m_DebugFlag = false;


//------------------------------------------------------------------------------
/**
 * @brief  イベント起動
 * @param  pGameMan ゲームイベントマネージャ
 */
//------------------------------------------------------------------------------
PokeListEvent * PokeListEvent::StartEvent( GameSys::GameManager* pGameMan, u32 index, bool debugFlag )
{
  m_Index = index;
  m_DebugFlag = debugFlag;

  GameSys::GameEventManager* pGameEventMan = pGameMan->GetGameEventManager();
  return GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, PokeListEvent >( pGameEventMan );
}



//------------------------------------------------------------------------------
/**
 * @brief  コンストラクタ
 * @param   pHeap    workをとるためのメモリクラス
 */
//------------------------------------------------------------------------------
PokeListEvent::PokeListEvent( gfl2::heap::HeapBase* pHeap )
  : GameSys::GameEvent( pHeap )
  , m_Seq( EVSEQ_INIT )
  , m_ProcSeq( PROC_POKELIST )
  , m_isInit( false )
  , m_pPokeListProc( NULL )
  , m_pBagProc( NULL )
  , m_pStatusProc( NULL )
  , m_pAsyncFileManager( NULL )
  , m_pAppRenderingManager( NULL )
  , m_pAppHeap( NULL )
  , m_PokeListAppParam()
  , m_PokeListContext()
  , m_StatusAppParam()
  , m_BagAppParam()
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief  デストラクタ
 */
//------------------------------------------------------------------------------
PokeListEvent::~PokeListEvent( void )
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief  イベントを動かしてよいかどうかの判定関数。純粋仮想関数
 */
//------------------------------------------------------------------------------
bool PokeListEvent::BootChk( GameSys::GameManager* )
{
  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief  bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 */
//------------------------------------------------------------------------------
void PokeListEvent::InitFunc( GameSys::GameManager* )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief  呼び出される関数 純粋仮想関数
 */
//------------------------------------------------------------------------------
GameSys::GMEVENT_RESULT PokeListEvent::MainFunc( GameSys::GameManager* pGameMan )
{
  GameSys::GameProcManager* pProcMan = pGameMan->GetProcManager();

  switch( m_Seq )
  {
  case EVSEQ_INIT:
    {
      // 起動PROC作成
      CreateProc( PROC_POKELIST );
      SetAppParam( pGameMan );

      m_Seq = EVSEQ_FRAME_INIT;
    }
    break;

  case EVSEQ_FRAME_INIT:
    {
      if( !pGameMan->IsProcessExists() )  //!< 万が一、先にプロセスが終了する場合にはセットアップしない
      {
        GFL_ASSERT( 0 );  //!< とはいえ想定外
        m_Seq = EVSEQ_JUMP_TITLE;
      }
      else if( m_pPokeListProc->GetFrameManager() )
      {
        m_Seq = EVSEQ_WAIT;
      }
    }
    break;

  case EVSEQ_WAIT:
    {
      CheckEndProc( pGameMan );
    } break;

  case EVSEQ_JUMP_TITLE:
    {
#if PM_DEBUG
      if( m_DebugFlag )
      { // ランチャー
        debug::startmenu::LauncherFukushima::Delete();

        debug::launcher::LauncherProc::InitCallParam( &m_LauncherParam );
        debug::launcher::LauncherProc* pLauncherProc = NIJI_PROC_CALL< GameSys::GameProcManager, debug::launcher::LauncherProc, debug::launcher::LauncherProc::CallParam >( pProcMan, &m_LauncherParam );
        pLauncherProc->CreateStartListData< debug::startmenu::StartLauncher >();
      }
#endif
      m_Seq = EVSEQ_END;
      return GameSys::GMEVENT_RES_FINISH;
    }
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------------------
/**
 * @brief  呼び出される関数 純粋仮想関数
 */
//------------------------------------------------------------------------------
void PokeListEvent::EndFunc( GameSys::GameManager* )
{
  ;
}



//------------------------------------------------------------------------------
/**
 * @brief   ポケモンリスト外部設定パラメータのセット
 * @param   context   ポケモンリスト外部設定パラメータ
 */
//------------------------------------------------------------------------------
void PokeListEvent::SetPokeListAppParam( App::PokeList::CONTEXT_PARAM * context )
{
  m_PokeListContext = *context;
}




//------------------------------------------------------------------------------
/**
 * @brief   フレームの生成
 * @param   gm    ゲームマネージャ
 */
//------------------------------------------------------------------------------
void PokeListEvent::SetAppParam( GameSys::GameManager* gm )
{
  switch( m_Index )
  {
  case FRAME_ID_POKELIST_FIELD:
  case FRAME_ID_POKELIST_JOIN:
  case FRAME_ID_POKELIST_SIMPLE:
  case FRAME_ID_POKELIST_SIMPLE2:
  case FRAME_ID_POKELIST_SODATEYA:
  case FRAME_ID_POKELIST_WAZAOSHIE:
  case FRAME_ID_POKELIST_FIELD_DEBUG:
    {
      static const App::PokeList::Mode modeList[] = {
        App::PokeList::MODE_FIELD,
        App::PokeList::MODE_JOIN,
        App::PokeList::MODE_SIMPLE,
        App::PokeList::MODE_SIMPLE2,
        App::PokeList::MODE_SODATEYA,
        App::PokeList::MODE_WAZAOSHIE,
        App::PokeList::MODE_FIELD,
      };

      m_PokeListContext.mode = modeList[m_Index];

      GameSys::GameData * gd = gm->GetGameData();
      {
        Savedata::MyItem * myItem = gd->GetMyItem();
        {
          FUKUSHIMA_PRINT("xx %x\n", myItem);
        }

        if( m_PokeListContext.my_party.GetPokeParty() == NULL )
        {
          m_PokeListContext.my_party.SetParameter( gd->GetPlayerParty() );
        }
        m_PokeListContext.my_status = gd->GetPlayerStatus();
      }

#if PM_DEBUG
#if 0
      if( m_PokeListContext.mode == App::PokeList::MODE_SIMPLE )
      {
        m_PokeListContext.mode     = App::PokeList::MODE_BATTLE;
        m_PokeListContext.btl_mode = App::PokeList::BTL_MODE_MULTI_RIGHT;
      }
#endif
#endif

      // フレーム生成
      m_pPokeListProc->SetAppParam( gm, &m_PokeListContext, &m_BagAppParam, &m_StatusAppParam );
    } break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   プロックの生成
 */
//------------------------------------------------------------------------------
void PokeListEvent::CreateProc( u32 procID )
{
  GameSys::GameProcManager * gpm = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager();

  switch( procID )
  {
  case PROC_POKELIST:
    m_pPokeListProc = NIJI_PROC_SETNEXT< GameSys::GameProcManager, App::PokeList::Proc >( gpm );
    break;
  case PROC_BAG:
//    m_pBagProc = NIJI_PROC_SETNEXT< GameSys::GameProcManager, app::bag::BagProc >( gpm );
    m_pBagProc = GameSys::SetNextBagProc();
    m_pBagProc->Setup( &m_BagAppParam );
    break;
  case PROC_STATUS:
    {
      m_pStatusProc = GameSys::CallStatusProc();
      m_pStatusProc->SetAppParam( &m_StatusAppParam );
    }
    break;
  case PROC_WAZAOSHIE:
    {
      App::WazaOshie::Proc * proc = GameSys::CallWazaOshieProc();
      proc->SetAppParam( &m_WazaOshieParam );
    }
    break;
  case PROC_EVOLUTION:
    NetEvent::Evolution::EvolutionEvent::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager), m_EvolutionParam );
    break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   プロックの終了チェック
 */
//------------------------------------------------------------------------------
void PokeListEvent::CheckEndProc( GameSys::GameManager* gm )
{
  // プロセスの終了待ち
  if( gm->IsProcessExists() ) return;

  if( m_ProcSeq == PROC_POKELIST )
  {
    switch( m_PokeListContext.end_mode )
    {
    case App::PokeList::END_MODE_CALL_BAG:
      {
        m_BagAppParam.rcv_mode = app::bag::RCV_NONE;  // 復帰モードをクリア
        CreateProc( PROC_BAG );
        m_ProcSeq = PROC_BAG;
      } break;

    case App::PokeList::END_MODE_CALL_STATUS:
      {
        CreateProc( PROC_STATUS );
        m_ProcSeq = PROC_STATUS;
      } break;

    default:
      {
        m_Seq = EVSEQ_JUMP_TITLE;
      }
    }
  }
  else if( m_ProcSeq == PROC_BAG )
  {
    gm->GetGameData()->GetBagPlayData()->SetPocketPos(m_BagAppParam.pocket_no);   // 終了時のポケット番号
    // 技忘れ画面を呼び出す
    if( m_BagAppParam.end_mode == app::bag::END_CALL_WAZA_WASURE )
    {
      App::Event::BagEvent::SetWazaOhsieParam( &m_BagAppParam, &m_WazaOshieParam );
      CreateProc( PROC_WAZAOSHIE );
      m_ProcSeq = PROC_WAZAOSHIE;
    }
    // 進化画面を呼び出す
    else if( m_BagAppParam.end_mode == app::bag::END_CALL_EVOLUTION )
    {
      App::Event::BagEvent::SetEvolutionDemoParam( gm->GetGameData(), &m_BagAppParam, &m_EvolutionParam );
      CreateProc( PROC_EVOLUTION );
      m_ProcSeq = PROC_EVOLUTION;
    }
    else
    {
      // 選択位置を更新
      m_PokeListContext.poke_index = m_BagAppParam.pokemon_index; // @fix NMCat[77]

      CreateProc( PROC_POKELIST );
      SetAppParam( gm );
      m_ProcSeq = PROC_POKELIST;
    }
  }
  else if( m_ProcSeq == PROC_STATUS )
  {
    m_PokeListContext.poke_index = m_StatusAppParam.poke_index;
    
    CreateProc( PROC_POKELIST );
    SetAppParam( gm );

    m_ProcSeq = PROC_POKELIST;
  }
  else if( m_ProcSeq == PROC_WAZAOSHIE )
  {
    m_BagAppParam.rcv_mode = app::bag::RCV_WAZAOSHIE;
    if( m_WazaOshieParam.end_mode == 0 )
    {
      m_BagAppParam.select_skill_pos = pml::MAX_WAZA_NUM;
    }
    else
    {
      m_BagAppParam.select_skill_pos = m_WazaOshieParam.select_index;  // 選択した技の位置
    }
    CreateProc( PROC_BAG );
    m_ProcSeq = PROC_BAG;
  }
  else if( m_ProcSeq == PROC_EVOLUTION )
  {
    m_BagAppParam.rcv_mode = app::bag::RCV_EVOLUTION;
    CreateProc( PROC_BAG );
    m_ProcSeq = PROC_BAG;
  }
}



GFL_NAMESPACE_END( event )
GFL_NAMESPACE_END( app )
