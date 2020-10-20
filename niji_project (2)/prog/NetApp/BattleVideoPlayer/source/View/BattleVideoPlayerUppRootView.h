﻿//======================================================================
/**
 * @file BattleVideoPlayerUppRootView.h
 * @date 2015/09/24 16:23:10
 * @author uchida_yuto
 * @brief アプリの上画面ルートView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if !defined __BATTLE_VIDEO_PLAYER_UPP_ROOT_VIEW_H_INCLUDED__
#define __BATTLE_VIDEO_PLAYER_UPP_ROOT_VIEW_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>

//  nijiのインクルード
#include "AppLib/include/ui/UIView.h"
#include "AppLib/include/Tool/CommonMessageWindowManipulator.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(print)

  class WordSet;

GFL_NAMESPACE_END(print)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  class CommonMessageObject;

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideoPlayer)

  /**
   * @class UppRootView
   * @brief アプリの上画面ルートView
   */
  class UppRootView : public app::ui::UIView
  {
    GFL_FORBID_COPY_AND_ASSIGN(UppRootView);

  public:
    //  状態一覧
    enum State
    {
      STATE_IDLE  = 0,
    };

    enum ShowDialogMessageLine
    {
      SHOW_DIALOG_MESSAGE_LINE_NONE = 0,
      SHOW_DIALOG_MESSAGE_LINE_ONE,
      SHOW_DIALOG_MESSAGE_LINE_TWO,
    };

    //  初期化パラメータ
    struct INIT_PARAM
    {
      INIT_PARAM()
      {
        pHeap           = NULL;
        pRenderManager  = NULL;
        pLytBinData     = NULL;
        pAppMsgData     = NULL;
        pWordSet        = NULL;
      }

      app::util::Heap* pHeap;
      app::util::AppRenderingManager* pRenderManager;
      void* pLytBinData;
      gfl2::str::MsgData* pAppMsgData;
      print::WordSet* pWordSet;
    };

    //------------------------------------------------------------------
    /**
      * @brief   コンストラクタ
      * @param[in] pHeap 使用するメモリ
      */
    //------------------------------------------------------------------
    UppRootView( INIT_PARAM& rInitParam );

    virtual ~UppRootView();

    //------------------------------------------------------------------
    /**
      * @brief   初期化
      */
    //------------------------------------------------------------------
    bool InitFunc();

    //------------------------------------------------------------------
    /**
      * @brief   終了処理
      */
    //------------------------------------------------------------------
    bool EndFunc();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  更新処理
    *         子ウインドウについても自動的にコールする(子→親の順）
    */
    //--------------------------------------------------------------------------------------------
    virtual void Update();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  描画処理
    *         子ウインドウについても自動的にコールする(親→子の順）
    */
    //--------------------------------------------------------------------------------------------
    virtual void Draw(  gfl2::gfx::CtrDisplayNo displayNo );

    //------------------------------------------------------------------
    /**
      * @brief   状態設定
      * @param[in] newStaet 設定する状態
      */
    //------------------------------------------------------------------
    void SetState( const State newStaet );

    //------------------------------------------------------------------
    /**
      * @brief   上タイトルメッセージ表示
      */
    //------------------------------------------------------------------
    void ShowMessageUpperOneLine( gfl2::str::StrBuf* pStrBuf );
    void ShowMessageUpperTwoLine( gfl2::str::StrBuf* pStrBufOneLine, gfl2::str::StrBuf* pStrBufTwoLine );

    //------------------------------------------------------------------
    /**
      * @brief   上タイトルメッセージ非表示
      */
    //------------------------------------------------------------------
    void HideMessageUpper();

    //------------------------------------------------------------------
    /**
      * @brief   中央メッセージ表示
      */
    //------------------------------------------------------------------
    void ShowMessageCenter( gfl2::str::StrBuf* pStrBuf );

    //------------------------------------------------------------------
    /**
      * @brief   中央メッセージ非表示
      */
    //------------------------------------------------------------------
    void HideMessageCenter();

  private:
    //  作成するレイアウトワークの種類
    enum
    {
      LYT_WORK_NORMAL  = 0,
      LYT_WORK_MAX
    };

    //------------------------------------------------------------------
    /**
      * @brief   2D画面の作成
      * @param   pAppLytBuff レイアウトデータのバイナリポインタ
      */
    //------------------------------------------------------------------
    void _Initialize2D( void* pAppLytBuff, gfl2::str::MsgData* pAppMsgData, print::WordSet* pWordSet );

    //------------------------------------------------------------------
    /**
      * @brief   上タイトルメッセージ非表示(1行)
      */
    //------------------------------------------------------------------
    void _HideMessageUpperOneLine();

    //------------------------------------------------------------------
    /**
      * @brief   上タイトルメッセージ非表示(2行)
      */
    //------------------------------------------------------------------
    void _HideMessageUpperTwoLine();

    //------------------------------------------------------------------
    /**
      * @brief   変数の初期化はここに
      */
    //------------------------------------------------------------------
    void _Clear()
    {
      m_state           = STATE_IDLE;
      m_initSeq         = 0;
      m_endSeq          = 0;

      m_pTwoLineCommonMessageObject = NULL;
      m_pOneLineCommonMessageObject = NULL;
      m_pCenterCommonMessageObject  = NULL;
    }

    State                           m_state;

    u32                             m_initSeq;
    u32                             m_endSeq;

    INIT_PARAM                      m_initParam;

    app::tool::CommonMessageObject* m_pTwoLineCommonMessageObject;
    app::tool::CommonMessageObject* m_pOneLineCommonMessageObject;
    app::tool::CommonMessageObject* m_pCenterCommonMessageObject;
  };

GFL_NAMESPACE_END(BattleVideoPlayer)
GFL_NAMESPACE_END(NetApp)

#endif // __BATTLE_VIDEO_PLAYER_UPP_ROOT_VIEW_H_INCLUDED__