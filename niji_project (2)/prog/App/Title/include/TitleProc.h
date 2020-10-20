#if !defined(NIJI_PROJECT_TITLE_PROC_H_INCLUDED)
#define NIJI_PROJECT_TITLE_PROC_H_INCLUDED
#pragma once

//==============================================================================
/**
 @file    TitleProc.h
 @date    2015.05.08
 @note
          タイトルプロセス
 */
//==============================================================================

// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include "GameSys/include/GameProc.h"
#include "GameSys/include/GameManager.h"
#include "Applib/include/Frame/AppFrameListener.h"

#include "App/Title/include/TitleFrame.h"

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
GFL_NAMESPACE_BEGIN(title)

  //  処理終了時の結果
  enum ProcRet
  {
    PROC_RET_DEBUG_SEQ  = 0,
    PROC_RET_GAME_SEQ,
    PROC_RET_DELETE_SAVE
  };

  //  procアプリ用のパラメータ
  struct APP_PARAM
  {
    /** 出力用パラメータ */
    struct _tag_out_param
    {
      ProcRet ret;

    } out_param;
  };

  //==============================================================================
  /**
   @class     TitleProc
   */
  //==============================================================================
  class TitleProc : public GameSys::GameProc, applib::frame::AppFrameListener
  {
    GFL_FORBID_COPY_AND_ASSIGN(TitleProc);

  public:
    TitleProc();
    virtual ~TitleProc();

    //------------------------------------------------------------------
    /**
     * @brief	  動作に必要な引数を登録
     * @note
     *    必ずインスタンス生成の段階で呼び出さないと後の動作保障はなし
     */
    //------------------------------------------------------------------
    void Initialize( APP_PARAM* pAppParam );

    virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );
    virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );
    virtual void PreDrawFunc( gfl2::proc::Manager* pManager );
    virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );
    virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pManager );

    //------------------------------------------------------------------
    /**
     * @brief	  EndFuncが完了した直後に呼び出される関数
     */
    //------------------------------------------------------------------
    virtual void EndFuncFinish( applib::frame::CellSuper* pEndCell );

  private:

    void _Clear()
    {
      m_pHeapMem              = NULL;
      m_pDevMem               = NULL;
      m_pUtilheap             = NULL;
      m_pRenderingManager     = NULL;
      m_pFrameManager         = NULL;
      m_pTitleFrame           = NULL;
      m_pAppParam             = NULL;

      m_pNijiAllocatorForEdgeMapTexture = NULL;
    }

    gfl2::heap::HeapBase*             m_pHeapMem;  // heapMemでなければならないところに使うメモリ
    gfl2::heap::HeapBase*             m_pDevMem;   // devMemでなければならないところに使うメモリ

    app::util::Heap*                  m_pUtilheap;

    app::util::AppRenderingManager*   m_pRenderingManager;

    applib::frame::Manager*           m_pFrameManager;

    TitleFrame*                       m_pTitleFrame;
    TitleFrame::APP_PARAM             m_titleFrameAppParam;

    //  3Dモデル描画用
    System::nijiAllocator*            m_pNijiAllocatorForEdgeMapTexture;  // VRAM限定。

    struct APP_PARAM*                 m_pAppParam;
  };

GFL_NAMESPACE_END(title)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_TITLE_PROC_H_INCLUDED

