//=============================================================================
/**
 * @file    PokeParureEvent.cpp
 * @brief   ポケパルレ画面イベント
 * @author  Hiroyuki Nakamura
 * @date    2015.10.21
 */
//=============================================================================

// module
#include "App/AppEvent/include/PokeParure/PokeParureEvent.h"
#include "App/ParurePokeList/include/ParurePokeListProc.h"
// niji
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/DllProc.h"

#if PM_DEBUG
#include "Debug/StartMenu/include/StartLauncher.h"
#endif


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)

//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap  ヒープ
 */
//-----------------------------------------------------------------------------
PokeParureEvent::PokeParureEvent( gfl2::heap::HeapBase * heap )
  : GameSys::GameEvent( heap )
  , m_pokemonIndex( 0 )
{
  gfl2::std::MemClear( &m_kawaigariProcParam, sizeof(App::Kawaigari::KawaigariProcParam) );
  gfl2::std::MemClear( &m_pokeListParam, sizeof(App::PokeParure::APP_PARAM) );

#if PM_DEBUG
  SetupLauncherCall( false ); // デバッグフラグ初期化
#endif  // PM_DEBUG
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
PokeParureEvent::~PokeParureEvent()
{
}


//-----------------------------------------------------------------------------
/**
 * @func    StartEvent
 * @brief   イベント起動
 * @date    2015.10.14
 *
 * @param   man     ゲームマネージャクラス
 *
 * @return  PokeParureEventクラス
 */
//-----------------------------------------------------------------------------
PokeParureEvent * PokeParureEvent::StartEvent( GameSys::GameManager * gmgr )
{
  GameSys::GameEventManager * ev_man = gmgr->GetGameEventManager();
  return GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, PokeParureEvent >( ev_man );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetupParam
 * @brief   パラメータ設定
 * @date    2015.12.08
 *
 * @param   prm     メイン画面の外部設定パラメータ
 * @param   index   ポケモンの並び位置
 *
 * @note  入れ替えがない場合はindex = pml::PokeParty::MAX_MEMBERS
 */
//-----------------------------------------------------------------------------
void PokeParureEvent::SetupParam( const App::Kawaigari::KawaigariProcParam * prm, u32 index )
{
  m_kawaigariProcParam = *prm;
  m_pokemonIndex       = index;

  // 瀕死・タマゴチェック
  pml::PokeParty * party = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();
  pml::pokepara::PokemonParam * pp = party->GetMemberPointer( m_pokemonIndex );
  if( pp->GetHp() == 0 || pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) != false )
  {
    for( u32 i=0; i<party->GetMemberCount(); i++ )
    {
      if( i == m_pokemonIndex )
      {
        continue;
      }
      pp = party->GetMemberPointer( i );
      if( pp->GetHp() != 0 && pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) == false )
      {
        m_pokemonIndex = i;
        m_kawaigariProcParam.pokemonParam = pp;
        break;
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @date    2015.10.14
 *
 * @param   gmgr  ゲームマネージャークラス
 *
 * @retval  true  = 起動してよい
 * @retval  false = 起動しない
 */
//-----------------------------------------------------------------------------
bool PokeParureEvent::BootChk( GameSys::GameManager * /*gmgr*/ )
{
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @date    2015.10.14
 *
 * @param   gmgr  ゲームマネージャークラス
 */
//-----------------------------------------------------------------------------
void PokeParureEvent::InitFunc( GameSys::GameManager * /*gmgr*/ )
{
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   呼び出される関数 純粋仮想関数
 * @date    2015.10.14
 *
 * @param   gmgr  ゲームマネージャークラス
 *
 * @return  イベント制御関数の戻り値
 */
//-----------------------------------------------------------------------------
GameSys::GMEVENT_RESULT PokeParureEvent::MainFunc( GameSys::GameManager * gmgr )
{
  enum
  {
    EVSEQ_PROC_CREATE = 0,
    EVSEQ_PROC_WAIT,

    EVSEQ_POKELIST_PROC_CREATE,
    EVSEQ_POKELIST_PROC_WAIT,

    EVSEQ_PROC_END,
  };

  switch( GetSeqNo() )
  {
  case EVSEQ_PROC_CREATE:
    if( m_kawaigariProcParam.pokemonParam->IsEgg(pml::pokepara::CHECK_BOTH_EGG) != false )
    {
      GFL_ASSERT( 0 );
      SetSeqNo( EVSEQ_PROC_END );
    }
    else
    {
      App::Kawaigari::KawaigariProc * proc = GameSys::CallKawaigariProc( &m_kawaigariProcParam );
      SetSeqNo( EVSEQ_PROC_WAIT );
    }
    break;

  case EVSEQ_PROC_WAIT:
    // プロセスの終了待ち
    if( !gmgr->IsProcessExists() )
    {
      if( m_kawaigariProcParam.exitCode == App::Kawaigari::KAWAIGARI_EXITCODE_NORMAL )
      {
        SetSeqNo( EVSEQ_PROC_END );
      }
      else if( m_kawaigariProcParam.exitCode == App::Kawaigari::KAWAIGARI_EXITCODE_POKEMONSELECT )
      {
        SetSeqNo( EVSEQ_POKELIST_PROC_CREATE );
      }
      else
      {
        GFL_ASSERT( 0 );
        SetSeqNo( EVSEQ_PROC_END );
      }
    }
    break;

  case EVSEQ_POKELIST_PROC_CREATE:
    {
      if( m_pokemonIndex == pml::PokeParty::MAX_MEMBERS )
      {
        GFL_ASSERT( 0 );
        m_pokemonIndex = 0;
      }
      App::PokeParure::ParurePokeListProc * proc = GameSys::SetNextParurePokeListProc();
      m_pokeListParam.select_pos = m_pokemonIndex;
      proc->Setup( &m_pokeListParam );
    }
    SetSeqNo( EVSEQ_POKELIST_PROC_WAIT );
    break;

  case EVSEQ_POKELIST_PROC_WAIT:
    // プロセスの終了待ち
    if( !gmgr->IsProcessExists() )
    {
      pml::PokeParty * party = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();
      if( m_pokeListParam.select_pos != pml::PokeParty::MAX_MEMBERS )
      {
        m_pokemonIndex = m_pokeListParam.select_pos;
        m_kawaigariProcParam.pokemonParam = party->GetMemberPointer( m_pokemonIndex );
        m_kawaigariProcParam.mode = App::Kawaigari::KAWAIGARI_MODE_NORMAL;
      }
      else
      {
        m_kawaigariProcParam.mode = App::Kawaigari::KAWAIGARI_MODE_FORCE_IDLE;
      }
      SetSeqNo( EVSEQ_PROC_CREATE );
    }
    break;

  case EVSEQ_PROC_END:
    if( m_pokemonIndex != pml::PokeParty::MAX_MEMBERS )
    {
      pml::PokeParty * party = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();
      party->SetMarkingIndex( m_pokemonIndex );
    }
#if PM_DEBUG
    if( IsLauncherCall() != false )
    {
      debug::launcher::LauncherProc::CallParam param;
      debug::launcher::LauncherProc::InitCallParam( &param );
      debug::launcher::LauncherProc * proc = NIJI_PROC_CALL< GameSys::GameProcManager, debug::launcher::LauncherProc, debug::launcher::LauncherProc::CallParam >( gmgr->GetProcManager(), &param );
      proc->CreateStartListData< debug::startmenu::StartLauncher >();
    }
#endif  // PM_DEBUG
    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   呼び出される関数 純粋仮想関数
 * @date    2015.10.14
 *
 * @param   gmgr  ゲームマネージャークラス
 */
//-----------------------------------------------------------------------------
void PokeParureEvent::EndFunc( GameSys::GameManager * gmgr )
{
}


//-----------------------------------------------------------------------------
/**
 * @func    IsNoHungryPokemon
 * @brief   満腹度を無視するポケモンかをチェック
 * @date    2016.05.23
 *
 * @param   mons	ポケモン番号
 *
 * @retval  true  = 満腹度を無視する
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool PokeParureEvent::IsNoHungryPokemon( MonsNo mons )
{
  static const MonsNo tbl[] =
  {
    MONSNO_TORANSERU,   // トランセル
    MONSNO_KOKUUN,      // コクーン
    MONSNO_KUNUGIDAMA,  // クヌギダマ
    MONSNO_KARASARISU,  // カラサリス
    MONSNO_MAYURUDO,    // マユルド
    MONSNO_NUKENIN,     // ヌケニン
    MONSNO_KOHUURAI,    // コフーライ
    MONSNO_MAYU,        // コスモウム ※新ポケ
  };

  for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
  {
    if( mons == tbl[i] )
    {
      return true;
    }
  }

  return false;
}




//=============================================================================
//  デバッグ用
//=============================================================================
#if PM_DEBUG
bool DebugPokeParureLauncherCall = false;

void PokeParureEvent::SetupLauncherCall( bool flg )
{
  DebugPokeParureLauncherCall = flg;
}

bool PokeParureEvent::IsLauncherCall(void)
{
  return DebugPokeParureLauncherCall;
}
#endif  // PM_DEBUG


GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)
