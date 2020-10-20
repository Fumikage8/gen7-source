#if !defined(NIJI_PROJECT_BATTLE_VIDEO_RECORDING_PROC_H_INCLUDED)
#define NIJI_PROJECT_BATTLE_VIDEO_RECORDING_PROC_H_INCLUDED
#pragma once

//==============================================================================
/**
 @file    BattleVideoRecordingProc.h
 @date    2015.09.15
 @note
          バトルビデオ録画プロセス
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
#include "NetApp/BattleVideoRecording/include/BattleVideoRecordingAppParam.h"

//  アプリ内のフレーム
#include "NetApp/BattleVideoRecording/source/Frame/BattleVideoRecordingDataChkFrame.h"
#include "NetApp/BattleVideoRecording/source/Frame/BattleVideoRecordingDataSaveFrame.h"

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

  class MenuCursor;

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideoRecording)

  //  前方宣言
  class UIViewPool;

  //==============================================================================
  /**
   @class     Proc
   */
  //==============================================================================
  class Proc : public GameSys::GameProc
  {
    GFL_FORBID_COPY_AND_ASSIGN(Proc);

  public:
    Proc( APP_PARAM* pAppParam );
    virtual ~Proc();

    virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );
    virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );
    virtual void PreDrawFunc( gfl2::proc::Manager* pManager );
    virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );
    virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pManager );

  private:
    enum CreateFrame
    {
      CREATE_FRAME_NONE = 0,
      CREATE_FRAME_DATA_CHK,
      CREATE_FRAME_DATA_SAVE
    };

    //  フレーム生成
    void _CreateFrame( const CreateFrame nowSeq );

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
      m_pFrameManagerHeap     = NULL;
      m_pRenderingManager     = NULL;
      m_pFrameManager         = NULL;
      m_subSeq                = 0;

    }

    //  外部設定したAppParam
    APP_PARAM*                                        m_pAppParam;

    u32                                               m_subSeq;

    // heapMemでなければならないところに使うメモリ
    gfl2::heap::HeapBase*                             m_pHeapMem;
    // devMemでなければならないところに使うメモリ
    gfl2::heap::HeapBase*                             m_pDevMem;
    //  frameManager用のヒープ
    gfl2::heap::HeapBase*                             m_pFrameManagerHeap;

    app::util::Heap*                                  m_pUtilheap;

    app::util::AppRenderingManager*                   m_pRenderingManager;
    applib::frame::Manager*                           m_pFrameManager;

    //  レイアウトファイルロード制御
    app::util::AppUtilFileControlState*               m_pLayoutFileCtrlState;

    //  UIViewインスタンスプール
    UIViewPool*                                       m_pUIViewPool;

    //  メッセージ
    gfl2::str::MsgData*                               m_pAppMsgData;
    //  ワードセット
    print::WordSet*                                   m_pWordSet;

    //  メニューカーソルのリソース
    app::tool::MenuCursor*                            m_pMenuCursor;
    void*                                             m_pMenuCursorLytBuffer;

    DataChkFrame::FRAME_PARAM                         m_dataChkFrameParam;
    DataSaveFrame::FRAME_PARAM                        m_dataSaveFrameParam;
  };

GFL_NAMESPACE_END(BattleVideoRecording)
GFL_NAMESPACE_END(NetApp)


#endif  // NIJI_PROJECT_BATTLE_VIDEO_RECORDING_PROC_H_INCLUDED

