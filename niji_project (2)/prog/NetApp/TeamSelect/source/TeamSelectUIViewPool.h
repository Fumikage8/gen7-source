#if !defined(NIJI_PROJECT_TEAN_SELECT_UIVIEW_POOL_H_INCLUDED)
#define NIJI_PROJECT_TEAN_SELECT_UIVIEW_POOL_H_INCLUDED
#pragma once

//==============================================================================
/**
 @file    TeamSelectUIViewPool.h
 @date    2015.06.25
 @note
          チーム選択のUIVIEW生成
          ここでアプリが表示するViewを一括で生成して破棄の管理をする
          またView生成関数を呼んだ時にRootViewのAddやRemoveなどを呼ぶことをする
          これで使う側がAdd/Removeについて気にすることはない
 */
//==============================================================================

//  UIView関連のヘッダーをインクルード
#include "NetApp/TeamSelect/source/View/TeamSelectLowRootView.h"
#include "NetApp/TeamSelect/source/View/TeamSelectLowTopSeqSelectView.h"
#include "NetApp/TeamSelect/source/View/TeamSelectLowTwoItemSelectView.h"
#include "NetApp/TeamSelect/source/View/TeamSelectLowTopPokePartySelectView.h"

#include "NetApp/TeamSelect/source/View/TeamSelectUppRootView.h"
#include "NetApp/TeamSelect/source/View/TeamSelectUppEntryPokePartyView.h"

// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameProc.h>
#include <GameSys/include/GameManager.h>

// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  class InfoWindowBookType;

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(print)

  class WordSet;

GFL_NAMESPACE_END(print)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

  class Heap;

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(ui)

  class UIView;

GFL_NAMESPACE_END(ui)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  class PokeIcon;

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)

  //==============================================================================
  /**
   @class     TeamSelectUIViewPool
   */
  //==============================================================================
  class TeamSelectUIViewPool
  {
    GFL_FORBID_COPY_AND_ASSIGN(TeamSelectUIViewPool);

  public:
    //  初期化パラメータ
    struct INIT_PARAM
    {
      INIT_PARAM()
      {
        pUtilHeap           = NULL;
        pLytBinData         = NULL;
        pMenuCursorBinData  = NULL;
        pRenderManager      = NULL;
        pAppMsgData         = NULL;
        pWordSet            = NULL;
        pPokeIconTexManager = NULL;
        pTeamData           = NULL;
        teamDataNum         = 0;
        firstTeamCurrentIdx = 0;
      }

      app::util::Heap*                  pUtilHeap;
      void*                             pLytBinData;
      void*                             pMenuCursorBinData;
      app::util::AppRenderingManager*   pRenderManager;
      gfl2::str::MsgData*               pAppMsgData;
      print::WordSet*                   pWordSet;
      app::tool::PokeIcon*              pPokeIconTexManager;

      //  チームデータと数
      struct TEAM_VIEW_PARAM*     pTeamData;
      u32                         teamDataNum;
      u32                         firstTeamCurrentIdx;
    };

    TeamSelectUIViewPool();
    virtual ~TeamSelectUIViewPool();

    //------------------------------------------------------------------
    /**
      * @brief      初期化
      * @param[in]  初期化パラメータ
      * @note
      *         バイナリデータからUIViewを生成、あくまで生成だけ
      */
    //------------------------------------------------------------------
    void Initialize( const struct INIT_PARAM& rInitParam );

    //------------------------------------------------------------------
    /**
      * @brief  使用可能
      * @return 使用可能 true / 使用不可能 false
      */
    //------------------------------------------------------------------
    bool IsReady();

    //------------------------------------------------------------------
    /**
      * @brief  終了
      */
    //------------------------------------------------------------------
    void Terminate();

    //------------------------------------------------------------------
    /**
      * @brief  終了しているか取得
      * @note
      *         Teaminate関数を呼んですぐに終了処理が終わるわけではないので、
      *         戻り値がfalseになるまで監視する
      */
    //------------------------------------------------------------------
    bool IsTerminate();

    //  初期パラメータ取得
    const struct INIT_PARAM&  GetInitParam() const { return m_initParam; }

    //  下画面のルートView
    TeamSelectLowRootView*  GetLowRootView() { return m_pLowRootView; }

    //  下画面の先頭３択のView
    TeamSelectLowTopSeqSelectView*  GetLowTopSeqSelectView() { return m_pLowTopSeqSelectView; }

    //  下画面の先頭2択のView
    TeamSelectLowTwoItemSelectView*  GetLowTwoItemSelectView() { return m_pLowTwoItemSelectView; }

    //  下画面の先頭ポケモンパーティの選択View
    TeamSelectLowTopPokePartySelectView*  GetLowTopPokePartySelectView() { return m_pLowTopPokePartySelectView; }

    //  下画面のインフォウィンドウView
    app::tool::InfoWindowBookType*  GetLowInfoWindowBookType() { return m_pLowInfoWindowBookTypeView; }

    //  上画面のルートView
    TeamSelectUppRootView*  GetUppRootView() { return m_pUppRootView; }

    //  上画面のエントリーしたポケモンパーティView
    TeamSelectUppEntryPokePartyView*  GetUppEntryPokePartyView() { return m_pUppEntryPokePartyView; }

  private:

    void _Clear()
    {
      m_pLowRootView      = NULL;

      m_pLowTopSeqSelectView        = NULL;
      m_pLowTopPokePartySelectView  = NULL;
      m_pLowTwoItemSelectView       = NULL;

      m_pUppEntryPokePartyView      = NULL;
      m_pLowInfoWindowBookTypeView     = NULL;

      m_pUppRootView  = NULL;
      m_readySeq  = 0;
    }

    TeamSelectLowRootView*                m_pLowRootView;
    TeamSelectLowTopSeqSelectView*        m_pLowTopSeqSelectView;
    TeamSelectLowTopPokePartySelectView*  m_pLowTopPokePartySelectView;
    TeamSelectLowTwoItemSelectView*       m_pLowTwoItemSelectView;
    app::tool::InfoWindowBookType*        m_pLowInfoWindowBookTypeView;
    
    TeamSelectUppRootView*                m_pUppRootView;
    TeamSelectUppEntryPokePartyView*      m_pUppEntryPokePartyView;

    u32                                   m_readySeq;
    struct INIT_PARAM                     m_initParam;
  };

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)


#endif  // NIJI_PROJECT_TEAN_SELECT_UIVIEW_POOL_H_INCLUDED

