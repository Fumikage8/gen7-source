//======================================================================
/**
 * @file    FieldPokeListEvent.cpp
 * @date    2015/10/01 18:40:06
 * @author  fukushima_yuya
 * @brief   アプリイベント：フィールドポケリスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/AppEvent/include/FieldPokeList/FieldPokeListEvent.h>

#include <App/PokeList/include/PokeListProc.h>
#include <App/Status/include/StatusProc.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/DllProc.h>
#include <System/include/HeapDefine.h>


GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( event )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace {
  enum
  {
    // ポケモンリスト
    EVSEQ_INIT,
    EVSEQ_FRAME_INIT,
    EVSEQ_WAIT,
    // ステータス
    EVSEQ_STATUS_INIT,
    EVSEQ_STATUS_FRAME_INIT,
    EVSEQ_STATUS_WAIT,

    EVSEQ_JUMP_TITLE,
    EVSEQ_END,
  };
}

//------------------------------------------------------------------------------
/**
 * @brief  イベント起動
 * @param  pGameMan ゲームイベントマネージャ
 */
//------------------------------------------------------------------------------
FieldPokeListEvent * FieldPokeListEvent::StartEvent( GameSys::GameManager* pGameMan )
{
  GameSys::GameEventManager* pGameEventMan = pGameMan->GetGameEventManager();
  return GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, FieldPokeListEvent >( pGameEventMan );
}



//------------------------------------------------------------------------------
/**
 * @brief  コンストラクタ
 * @param   pHeap    workをとるためのメモリクラス
 */
//------------------------------------------------------------------------------
FieldPokeListEvent::FieldPokeListEvent( gfl2::heap::HeapBase* pHeap )
  : GameSys::GameEvent( pHeap )
  , m_Seq(EVSEQ_INIT)
  , m_pPokeListProc(NULL)
  , m_pContextParam(NULL)
  , m_pStatusProc(NULL)
  , m_StatusAppParam()
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief  イベントを動かしてよいかどうかの判定関数。純粋仮想関数
 */
//------------------------------------------------------------------------------
bool FieldPokeListEvent::BootChk( GameSys::GameManager* )
{
  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief  bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 */
//------------------------------------------------------------------------------
void FieldPokeListEvent::InitFunc( GameSys::GameManager* )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief  呼び出される関数 純粋仮想関数
 */
//------------------------------------------------------------------------------
GameSys::GMEVENT_RESULT FieldPokeListEvent::MainFunc( GameSys::GameManager* pGameMan )
{
  GameSys::GameProcManager* pProcMan = pGameMan->GetProcManager();

  switch( m_Seq )
  {
  case EVSEQ_INIT:
    {
      CreatePokeListProc( pGameMan );

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
      // プロセスの終了待ち
      if( pGameMan->IsProcessExists() ) break;

      if( m_pContextParam->end_mode != App::PokeList::END_MODE_CALL_STATUS )
      {
        m_Seq = EVSEQ_JUMP_TITLE;
      }
      else {
        m_Seq = EVSEQ_STATUS_INIT;
      }
    } break;

  case EVSEQ_JUMP_TITLE:
    {
      m_Seq = EVSEQ_END;
      return GameSys::GMEVENT_RES_FINISH;
    }

  case EVSEQ_STATUS_INIT:
    {
      CreateStatusProc();

      m_Seq = EVSEQ_STATUS_FRAME_INIT;
    } break;

  case EVSEQ_STATUS_FRAME_INIT:
    {
      if( !pGameMan->IsProcessExists() )  //!< 万が一、先にプロセスが終了する場合にはセットアップしない
      {
        GFL_ASSERT( 0 );  //!< とはいえ想定外
        m_Seq = EVSEQ_JUMP_TITLE;
      }
      else if( m_pStatusProc->GetFrameManager() )
      {
        m_Seq = EVSEQ_STATUS_WAIT;
      }
    } break;

  case EVSEQ_STATUS_WAIT:
    {
      // プロセスの終了待ち
      if( pGameMan->IsProcessExists() ) break;

      {
        m_pContextParam->poke_index = m_StatusAppParam.poke_index;
      }

      m_Seq = EVSEQ_INIT;
    } break;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------------------
/**
 * @brief  呼び出される関数 純粋仮想関数
 */
//------------------------------------------------------------------------------
void FieldPokeListEvent::EndFunc( GameSys::GameManager* )
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   App::PokeList::Procの生成
 * @param   gm    GameSys::GameManager
 */
//------------------------------------------------------------------------------
void FieldPokeListEvent::CreatePokeListProc( GameSys::GameManager* gm )
{
  GFL_ASSERT( m_pContextParam );

  GameSys::GameData * gd = gm->GetGameData();

  // 起動PROC作成
  m_pPokeListProc = NIJI_PROC_SETNEXT< GameSys::GameProcManager, App::PokeList::Proc >( gm->GetProcManager() );
  
  // パーティデータが無ければゲームデータから手持ちパーティをセットする
  if( m_pContextParam->my_party.IsInvalid() )
  {
    m_pContextParam->my_party.SetParameter( gd->GetPlayerParty() );
  }
  
  m_pContextParam->my_status = gd->GetPlayerStatus();

  m_pPokeListProc->SetAppParam( gm, m_pContextParam, NULL, &m_StatusAppParam );
}


//------------------------------------------------------------------------------
/**
 * @brief   App::Status::Procの生成
 * @param   gm    GameSys::GameManager
 */
//------------------------------------------------------------------------------
void FieldPokeListEvent::CreateStatusProc( void )
{
  m_pStatusProc = GameSys::CallStatusProc();
  m_pStatusProc->SetAppParam( &m_StatusAppParam );
}


//------------------------------------------------------------------------------
/**
 * @brief   ポケモンリスト外部設定パラメータのセット
 * @param   context   ポケモンリスト外部設定パラメータ
 */
//------------------------------------------------------------------------------
void FieldPokeListEvent::SetPokeListAppParam( App::PokeList::CONTEXT_PARAM * context )
{
  m_pContextParam = context;
}

GFL_NAMESPACE_END( event )
GFL_NAMESPACE_END( app )
