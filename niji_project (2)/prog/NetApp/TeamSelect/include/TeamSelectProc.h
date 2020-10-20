#if !defined(NIJI_PROJECT_TEAN_SELECT_PROC_H_INCLUDED)
#define NIJI_PROJECT_TEAN_SELECT_PROC_H_INCLUDED
#pragma once

//==============================================================================
/**
 @file    TeamSelectProc.h
 @date    2015.06.23
 @note
          チーム選択プロセス
 */
//==============================================================================

// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameProc.h>
#include <GameSys/include/GameManager.h>

//  アプリでの公開用
#include "NetApp/TeamSelect/include/TeamSelectAppParam.h"

//  アプリ内のフレーム
#include "NetApp/TeamSelect/source/Frame/TeamSelectTopSeqSelectFrame.h"
#include "NetApp/TeamSelect/source/Frame/TeamSelectTopPokePartySelectFrame.h"
#include "NetApp/TeamSelect/source/Frame/TeamSelectConfirmSelectPokePartyFrame.h"
#include "NetApp/TeamSelect/source/Frame/TeamSelectErrorRegulationFrame.h"
#include "NetApp/TeamSelect/source/Frame/TeamSelectServerRequestRegulationFrame.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(print)

  class WordSet;

GFL_NAMESPACE_END(print)

GFL_NAMESPACE_BEGIN(applib)
GFL_NAMESPACE_BEGIN(frame)

  class Manager;

GFL_NAMESPACE_END(frame)
GFL_NAMESPACE_END(applib)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

  class AppRenderingManager;
  class AppUtilFileControlState;
  class Heap;

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
  
  class PokeIcon;
  class MenuCursor;

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

#if PM_DEBUG
class RegulationDebug;
#endif


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)

  //  前方宣言
  class TeamSelectUIViewPool;

  //==============================================================================
  /**
   @class     TeamSelectProc
   */
  //==============================================================================
  class TeamSelectProc : public GameSys::GameProc
  {
    GFL_FORBID_COPY_AND_ASSIGN(TeamSelectProc);

  public:
    TeamSelectProc( APP_PARAM* pAppParam );
    virtual ~TeamSelectProc();

    virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );
    virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );
    virtual void PreDrawFunc( gfl2::proc::Manager* pManager );
    virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );
    virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pManager );

  private:
    //  UpdateFuncのシーケンス
    enum
    {
      //  フェードイン処理
      UPDATE_SEQ_FADE_IN_START = 0,
      UPDATE_SEQ_FADE_IN_WAIT,

      //  トップ処理
      UPDATE_SEQ_TOP_START,
      UPDATE_SEQ_TOP_WAIT,
      UPDATE_SEQ_TOP_END,

      //  チーム確認処理
      UPDATE_SEQ_TEAM_ENTRY_CONFIRMATION_START,
      UPDATE_SEQ_TEAM_ENTRY_CONFIRMATION_WAIT,
      UPDATE_SEQ_TEAM_ENTRY_CONFIRMATION_END,

      //  レギュレーションチェック処理
      UPDATE_SEQ_CHK_REQULATION_START,
      UPDATE_SEQ_CHK_REQULATION_WAIT,
      UPDATE_SEQ_CHK_REQULATION_END,

      //  レギュレーション違反処理
      UPDATE_SEQ_VIOLATION_REQULATION_START,
      UPDATE_SEQ_VIOLATION_REQULATION_WAIT,
      UPDATE_SEQ_VIOLATION_REQULATION_END,

      //  エラーメッセージ処理
      UPDATE_SEQ_ERROR_MSG_START,
      UPDATE_SEQ_ERROR_MSG_WAIT,
      UPDATE_SEQ_ERROR_MSG_END,

      UPDATE_SEQ_END
    };

    enum
    {
      SELECT_TEAM_DATA_IDX_ERROR  = 0xffff
    };

    //------------------------------------------------------------------
    /**
     * @brief   シーケンス変更
     */
    //------------------------------------------------------------------
    u32 _SetSeq( const u32 seq );

    //------------------------------------------------------------------
    /**
     * @brief   すでに選択決定が予約しているチームデータのlistIdxを取得
     * @return  選択決定チームデータidx / ない場合は SELECT_TEAM_DATA_IDX_ERROR
     */
    //------------------------------------------------------------------
    u32 _GetAppintEnterTeamDataIdx();

    //  チームデータがあるか
    bool _IsTeamData();

    void _Clear()
    {
      m_pAppParam             = NULL;
      m_pHeapMem              = NULL;
      m_pDevMem               = NULL;
      m_pUtilheap             = NULL;
      m_pUIViewPool           = NULL;
      m_pLayoutFileCtrlState  = NULL;
      m_pAppMsgData           = NULL;
      m_pWordSet              = NULL;
      m_pMenuCursor           = NULL;
      m_pMenuCursorLytBuffer  = NULL;
      m_pServerRequestClient  = NULL;
      m_pPokeIconResManager   = NULL;
      m_pTeamViewParamList    = NULL;
      m_pFrameManagerHeap     = NULL;
      m_pRenderingManager     = NULL;
      m_pFrameManager         = NULL;
      m_bServerSignal         = false;
      m_bOldInputExclusiveKey = false;

      m_pTopPokePartySelectFrame  = NULL;
      m_pTopSeqSelectFrame        = NULL;
      m_teamNum = 0;
      m_subSeq  = 0;
    }

    //  外部設定したAppParam
    APP_PARAM*                                        m_pAppParam;

    u32                                               m_teamNum;
    u32                                               m_subSeq;

    gfl2::heap::HeapBase*                             m_pHeapMem;  // heapMemでなければならないところに使うメモリ
    gfl2::heap::HeapBase*                             m_pDevMem;   // devMemでなければならないところに使うメモリ
    gfl2::heap::HeapBase*                             m_pFrameManagerHeap;  //  frameManager用のヒープ

    app::util::Heap*                                  m_pUtilheap;

    app::util::AppRenderingManager*                   m_pRenderingManager;
    applib::frame::Manager*                           m_pFrameManager;

    //  レイアウトファイルロード制御
    app::util::AppUtilFileControlState*               m_pLayoutFileCtrlState;

    //  UIViewインスタンスプール
    TeamSelectUIViewPool*                             m_pUIViewPool;

    bool                                              m_bServerSignal;
    bool                                              m_bOldInputExclusiveKey;

    //  メッセージ
    gfl2::str::MsgData*                               m_pAppMsgData;
    //  ワードセット
    print::WordSet*                                   m_pWordSet;

    //  メニューカーソルのリソース
    app::tool::MenuCursor*                            m_pMenuCursor;
    void*                                             m_pMenuCursorLytBuffer;

    //  ポケモンアイコンのリソース管理
    app::tool::PokeIcon*                              m_pPokeIconResManager;

    //  チーム描画データ
    struct TEAM_VIEW_PARAM*                           m_pTeamViewParamList;

    TeamSelectRequestClient*                          m_pServerRequestClient;

    TeamSelectTopPokePartySelectFrame*                m_pTopPokePartySelectFrame;
    TeamSelectTopSeqSelectFrame*                      m_pTopSeqSelectFrame;

    //  フレームのappparam
    struct TeamSelectTopSeqSelectFrame::APP_PARAM             m_topSeqSelectFrameAppParam;
    struct TeamSelectTopPokePartySelectFrame::APP_PARAM       m_topPokePartySelectFrameAppParam;
    struct TeamSelectConfirmSelectPokePartyFrame::APP_PARAM   m_confirmSelectPokePartyFrameAppParam;
    struct TeamSelectErrorRegulationFrame::APP_PARAM          m_errorRegulationFrameAppParam;
    struct TeamSelectServerRequestRegulationFrame::APP_PARAM  m_serverRequestRegulationFrameAppParam;

    // レギュレーションデバッグ機能
#if PM_DEBUG
    RegulationDebug* m_pRegulationDebug;
#endif
  };

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)


#endif  // NIJI_PROJECT_TEAN_SELECT_PROC_H_INCLUDED

