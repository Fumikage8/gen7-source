#if !defined(NIJI_PROJECT_TRAINER_PHOTO_UIVIEW_POOL_H_INCLUDED)
#define NIJI_PROJECT_TRAINER_PHOTO_UIVIEW_POOL_H_INCLUDED
#pragma once

//==============================================================================
/**
 @file    TrainerPhotoUIViewPool.h
 @date    2015.09.02
 @note
          証明写真アプリで使うUIVIEWすべて生成して管理
          ここでアプリが表示するViewを一括で生成して破棄の管理をする
          またView生成関数を呼んだ時にRootViewのAddやRemoveなどを呼ぶことをする
          これで使う側がAdd/Removeについて気にすることはない
 */
//==============================================================================

//  UIView関連のヘッダーをインクルード

// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameProc.h>
#include <GameSys/include/GameManager.h>

//  Viewのインクルード
#include "App/TrainerPhoto/source/View/TrainerPhotoUppRootView.h"
#include "App/TrainerPhoto/source/View/TrainerPhotoUppShootFrameView.h"
#include "App/TrainerPhoto/source/View/TrainerPhotoLowRootView.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)

  class MsgData;

GFL_NAMESPACE_END(str)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(print)

  class WordSet;

GFL_NAMESPACE_END(print)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

  class Heap;
  class AppRenderingManager;

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  class  MenuWindow;

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(ui)

  class UIView;

GFL_NAMESPACE_END(ui)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(photo)

  //==============================================================================
  /**
   @class     TrainerPhotoUIViewPool
   */
  //==============================================================================
  class TrainerPhotoUIViewPool
  {
    GFL_FORBID_COPY_AND_ASSIGN(TrainerPhotoUIViewPool);

  public:
    enum MENU_WINDWOW_SELECT
    {
      MENU_WINDWOW_SELECT_YES = 0,
      MENU_WINDWOW_SELECT_NO,

      MENU_WINDWOW_SELECT_MAX
    };

    //  初期化パラメータ
    struct INIT_PARAM
    {
      INIT_PARAM()
      {
        pUtilHeap           = NULL;
        pLytBinData         = NULL;
        pRenderManager      = NULL;
        pAppMsgData         = NULL;
        pWordSet            = NULL;
      }

      app::util::Heap*                  pUtilHeap;
      void*                             pLytBinData;
      app::util::AppRenderingManager*   pRenderManager;
      gfl2::str::MsgData*               pAppMsgData;
      print::WordSet*                   pWordSet;
    };

    TrainerPhotoUIViewPool( const struct INIT_PARAM& rInitParam );
    virtual ~TrainerPhotoUIViewPool();

    //------------------------------------------------------------------
    /**
      * @brief      初期化
      * @return     初期化完了: true / 初期化中: false
      */
    //------------------------------------------------------------------
    bool InitFunc();

    //------------------------------------------------------------------
    /**
      * @brief  終了
      * @return 終了完了: true / 終了中: false
      */
    //------------------------------------------------------------------
    bool EndFunc();

    //  下画面のルートView取得
    TrainerPhotoLowRootView*  GetRootLowView() const { return m_pLowRootView; }
    //  下画面のメニューウィンドウView取得
    app::tool::MenuWindow*    GetMenuWindowLowView() const { return m_pLowMenuWindowView; }

    //  上画面のルートView取得
    TrainerPhotoUppRootView*  GetRootUppView() const { return m_pUppRootView; }
    //  上画面の撮影フレームView取得
    TrainerPhotoUppShootFrameView*  GetShootFrameUppView() const { return m_pUppShootFrameView; }

    //  シャッター開始
    void DoEventShutter();
    //  シャッター中か
    bool IsEventShutter();

    //  初期パラメータ取得
    const struct INIT_PARAM&  GetInitParam() const { return m_initParam; }

  private:
    void _Clear()
    {
      m_pUppRootView          = NULL;
      m_pUppShootFrameView    = NULL;
      m_pLowRootView          = NULL;
      m_pLowMenuWindowView    = NULL;

      m_initSeq = 0;
      m_endSeq  = 0;
    }

    TrainerPhotoUppRootView*        m_pUppRootView;
    TrainerPhotoUppShootFrameView*  m_pUppShootFrameView;
    TrainerPhotoLowRootView*        m_pLowRootView;
    app::tool::MenuWindow*          m_pLowMenuWindowView;

    INIT_PARAM                      m_initParam;

    u32 m_initSeq;
    u32 m_endSeq;
  };

GFL_NAMESPACE_END(photo)
GFL_NAMESPACE_END(app)

#endif  // NIJI_PROJECT_TRAINER_PHOTO_UIVIEW_POOL_H_INCLUDED

