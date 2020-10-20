#if !defined(NIJI_PROJECT_CONFIG_PROC_H_INCLUDED)
#define NIJI_PROJECT_CONFIG_PROC_H_INCLUDED
#pragma once

//==============================================================================
/**
 @file    ConfigProc.h
 @date    2015.05.19
 @note
          コンフィグプロセス
 */
//==============================================================================

// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameProc.h>
#include <GameSys/include/GameManager.h>

#include "App/Config/include/ConfigFrame.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(applib)
GFL_NAMESPACE_BEGIN(frame)

  class Manager;

GFL_NAMESPACE_END(frame)
GFL_NAMESPACE_END(applib)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

  class AppRenderingManager;
  class Heap;

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(config)

  //  前方宣言
  class ConfigUIViewPool;

  //==============================================================================
  /**
   @class     ConfigProc
   */
  //==============================================================================
  class ConfigProc : public GameSys::GameProc
  {
    GFL_FORBID_COPY_AND_ASSIGN(ConfigProc);

  public:
    ConfigProc();
    virtual ~ConfigProc();

    virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );
    virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );
    virtual void PreDrawFunc( gfl2::proc::Manager* pManager );
    virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );
    virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pManager );

  private:

    void _Clear()
    {
      m_pHeapMem          = NULL;
      m_pDevMem           = NULL;
      m_pUtilheap         = NULL;
      m_pRenderingManager = NULL;
      m_pFrameManager     = NULL;
      m_pConfigUIViewPool = NULL;
      m_pNormalMessage    = NULL;
      m_pKanaMessage      = NULL;
      m_pKanjiMessage     = NULL;
      m_pWordSet          = NULL;
      m_pConfigFrame      = NULL;
      m_bOldInputKeyByButtonAnimation = false;
    }

    gfl2::heap::HeapBase*             m_pHeapMem;  // heapMemでなければならないところに使うメモリ
    gfl2::heap::HeapBase*             m_pDevMem;   // devMemでなければならないところに使うメモリ

    app::util::Heap*                  m_pUtilheap;

    app::util::AppRenderingManager*   m_pRenderingManager;
    applib::frame::Manager*           m_pFrameManager;

    //  デフォルトメッセージデータ
    gfl2::str::MsgData*               m_pNormalMessage;

    //  オプションの仮名/漢字切り替えで表示している日本語を即時切替ために必要(しかしこれは日本語版限定)
    gfl2::str::MsgData*               m_pKanaMessage;
    gfl2::str::MsgData*               m_pKanjiMessage;

    // 単語セット
    print::WordSet*                   m_pWordSet;

    ConfigFrame*                      m_pConfigFrame;
    ConfigFrame::APP_PARAM            m_configFrameAppParam;
    ConfigUIViewPool*                 m_pConfigUIViewPool;

    bool                              m_bOldInputKeyByButtonAnimation;
  };

GFL_NAMESPACE_END(config)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_CONFIG_PROC_H_INCLUDED

