// ============================================================================
/*
 * @file BattleVideoPlayerEvent.h
 * @brief バトルビデオ再生アプリ起動イベント
 * @date 2015.09.24
 * @author yuto_uchida
 */
// ============================================================================
#if !defined( BATTLE_VIDEO_PLAYER_EVENT_H_INCLUDE )
#define BATTLE_VIDEO_PLAYER_EVENT_H_INCLUDE
#pragma once

//  niji
#include <macro/include/gfl2_Macros.h>
#include <GameSys/include/GameProc.h>
#include "GameSys/include/GameManager.h"

#include <Debug/Launcher/include/LauncherProc.h>

#include "NetStatic/NetEvent/include/NetEventBase.h" // for NetEventBase

#include "NetApp/BattleVideoPlayer/include/BattleVideoPlayerProcAppParam.h"

//  チーム選択アプリイベントパラメータ
#include "NetApp/TeamSelect/include/TeamSelectEventAppParam.h"

//  イベントのインクルード
#include "NetStatic/NetEvent/include/BattleVideoCallEvent.h"
#include "NetStatic/NetEvent/include/FatalErrorEvent.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(PokeTool)
  class BattleParty;
GFL_NAMESPACE_END(PokeTool)

GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(BattleVideoPlayer)

  //----------------------------------------------------------------------
  /**
   *  @brief  バトルビデオ再生アプリ起動クラス
   */
  //----------------------------------------------------------------------
  class Event : public NetEvent::NetEventBase
  {
    GFL_FORBID_COPY_AND_ASSIGN( Event );
  public:
    //  ローカルかネットか
    enum PlayerDataType
    {
      PLAYAER_DATA_TYPE_LOCAL  = 0,
      PLAYAER_DATA_TYPE_INTERNET
    };

    //  イベント開始
    static void StartEvent(GameSys::GameManager* gmgr, const PlayerDataType dataType, const bool bDebugFlg = false );

    // コンストラクタ
    Event( gfl2::heap::HeapBase* pHeap );

    // デストラクタ
    virtual~Event();

    virtual bool BootChk( GameSys::GameManager* pGameManager ) { return true; }
    virtual void InitFunc( GameSys::GameManager* pGameManager );
    virtual void EndFunc( GameSys::GameManager* pGameManager );
    virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

  private:
    //  イベントのシーケンス
    enum
    {
      SEQ_NONE  = 0,
      SEQ_VIDEO_PLAYER_PROC,
      SEQ_BTL_TAME_PROC,
      SEQ_BTL_AI_PROC,
      SEQ_BTL_PROC,
      SEQ_PROCT_WAIT,
      SEQ_ERROR_NET_WAIT,
      SEQ_END
    };

    //  再生データタイプ設定
    void SetPlayerDataType( const PlayerDataType dataType )
    {
      m_playerDataType  = dataType;
    }

    //  プレイヤーアプリ呼び出し
    void _CallPlayerProc();

    //  バトル実行パラメータ作成
    bool  _CreateBtlSetupParam( BATTLE_SETUP_PARAM* pOutputParam, GameSys::GameManager* pGameManager );
    //  バトル実行パラメータ破棄
    void  _DestroyBtlSetupParam( BATTLE_SETUP_PARAM* pParam );

    //  シーケンス設定
    u32 _StartSeq( const u32 seq, GameSys::GameManager* pGameManager );

    //  バトル模擬線シーケンス設定
    u32 _StartBtlAISeq( pml::PokeParty* pSelfPokeParty, GameSys::GameManager* pGameManager );

    //  模擬戦データ作成
    bool _CreateDuelPokemonData( pml::PokeParty* pSelfPokeParty );

    void  _Clear()
    {
      m_pProc             = NULL;
      m_pBtlParty         = NULL;
      m_pLocalHeap        = NULL;
#if PM_DEBUG
      m_bDebug            = false;
#endif

      m_playerDataType    = PLAYAER_DATA_TYPE_LOCAL;
    }

    GameSys::GameProc*                            m_pProc;

    NetApp::BattleVideoPlayer::PROC_APP_PARAM     m_appParam;
    NetApp::TeamSelect::EVENT_APP_PARAM           m_themSelectEventAppParam;

    PokeTool::BattleParty*                        m_pBtlParty;

    gfl2::heap::HeapBase*                         m_pLocalHeap;
    PlayerDataType                                m_playerDataType;

    //  バトル再生イベントパラメータ
    NetEvent::BattleVideoCall::EVENT_PARAM        m_battleVideoCallParam;

    NetEvent::FatalError::EventParam              m_fatalEventParam;

#if PM_DEBUG
    bool                                          m_bDebug;
    debug::launcher::LauncherProc::CallParam      m_launcherParam;
#endif
  };

GFL_NAMESPACE_END(BattleVideoPlayer)
GFL_NAMESPACE_END(NetEvent)

#endif // BATTLE_VIDEO_PLAYER_EVENT_H_INCLUDE
