#if !defined(NIJI_PROJECT_CONFIG_UIVIEW_POOL_H_INCLUDED)
#define NIJI_PROJECT_CONFIG_UIVIEW_POOL_H_INCLUDED
#pragma once

//==============================================================================
/**
 @file    ConfigUIViewPool.h
 @date    2015.12.15
 @note
          アプリで使うUIVIEWすべて生成して管理
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
#include "AppLib/include/Tool/app_tool_MessageMenuView.h"

//  Viewのインクルード
#include "App/Config/source/parts/ConfigLowerUIDraw.h"
#include "App/Config/source/parts/ConfigUpperUIDraw.h"

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
  class AppUtilFileControlState;

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
GFL_NAMESPACE_BEGIN(config)

  //==============================================================================
  /**
   @class     ConfigUIViewPool
   */
  //==============================================================================
  class ConfigUIViewPool
  {
    GFL_FORBID_COPY_AND_ASSIGN(ConfigUIViewPool);

  public:
    //  初期化パラメータ
    struct INIT_PARAM
    {
      INIT_PARAM()
      {
        pUtilHeap           = NULL;
        pRenderManager      = NULL;
        pAppMsgData         = NULL;
        pWordSet            = NULL;
        pConfigData         = NULL;
      }

      app::util::Heap*                  pUtilHeap;
      app::util::AppRenderingManager*   pRenderManager;
      gfl2::str::MsgData*               pAppMsgData;
      print::WordSet*                   pWordSet;
      //  コンフィグデータアドレス
      Savedata::ConfigSave*             pConfigData;
    };

    ConfigUIViewPool( const struct INIT_PARAM& rInitParam );
    virtual ~ConfigUIViewPool();

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
    ConfigLowerUIDraw*             GetRootLowView() const { return m_pLowRootView; }
    //  下画面のメニューウィンドウView取得
    App::Tool::MessageMenuView*    GetMessageMenuLowView() const { return m_pLowMessageMenuView; }

    //  上画面のルートView取得
    ConfigUpperUIDraw*  GetRootUppView() const { return m_pUppRootView; }

    //  初期パラメータ取得
    const struct INIT_PARAM&  GetInitParam() const { return m_initParam; }

  private:
    //  ファイル制御一覧
    enum
    {
      FILE_DATA_UI_LYT  = 0,
      FILE_DATA_MENU_WINDOW,

      FILE_DATA_MAX
    };

    //  指定した設定タイプに沿って、リスト項目のデータ出力
    void _OutputItemListData( ConfigLowerUIDraw::LIST_ITEM_PARAM* pOutputData, const ConfigLowerUIDraw::ConfigType type );

    void _Clear()
    {
      m_pUppRootView          = NULL;
      m_pMenuCursor           = NULL;
      m_pLowRootView          = NULL;
      m_pLowMessageMenuView   = NULL;
      m_pMenuCursorLytDataBuffer  = NULL;

      gfl2::std::MemClear( m_pFileCtrlStateList, sizeof( m_pFileCtrlStateList) );
      m_initSeq = 0;
      m_endSeq  = 0;
    }

    ConfigUpperUIDraw*          m_pUppRootView;
    ConfigLowerUIDraw*          m_pLowRootView;
    App::Tool::MessageMenuView* m_pLowMessageMenuView;
    app::tool::MenuCursor*      m_pMenuCursor;
    void*                       m_pMenuCursorLytDataBuffer;
        //  ファイル制御リスト
    app::util::AppUtilFileControlState* m_pFileCtrlStateList[ FILE_DATA_MAX ];

    INIT_PARAM                      m_initParam;

    u32 m_initSeq;
    u32 m_endSeq;
  };

GFL_NAMESPACE_END(config)
GFL_NAMESPACE_END(app)

#endif  // NIJI_PROJECT_CONFIG_UIVIEW_POOL_H_INCLUDED

