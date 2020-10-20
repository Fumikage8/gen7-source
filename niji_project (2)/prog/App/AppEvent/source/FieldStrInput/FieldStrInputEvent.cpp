//======================================================================
/**
 * @file    FieldStrInputEvent.cpp
 * @date    2015/12/21 15:06:12
 * @author  fukushima_yuya
 * @brief   文字入力イベント：フィールド用
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/AppEvent/include/FieldStrInput/FieldStrInputEvent.h>

#include <App/StrInput/include/StrInputProc.h>
#include <GameSys/include/DllProc.h>
#include <GameSys/include/GameData.h>
#include <System/include/HeapDefine.h>


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
}

//------------------------------------------------------------------------------
/**
 * @brief  イベント起動
 * @param  pGameMan ゲームイベントマネージャ
 */
//------------------------------------------------------------------------------
FieldStrInputEvent * FieldStrInputEvent::StartEvent( GameSys::GameManager* pGameMan )
{
  GameSys::GameEventManager* pGameEventMan = pGameMan->GetGameEventManager();
  return GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, FieldStrInputEvent >( pGameEventMan );
}



//------------------------------------------------------------------------------
/**
 * @brief  コンストラクタ
 * @param   pHeap    workをとるためのメモリクラス
 */
//------------------------------------------------------------------------------
FieldStrInputEvent::FieldStrInputEvent( gfl2::heap::HeapBase* pHeap )
  : GameSys::GameEvent( pHeap )
  , m_Seq( EVSEQ_INIT )
  , m_pProc( NULL )
  , m_pAppParam( NULL )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief  イベントを動かしてよいかどうかの判定関数。純粋仮想関数
 */
//------------------------------------------------------------------------------
bool FieldStrInputEvent::BootChk( GameSys::GameManager* )
{
  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief  bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 */
//------------------------------------------------------------------------------
void FieldStrInputEvent::InitFunc( GameSys::GameManager* )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief  呼び出される関数 純粋仮想関数
 */
//------------------------------------------------------------------------------
GameSys::GMEVENT_RESULT FieldStrInputEvent::MainFunc( GameSys::GameManager* pGameMan )
{
  GameSys::GameProcManager* pProcMan = pGameMan->GetProcManager();

  switch( m_Seq )
  {
  case EVSEQ_INIT:
    {
      CreateProc( pGameMan );

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
      else if( m_pProc->GetFrameManager() )
      {
        m_Seq = EVSEQ_WAIT;
      }
    }
    break;

  case EVSEQ_WAIT:
    {
      // プロセスの終了待ち
      if( pGameMan->IsProcessExists() ) break;

      m_Seq = EVSEQ_JUMP_TITLE;
    } break;

  case EVSEQ_JUMP_TITLE:
    {
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
void FieldStrInputEvent::EndFunc( GameSys::GameManager* )
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   Procの生成
 * @param   gm    GameSys::GameManager
 */
//------------------------------------------------------------------------------
void FieldStrInputEvent::CreateProc( GameSys::GameManager* gm )
{
  GFL_ASSERT( m_pAppParam );

  GameSys::GameData * gd = gm->GetGameData();

  // 起動PROC作成
  m_pProc = GameSys::CallStrInputProc();
  {
    GFL_ASSERT( m_pProc );

    m_pProc->SetAppParam( m_pAppParam );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   外部設定パラメータのセット
 * @param   param   外部設定パラメータ
 */
//------------------------------------------------------------------------------
void FieldStrInputEvent::SetAppParam( App::StrInput::APP_PARAM * param )
{
  m_pAppParam = param;
}

GFL_NAMESPACE_END( event )
GFL_NAMESPACE_END( app )
