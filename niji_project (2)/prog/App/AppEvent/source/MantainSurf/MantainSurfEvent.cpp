//======================================================================
/**
 * @file MantainSurfEvent.cpp
 * @date 2016/11/22 19:20:07
 * @author saito_nozomu
 * @brief マンタインサーフアプリ呼び出し
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "App/AppEvent/include/MantainSurf/MantainSurfEvent.h"

#include  <Sound/include/Sound.h>
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/DllProc.h"
#include "App/MantainSurf/include/MantainSurfProc.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)

//-----------------------------------------------------------------------------
/**
* @brief   コンストラクタ
*
* @param   heap  ヒープ
*/
//-----------------------------------------------------------------------------
MantainSurfEvent::MantainSurfEvent(gfl2::heap::HeapBase * heap)
  : GameSys::GameEvent(heap)
{
  m_pParam = NULL;
  m_BgmFadeCounter = 0;
}

//-----------------------------------------------------------------------------
/**
* @brief   デストラクタ
*/
//-----------------------------------------------------------------------------
MantainSurfEvent::~MantainSurfEvent()
{
}


//-----------------------------------------------------------------------------
/**
* @func    StartEvent
* @brief   イベント起動
* @date    2015.06.11
*
* @param   man     ゲームマネージャクラス
*
* @return  MantainSurfEventクラス
*/
//-----------------------------------------------------------------------------
MantainSurfEvent * MantainSurfEvent::StartEvent(GameSys::GameManager * gmgr)
{
  GameSys::GameEventManager * ev_man = gmgr->GetGameEventManager();
  return GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, MantainSurfEvent >(ev_man);
}

//-----------------------------------------------------------------------------
/**
* @func    SetupAppParam
* @brief   パラメータ設定
* @date    2015.12.09
*
* @param   prm   外部設定パラメータ
*/
//-----------------------------------------------------------------------------
void MantainSurfEvent::SetupAppParam(MANTAIN_SURF_PARAM * prm)
{
  m_pParam = prm;
}

//-----------------------------------------------------------------------------
/**
* @func    BookChk
* @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
* @date    2015.06.11
*
* @param   gmgr  ゲームマネージャークラス
*
* @retval  true  = 起動してよい
* @retval  false = 起動しない
*/
//-----------------------------------------------------------------------------
bool MantainSurfEvent::BootChk(GameSys::GameManager * /*gmgr*/)
{
  return true;
}

//-----------------------------------------------------------------------------
/**
* @func    BookChk
* @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
* @date    2015.06.11
*
* @param   gmgr  ゲームマネージャークラス
*/
//-----------------------------------------------------------------------------
void MantainSurfEvent::InitFunc(GameSys::GameManager * /*gmgr*/)
{
}

//-----------------------------------------------------------------------------
/**
* @func    BookChk
* @brief   呼び出される関数 純粋仮想関数
* @date    2015.06.11
*
* @param   gmgr  ゲームマネージャークラス
*
* @return  イベント制御関数の戻り値
*/
//-----------------------------------------------------------------------------
GameSys::GMEVENT_RESULT MantainSurfEvent::MainFunc(GameSys::GameManager * gmgr)
{
  enum
  {
    EVSEQ_BGM_FADEOUT = 0,
    EVSEQ_PROC_CREATE,
    EVSEQ_PROC_WAIT,
    EVSEQ_PROC_END,
  };

  switch (GetSeqNo()){
  case EVSEQ_BGM_FADEOUT:
    if (Sound::IsBGMPlaying() && (!Sound::IsBGMFade()))
    {
      Sound::PushBGMReq();
      SetSeqNo(EVSEQ_PROC_CREATE);
    }
    else
    {
      //不測の事態に備えて、タイムアウトを設ける
      m_BgmFadeCounter++;
      if (m_BgmFadeCounter > 30 * 5) //5秒待っても処理が終了しないときはBGMのリクエストを開始する
      {
        GFL_ASSERT(0);
        Sound::PushBGMReq();
        SetSeqNo(EVSEQ_PROC_CREATE);
      }
    }
    break;
    case EVSEQ_PROC_CREATE:
  {
    App::MantainSurf::MantainSurfProc * proc = GameSys::CallMantainSurfProc();
    proc->Setup(m_pParam);

    SetSeqNo(EVSEQ_PROC_WAIT);
    break;
  }

  case EVSEQ_PROC_WAIT:
    // プロセスの終了待ち
    if (!gmgr->IsProcessExists())
    {
      Sound::PopBGMOnlyReq();
      SetSeqNo(EVSEQ_PROC_END);
    }
    break;
  case EVSEQ_PROC_END:
    Sound::StartBGMReq(STRM_BGM_SILENCE);//<<飛び先と飛び元でＢＧＭが同一の時にＢＧＭ復帰がされないので、無音ＢＧＭをコールし、差が出るようにしておく
    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
* @func    BookChk
* @brief   呼び出される関数 純粋仮想関数
* @date    2015.06.11
*
* @param   gmgr  ゲームマネージャークラス
*/
//-----------------------------------------------------------------------------
void MantainSurfEvent::EndFunc(GameSys::GameManager * gmgr)
{
}

GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)
