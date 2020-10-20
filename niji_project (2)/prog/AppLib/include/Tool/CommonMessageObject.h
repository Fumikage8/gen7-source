//======================================================================
/**
 * @file CommonMessageObject.h
 * @date 2015/10/09 14:48:36
 * @author uchida_yuto
 * @brief CommenMessageの処理共通
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __COMMON_MESSAGE_OBJECT_HEADER_H_INCLUDED__
#define __COMMON_MESSAGE_OBJECT_HEADER_H_INCLUDED__
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <str/include/gfl2_Str.h>
#include <Layout/include/gfl2_Layout.h>

//  nijiのインクルード
#include "Print/include/MessageWindow.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

  class G2DUtil;

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
  /**
   * @class CommonMessageObject
   * @brief parsのCommonMessageの共通制御
   * @note  メッセージの共通処理化を目的に作成
   *        汎用に使えるようになれば AppLibに昇格
   */
  class CommonMessageObject
  {
    GFL_FORBID_COPY_AND_ASSIGN( CommonMessageObject );
  public:
    enum
    {
      TEXT_BOX_PANE_MAX = 2,
    };

    //  初期化パラメータ
    struct INIT_PARAM
    {
      INIT_PARAM()
      {
         pG2DUtil = NULL;
         pWindowPane  = NULL;

         gfl2::std::MemClear( pTextBoxPaneArray, sizeof( pTextBoxPaneArray ) );

         textBoxPaneNum = 0;
         showWaitFrame  = 0;
         lytId          = 0;

         inAnimeId  = gfl2::lyt::LYTWKBASE_ANIMEIDX_ERROR;
         outAnimeId = gfl2::lyt::LYTWKBASE_ANIMEIDX_ERROR;
      };

      //  アニメ制御、ペインにテキスト設定するのに必要
      app::util::G2DUtil* pG2DUtil;
      /// アニメ制御に必要
      u32 lytId;

      //  テキスト設定するペインリスト
      gfl2::lyt::LytTextBox* pTextBoxPaneArray[ TEXT_BOX_PANE_MAX ];

      //  テキスト設定するペインリストの個数
      u32 textBoxPaneNum;

      //  表示時のアニメid
      u32 inAnimeId;
      //  非表示のアニメid
      u32 outAnimeId;

      //  表示のウェイト
      u32 showWaitFrame;

      //  ウィンドウ本体のペイン(表示設定をするのに必要)
      gfl2::lyt::LytPane* pWindowPane;
    };

    CommonMessageObject();

    //  初期化
    bool InitFunc( const INIT_PARAM& rParam, gfl2::heap::HeapBase* pHeap );

    //  更新
    void Update();

    //  終了
    bool EndFunc();

    //  表示
    void ShowMessage( gfl2::str::StrBuf& rStr, const bool bIsImmediateMessage );

    //  ペインに直接設定
    //  INIT_PARAM::textBoxPnaeNum < listNum でないと動作しない
    void ShowMessageTextPane( gfl2::str::StrBuf** pStrArray, const u32 listNum );

    //  非表示
    void HideMessage();

  private:
    void _InAnime();
    void _OutAnime();

    void _Clear()
    {
      m_pMsgWin             = NULL;
      m_bShow               = false;
      m_bIsImmediateMessage = false;
      m_bMsgProc            = false;
    }

    INIT_PARAM m_initParam;
    print::MessageWindow * m_pMsgWin;     //!< メッセージウィンドウ

    bool  m_bShow;
    bool  m_bIsImmediateMessage;
    bool  m_bMsgProc;
  };

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

#endif // __COMMON_MESSAGE_OBJECT_HEADER_H_INCLUDED__