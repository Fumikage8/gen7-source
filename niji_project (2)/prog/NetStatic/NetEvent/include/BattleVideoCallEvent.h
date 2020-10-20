// ============================================================================
/*
 * @file BattleVideoCallEvent.h
 * @brief バトルビデオアプリ起動イベント
 * @date 2015.12.28
 * @author yuto_uchida
 */
// ============================================================================
#if !defined( BATTLE_VIDEO_CALL_EVENT_H_INCLUDE )
#define BATTLE_VIDEO_CALL_EVENT_H_INCLUDE
#pragma once

//  niji
#include <macro/include/gfl2_Macros.h>
#include <GameSys/include/GameProc.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEvent.h"

#include "Battle/include/battle_SetupParam.h"

GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(BattleVideoCall)

  //  イベント起動のパラメータ
  struct EVENT_PARAM
  {
    struct __tag_in__
    {
      BATTLE_SETUP_PARAM                            btlSetupParam;
      u32                                           bgmDefaultId;
    } in;

    struct __tag_out__
    {
      __tag_out__()
      {
        Clear();
      }

      void Clear()
      {
        bCompPlay = false;
        bEventEnd = false;
      }

      bool  bCompPlay;
      bool  bEventEnd;

    } out;
  };

  //----------------------------------------------------------------------
  /**
   *  @brief  バトルビデオ起動アプリ起動クラス
   */
  //----------------------------------------------------------------------
  class Event : public GameSys::GameEvent
  {
    GFL_FORBID_COPY_AND_ASSIGN( Event );
  public:
    //  イベント開始
    static void StartEvent(GameSys::GameManager* gmgr, EVENT_PARAM* pEventParam );

    // コンストラクタ
    Event( gfl2::heap::HeapBase* pHeap );

    // デストラクタ
    virtual~Event();

    virtual bool BootChk( GameSys::GameManager* pGameManager ) { return true; }
    virtual void InitFunc( GameSys::GameManager* pGameManager );
    virtual void EndFunc( GameSys::GameManager* pGameManager );
    virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

  private:
    void  SetParam( EVENT_PARAM* pEventParam )
    {
      GFL_ASSERT( pEventParam );
      m_pParam  = pEventParam;
    }

    void  _Clear()
    {
      m_pProc             = NULL;
      m_pParam            = NULL;
    }

    GameSys::GameProc*                            m_pProc;
    EVENT_PARAM*                                  m_pParam;

  };

GFL_NAMESPACE_END(BattleVideoPlayer)
GFL_NAMESPACE_END(NetEvent)

#endif // BATTLE_VIDEO_CALL_EVENT_H_INCLUDE
