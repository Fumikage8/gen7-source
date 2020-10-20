//==============================================================================
/**
 @file    TeamSelectUIViewPool.h
 @date    2015.06.25
 @note
          チーム選択のUIVIEW生成
 */
//==============================================================================
#include "NetApp/TeamSelect/source/TeamSelectUIViewPool.h"

//  nijiのインクルード
#include "System/include/HeapDefine.h"
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookType.h"
#include "NetApp/TeamSelect/source/RequestSequence/TeamSelectResponseListener.h"

//  サウンド
#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)

  TeamSelectUIViewPool::TeamSelectUIViewPool()
  {
    _Clear();
  }

  TeamSelectUIViewPool::~TeamSelectUIViewPool()
  {
    GFL_SAFE_DELETE( m_pLowRootView );
    GFL_SAFE_DELETE( m_pLowTopSeqSelectView );
    GFL_SAFE_DELETE( m_pLowTwoItemSelectView );
    GFL_SAFE_DELETE( m_pLowTopPokePartySelectView );
    GFL_SAFE_DELETE( m_pLowInfoWindowBookTypeView );

    GFL_SAFE_DELETE( m_pUppRootView );
    GFL_SAFE_DELETE( m_pUppEntryPokePartyView );
  }

  //------------------------------------------------------------------
  /**
    * @brief  初期化
    * @param[in]  初期化パラメータ
    * @note
    *         バイナリデータからUIViewを生成、あくまで生成だけ
    */
  //------------------------------------------------------------------
  void TeamSelectUIViewPool::Initialize( const struct INIT_PARAM& rInitParam )
  {
    GFL_ASSERT( rInitParam.pUtilHeap );
    GFL_ASSERT( rInitParam.pAppMsgData );
    GFL_ASSERT( rInitParam.pWordSet );
    GFL_ASSERT( rInitParam.pMenuCursorBinData );

    // チーム選択データがない時に rInitParam.pPokeIconTexManager == NULL になるのでスルーさせる
    //GFL_ASSERT( rInitParam.pPokeIconTexManager );

    //  下画面のView作成
    {
      m_pLowRootView  = GFL_NEW( rInitParam.pUtilHeap->GetDeviceHeap() ) TeamSelectLowRootView( rInitParam.pUtilHeap, rInitParam.pRenderManager );
      m_pLowRootView->Initialize( rInitParam.pLytBinData, rInitParam.pAppMsgData, rInitParam.pWordSet );

      //  先頭シーケンス３択View
      m_pLowTopSeqSelectView  = GFL_NEW( rInitParam.pUtilHeap->GetDeviceHeap() ) TeamSelectLowTopSeqSelectView( rInitParam.pUtilHeap, rInitParam.pRenderManager );
      m_pLowTopSeqSelectView->Initialize( rInitParam.pLytBinData, rInitParam.pMenuCursorBinData, rInitParam.pAppMsgData, rInitParam.pWordSet );

      //  ２択View
      m_pLowTwoItemSelectView = GFL_NEW( rInitParam.pUtilHeap->GetDeviceHeap() ) TeamSelectLowTwoItemSelectView( rInitParam.pUtilHeap, rInitParam.pRenderManager );
      m_pLowTwoItemSelectView->Initialize( rInitParam.pLytBinData, rInitParam.pMenuCursorBinData, rInitParam.pAppMsgData, rInitParam.pWordSet );

      //  インフォウィンドウView
      m_pLowInfoWindowBookTypeView  = GFL_NEW( rInitParam.pUtilHeap->GetDeviceHeap() ) app::tool::InfoWindowBookType( rInitParam.pUtilHeap );
      m_pLowInfoWindowBookTypeView->StartAsyncCreate( rInitParam.pUtilHeap, rInitParam.pRenderManager, REGULATION_NG_TYPE_MAX, NULL );

      //  ポケモンパーティ選択View
      if( 0 < rInitParam.teamDataNum )
      {
        struct TeamSelectLowTopPokePartySelectView::INIT_PARAM  initParam;
        initParam.pAppMsgData           = rInitParam.pAppMsgData;
        initParam.pWordSet              = rInitParam.pWordSet;
        initParam.pLytBinData           = rInitParam.pLytBinData;
        initParam.pPokeIconTexManager   = rInitParam.pPokeIconTexManager;
        initParam.pViewParamArray       = rInitParam.pTeamData;
        initParam.viewArrayNum          = rInitParam.teamDataNum;

        m_pLowTopPokePartySelectView  = GFL_NEW( rInitParam.pUtilHeap->GetDeviceHeap() ) TeamSelectLowTopPokePartySelectView( rInitParam.pUtilHeap, rInitParam.pRenderManager );
        m_pLowTopPokePartySelectView->Initialize( initParam );

        //  リストの初回カレント位置を指定
        m_pLowTopPokePartySelectView->SetStateAutoScroll( rInitParam.firstTeamCurrentIdx, false );

        //  初回位置決定なのでスクロールのSEを再生しないために
        m_pLowTopPokePartySelectView->SetEnableListPlaySE( false );
      }
    }

    //  上画面のView作成
    {
      m_pUppRootView  = GFL_NEW( rInitParam.pUtilHeap->GetDeviceHeap() ) TeamSelectUppRootView( rInitParam.pUtilHeap, rInitParam.pRenderManager );
      m_pUppRootView->Initialize( rInitParam.pLytBinData, rInitParam.pAppMsgData, rInitParam.pWordSet );

      //  エントリーしたポケモンパーティView
      m_pUppEntryPokePartyView  = GFL_NEW( rInitParam.pUtilHeap->GetDeviceHeap() ) TeamSelectUppEntryPokePartyView( rInitParam.pUtilHeap, rInitParam.pRenderManager );
      m_pUppEntryPokePartyView->Initialize( rInitParam.pLytBinData, rInitParam.pAppMsgData, rInitParam.pWordSet, rInitParam.pPokeIconTexManager );
    }

    m_readySeq  = 0;
    m_initParam = rInitParam;
  }

  //------------------------------------------------------------------
  /**
    * @brief  使用可能
    * @return 使用可能 true / 使用不可能 false
    */
  //------------------------------------------------------------------
  bool TeamSelectUIViewPool::IsReady()
  {
    bool  bRet  = false;

    switch( m_readySeq )
    {
      //  違反ウィンドウ生成待ち
      case 0:
      {
        m_pLowInfoWindowBookTypeView->UpdateTree();
        if( m_pLowInfoWindowBookTypeView->IsCreated() == true )
        {
          ++m_readySeq;
        }

        break;
      }

      //  チーム選択初期カーソル設定待ち
      case 1:
      {
        if( m_pLowTopPokePartySelectView != NULL )
        {
          if( m_pLowTopPokePartySelectView->GetState() == TeamSelectLowTopPokePartySelectView::STATE_AUTO_SCROLL )
          {
            m_pLowTopPokePartySelectView->UpdateTree();            
          }
          else
          {
            //  もう移動が終わったのでSEは再生できるようにする
            m_pLowTopPokePartySelectView->SetEnableListPlaySE( true );

            ++m_readySeq;
          }
        }
        else
        {
          ++m_readySeq;
        }

        break;
      }
      //  準備完了
      case 2:
      {
        //  もう移動が終わったのでSEは再生できるようにする
        bRet  = true;
        break;
      }
    }

    return  bRet;
  }

  //------------------------------------------------------------------
  /**
    * @brief  終了
    */
  //------------------------------------------------------------------
  void TeamSelectUIViewPool::Terminate()
  {
    m_pLowRootView->Terminate();
    m_pLowTopSeqSelectView->Terminate();
    m_pLowTwoItemSelectView->Terminate();

    if( m_pLowTopPokePartySelectView != NULL )
    {
      m_pLowTopPokePartySelectView->Terminate();
    }

    m_pUppRootView->Terminate();
    m_pUppEntryPokePartyView->Terminate();
  }

  //------------------------------------------------------------------
  /**
    * @brief  終了しているか取得
    * @note
    *         Teaminate関数を呼んですぐに終了処理が終わるわけではないので、
    *         戻り値がfalseになるまで監視する
    */
  //------------------------------------------------------------------
  bool TeamSelectUIViewPool::IsTerminate()
  {
    bool  bRet  = true;

    if( m_pLowRootView->IsDelete() == false )
    {
      bRet  = false;
    }

    if( m_pLowTopSeqSelectView->IsDelete() == false )
    {
      bRet  = false;
    }

    if( m_pLowTwoItemSelectView->IsDelete() == false )
    {
      bRet  = false;
    }

    if( m_pUppRootView->IsDelete() == false )
    {
      bRet  = false;
    }

    if( m_pLowTopPokePartySelectView != NULL )
    {
      if( m_pLowTopPokePartySelectView->IsDelete() == false )
      {
        bRet  = false;
      }
    }

    if( m_pUppEntryPokePartyView->IsDelete() == false )
    {
      bRet  = false;
    }

    if( m_pLowInfoWindowBookTypeView->EndFunc() == false )
    {
      bRet  = false;
    }

    return  bRet;
  }

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)
