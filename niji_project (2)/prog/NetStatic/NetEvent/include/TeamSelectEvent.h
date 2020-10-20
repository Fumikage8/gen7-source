// ============================================================================
/*
 * @file TeamSelectEvent.h
 * @brief チーム選択アプリ起動イベント
 * @date 2015.06.23
 * @author yuto_uchida
 */
// ============================================================================
#if !defined( TEAM_SELECT_EVENT_H_INCLUDE )
#define TEAM_SELECT_EVENT_H_INCLUDE
#pragma once

//  niji
#include <macro/include/gfl2_Macros.h>
#include "GameSys/include/GameManager.h"
#include "NetStatic/NetEvent/include/NetEventBase.h" // for NetEventBase

#include "NetApp/TeamSelect/include/TeamSelectProc.h"

//  呼び出すイベント
#include "NetStatic/NetEvent/include/QRReaderEvent.h"
#include "NetStatic/NetEvent/include/RegulationSelectEvent.h"

//  boxイベントのリスナー
#include "App/AppEvent/include/Box/BoxEventListener.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)

  struct EVENT_APP_PARAM;

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)

GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(TeamSelect)

  //----------------------------------------------------------------------
  /**
   *  @brief  チーム選択アプリ起動クラス
   */
  //----------------------------------------------------------------------
  class TeamSelectEvent : public NetEvent::NetEventBase, public App::Event::BoxEventListener
  {
    GFL_FORBID_COPY_AND_ASSIGN( TeamSelectEvent );
  public:
    //  イベント開始
    static void StartEvent(GameSys::GameManager* gmgr, NetApp::TeamSelect::EVENT_APP_PARAM* pAppParam );

    // コンストラクタ
    TeamSelectEvent( gfl2::heap::HeapBase* pHeap );
    // デストラクタ
    virtual~TeamSelectEvent();

    virtual bool BootChk( GameSys::GameManager* pGameManager ) { return true; }
    virtual void InitFunc( GameSys::GameManager* pGameManager );
    virtual void EndFunc( GameSys::GameManager* pGameManager );
    virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

    //  App::Event::BoxEventListener
    //-----------------------------------------------------------------------------
    /**
     * @func    OnTermination
     * @brief   終了の通知
     * @param   outJumpMode  ジャンプモード
     * @param   outTray  選んだトレイ番号 ※BoxAppParam.hの特殊な戻り値参照
     * @param   outPos   選んだ場所番号
     */
    //-----------------------------------------------------------------------------
    virtual void OnTermination(u8 outJumpMode, u8 outTray,u8 outPos ) {}

    //-----------------------------------------------------------------------------
    /**
     * @func    OnBattleTray
     * @brief   終了の時のバトルトレイの場所を得る
     * @param   outTray  バトルトレイ番号0-5
     */
    //-----------------------------------------------------------------------------
    virtual void OnBattleTray(u8 outTray );

  private:
    //  アプリ用のパラメータを設定
    void SetAppParam( NetApp::TeamSelect::EVENT_APP_PARAM* appParam )
    {
      GFL_ASSERT( appParam );
      m_pEventAppParam = appParam;
    }

    //----------------------------------------------------------------------
    /**
     *  @brief  チーム選択アプリ開始
     */
    //----------------------------------------------------------------------
    void  _StartApp( GameSys::GameManager* pGameManager );

    //  @brief  データ破棄
    void  _DeleteData();

    //  Savedata::BOX::GetTeamPokePos()から取得した値を意味ある値に変換して返す関数一覧
    //  Boxのトレイ場所に変換
    u32   _ConvBoxTrayPos( u32 num );
    //  Boxのパーティー場所に変換
    u32   _ConvBoxPokemonPartyPos( u32 num );

    //  チームデータがboxか
    bool  _IsBoxTeamData( const NetApp::TeamSelect::TEAM_DATA* pTeamData );

    void  _Clear();

    NetApp::TeamSelect::TeamSelectProc*   m_pProc;
    NetApp::TeamSelect::APP_PARAM         m_appParam;
    NetApp::TeamSelect::EVENT_APP_PARAM*  m_pEventAppParam;

    gfl2::heap::HeapBase*                 m_pUseLocalHeap;
    
    bool                                  m_bReadQR;

    //  レギュレーション
    NetApp::RegulationSelect::APP_PARAM     m_regulationSelectAppParam;
    //  QRのイベントアプリ
    NetEvent::QRReader::QRReaderEventParam  m_qrReaderEventParam;

    //  ボックスイベントが最後に参照したチームno
    u32 m_lastBoxEventSelectTeamNo;

    //  イベント内で使用するローカルヒープサイズ
    static const u32  ms_useLocalHeapSize;

    //  チーム名の文字列データサイズ
    static const u32  ms_teamNameSize;
  };

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetEvent)

#endif // TEAM_SELECT_EVENT_H_INCLUDE
