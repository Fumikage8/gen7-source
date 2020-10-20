//==============================================================================
/**
 @file    BattleVideoPlayerUIViewPool.cpp
 @date    2015.09.24
 */
//==============================================================================
#include "NetApp/BattleVideoPlayer/source/BattleVideoPlayerUIViewPool.h"

//  nijiのインクルード
#include "System/include/HeapDefine.h"
#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Util/AppUtilFileControlState.h"

//  arc
#include "arc_def_index/arc_def.h"
#include "arc_index/MenuWindow.gaix"

//  メッセージのインクルード
#include "niji_conv_header/message/msg_battlevideo_player.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideoPlayer)

  UIViewPool::UIViewPool( const struct INIT_PARAM& rInitParam )
  {
    _Clear();

    GFL_ASSERT( rInitParam.pUtilHeap != NULL );
    GFL_ASSERT( rInitParam.pLytBinData != NULL );
    GFL_ASSERT( rInitParam.pMenuCursorLytBinData != NULL );
    GFL_ASSERT( rInitParam.pRenderManager != NULL );
    GFL_ASSERT( rInitParam.pAppMsgData != NULL );
    GFL_ASSERT( rInitParam.pWordSet != NULL );
    GFL_ASSERT( rInitParam.pTrainerIconRendering != NULL );
    GFL_ASSERT( rInitParam.pGrpFont != NULL );

    m_initParam = rInitParam;
  }

  UIViewPool::~UIViewPool()
  {
    m_pUppRootView->RemoveAllSubView();
    m_pLowRootView->RemoveAllSubView();

    GFL_SAFE_DELETE( m_pUppRootView );
    GFL_SAFE_DELETE( m_pUppGameInfoView );
    GFL_SAFE_DELETE( m_pUppGameResultView );

    GFL_SAFE_DELETE( m_pLowRootView );
    GFL_SAFE_DELETE( m_pLowDataCtrlView );
    GFL_SAFE_DELETE( m_pLowDataListView );

    GFL_SAFE_DELETE( m_pSelectButtonsView );
    GFL_SAFE_DELETE( m_pLowMessageMenuView );
    GFL_SAFE_DELETE( m_pMesssageMenuViewFileCtrl );
  }

  //------------------------------------------------------------------
  /**
    * @brief      初期化
    * @return     初期化完了: true / 初期化中: false
    */
  //------------------------------------------------------------------
  bool UIViewPool::InitFunc()
  {
    if( m_initSeq == 0 )
    {
      //  下画面のView作成
      {
        //  root
        {
          LowRootView::INIT_PARAM initParam;
          initParam.pAppMsgData     = m_initParam.pAppMsgData;
          initParam.pHeap           = m_initParam.pUtilHeap;
          initParam.pLytBinData     = m_initParam.pLytBinData;
          initParam.pRenderManager  = m_initParam.pRenderManager;
          initParam.pWordSet        = m_initParam.pWordSet;

          m_pLowRootView  = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) LowRootView( initParam );
        }

        //  選択のView
        {
          m_pSelectButtonsView = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) app::tool::GeneralLowerView( m_initParam.pUtilHeap, m_initParam.pRenderManager );
          //  デフォルトでＢボタン設定をしているのでクリアする。
          m_pSelectButtonsView->RegisterShortcut_B( app::tool::ButtonManager::BUTTON_ID_NULL );
        }

        //  dataCtrlView作成
        {
          LowDataCtrlView::INIT_PARAM initParam;
          initParam.pAppMsgData     = m_initParam.pAppMsgData;
          initParam.pHeap           = m_initParam.pUtilHeap;
          initParam.pLytBinData     = m_initParam.pLytBinData;
          initParam.pRenderManager  = m_initParam.pRenderManager;
          initParam.pWordSet        = m_initParam.pWordSet;
          initParam.pMenuCursorBinData  = m_initParam.pMenuCursorLytBinData;

          m_pLowDataCtrlView  = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) LowDataCtrlView( initParam );
        }

        //  dataListView作成
        {
          LowDataListView::INIT_PARAM initParam;
          initParam.pAppMsgData     = m_initParam.pAppMsgData;
          initParam.pHeap           = m_initParam.pUtilHeap;
          initParam.pLytBinData     = m_initParam.pLytBinData;
          initParam.pRenderManager  = m_initParam.pRenderManager;
          initParam.pWordSet        = m_initParam.pWordSet;
          initParam.pMenuCursorBinData  = m_initParam.pMenuCursorLytBinData;

          m_pLowDataListView  = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) LowDataListView( initParam );
        }

        //  MessageMenuView作成
        {
          App::Tool::MessageMenuView::PARAM param( m_initParam.pUtilHeap, m_initParam.pRenderManager );

          m_pMesssageMenuViewFileCtrl = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) app::util::AppUtilFileControlState( ARCID_MENU_WINDOW, GARC_MenuWindow_MenuWindow_applyt_COMP, m_initParam.pUtilHeap->GetDeviceHeap(), true, 128 );
          m_pMesssageMenuViewFileCtrl->Start();

          m_pLowMessageMenuView  = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) App::Tool::MessageMenuView( param );
        }
      }

      //  上画面のView作成
      {
        //  root
        {
          UppRootView::INIT_PARAM initParam;
          initParam.pAppMsgData     = m_initParam.pAppMsgData;
          initParam.pHeap           = m_initParam.pUtilHeap;
          initParam.pLytBinData     = m_initParam.pLytBinData;
          initParam.pRenderManager  = m_initParam.pRenderManager;
          initParam.pWordSet        = m_initParam.pWordSet;

          m_pUppRootView  = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) UppRootView( initParam );
        }

        //  gameInfo
        {
          UppGameInfoView::INIT_PARAM initParam;
          initParam.pAppMsgData     = m_initParam.pAppMsgData;
          initParam.pHeap           = m_initParam.pUtilHeap;
          initParam.pLytBinData     = m_initParam.pLytBinData;
          initParam.pRenderManager  = m_initParam.pRenderManager;
          initParam.pWordSet        = m_initParam.pWordSet;
          initParam.pTrainerIconRendering = m_initParam.pTrainerIconRendering;
          initParam.pPokeIconTexPool      = m_initParam.pPokeIconTexPool;

          //  トレーナーアイコンのid予約
          {
            //  左左
            initParam.aReservTrainerIconId[ UppGameInfoView::PLAYER_INFO_POS_LL ]  = 0;
            //  左右
            initParam.aReservTrainerIconId[ UppGameInfoView::PLAYER_INFO_POS_LR ]  = 1;

            //  右左
            initParam.aReservTrainerIconId[ UppGameInfoView::PLAYER_INFO_POS_RL ]  = 2;
            //  右右
            initParam.aReservTrainerIconId[ UppGameInfoView::PLAYER_INFO_POS_RR ]  = 3;
          }

          m_pUppGameInfoView  = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) UppGameInfoView( initParam );
        }

        //  gameResult
        {
          UppGameResultView::INIT_PARAM initParam;
          initParam.pAppMsgData     = m_initParam.pAppMsgData;
          initParam.pHeap           = m_initParam.pUtilHeap;
          initParam.pLytBinData     = m_initParam.pLytBinData;
          initParam.pRenderManager  = m_initParam.pRenderManager;
          initParam.pWordSet        = m_initParam.pWordSet;
          initParam.pTrainerIconRendering = m_initParam.pTrainerIconRendering;
          initParam.pGrpFont              = m_initParam.pGrpFont;
          initParam.pPokeIconTexPool      = m_initParam.pPokeIconTexPool;

          //  トレーナーアイコンのid予約
          {
            //  左
            initParam.aReservTrainerIconId[ UppGameResultView::PLAYER_INFO_POS_L ]  = 4;
            //  右
            initParam.aReservTrainerIconId[ UppGameResultView::PLAYER_INFO_POS_R ]  = 5;
          }

          m_pUppGameResultView  = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) UppGameResultView( initParam );
        }
      }

      ++m_initSeq;
    }
    else
    if( m_initSeq == 1 )
    {
      bool  bRet  = true;

      if( m_pLowRootView->InitFunc() == false )
      {
        bRet  = false;
      }

      if( m_pUppRootView->InitFunc() == false )
      {
        bRet  = false;
      }

      if( m_pSelectButtonsView->ObserveSetup() == false )
      {
        bRet  = false;
      }

      if( m_pLowDataCtrlView->InitFunc() == false )
      {
        bRet  = false;
      }

      if( m_pLowDataListView->InitFunc() == false )
      {
        bRet  = false;
      }

      if( m_pUppGameInfoView->InitFunc() == false )
      {
        bRet  = false;
      }

      if( m_pUppGameResultView->InitFunc() == false )
      {
        bRet  = false;
      }

      m_pMesssageMenuViewFileCtrl->Update();
      if( m_pMesssageMenuViewFileCtrl->IsEnd() == false ) 
      {
        bRet  = false;
      }

      if( bRet == true )
      {
        ++m_initSeq;
      }
    }
    else
    if( m_initSeq == 2 )
    {
      //  初回はＢボタンを非表示
      m_pLowRootView->SetVisiblePushBtn( LowRootView::BUTTON_ID_BACK, false );

      m_pLowMessageMenuView->Setup( m_pMesssageMenuViewFileCtrl->GetLoadDataBuffer(), m_initParam.pMenuCursorLytBinData );

      ++m_initSeq;
    }
    else
    if( m_initSeq == 3 )
    {
      bool  bRet  = true;
      if( m_pLowMessageMenuView->IsSetup() == false )
      {
        bRet  = false;
      }

      if( bRet == true )
      {
        ++m_initSeq;
      }
    }
    else
    {
      return  true;
    }

    return  false;
  }

  //------------------------------------------------------------------
  /**
    * @brief  終了
    * @return 終了完了: true / 終了中: false
    */
  //------------------------------------------------------------------
  bool UIViewPool::EndFunc()
  {
    if( m_endSeq == 0 )
    {
      bool  bRet  = true;

      if( m_pLowRootView->EndFunc() == false )
      {
        bRet  = false;
      }

      if( m_pUppRootView->EndFunc() == false )
      {
        bRet  = false;
      }

      if( m_pLowDataCtrlView->EndFunc() == false )
      {
        bRet  = false;
      }

      if( m_pLowDataListView->EndFunc() == false )
      {
        bRet  = false;
      }

      if( m_pUppGameInfoView->EndFunc() == false )
      {
        bRet  = false;
      }

      if( m_pUppGameResultView->EndFunc() == false )
      {
        bRet  = false;
      }

      if( bRet == true )
      {
        ++m_endSeq;
      }
    }
    else
    {
      return  true;
    }

    return  false;
  }

  //  上画面のルートViewにインフォ関連のViewを追加
  void UIViewPool::AddUppRootInfoView()
  {
    //  ゲーム情報viewを追加
    {
      m_pUppRootView->AddSubView( m_pUppGameInfoView );
    }

    //  ゲーム勝敗Viewを追加
    {
      m_pUppRootView->AddSubView( m_pUppGameResultView );
    }

    //  追加したので表示設定をする
    SetVisibleUppInfoView( true );
  }

  //  上画面のルートViewのインフォ関連のView解除
  void UIViewPool::RemoveUppRootInfoView()
  {
    m_pUppGameInfoView->RemoveFromSuperView();
    m_pUppGameResultView->RemoveFromSuperView();

    //  同時に非表示設定をする
    //  こうしないとアイコンロードアイコンが表示されるから
    SetVisibleUppInfoView( false );
  }

  //  上画面のインフォViewにビデオデータを設定
  void UIViewPool::SetVideoDataUppInfoView( NetAppLib::BattleVideoPlayer::VIDEO_DATA* pVideoData, bool bForceVideoData )
  {
    GFL_ASSERT( pVideoData );

    if( ( m_pVideoData != pVideoData ) || ( bForceVideoData == true ) )
    {
      m_pUppGameInfoView->SetVideoData( pVideoData );
      m_pUppGameResultView->SetVideoData( pVideoData );

      m_pVideoData  = pVideoData;
    }
  }

  //  情報状態設定
  void UIViewPool::SetUppInfoViewState( const UppInfoViewMode mode )
  {
    UppGameInfoView*  pUppGameInfoView      = m_pUppGameInfoView;
    UppGameResultView*  pUppGameResultView  = m_pUppGameResultView;

    //  プレイヤー情報を表示
    if( mode == UPP_INFO_VIEW_MODE_DETAIL )
    {
      pUppGameInfoView->SetDrawVisible( true );
      pUppGameResultView->SetDrawVisible( false );

      m_pLowDataListView->SetInfoMode( LowDataListView::INFO_MODE_DETAIL );
      m_pLowDataCtrlView->SetInfoMode( LowDataCtrlView::INFO_MODE_DETAIL );
    }
    else
    if( mode == UPP_INFO_VIEW_MODE_RESULT )
    {
      pUppGameInfoView->SetDrawVisible( false );
      pUppGameResultView->SetDrawVisible( true );

      m_pLowDataListView->SetInfoMode( LowDataListView::INFO_MODE_RESULT );
      m_pLowDataCtrlView->SetInfoMode( LowDataCtrlView::INFO_MODE_RESULT );
    }

    m_uppInfoViewMode = mode;
  }

  //  情報切り替え
  void UIViewPool::ChangeUppInfoViewState()
  {
    if( m_uppInfoViewMode == UPP_INFO_VIEW_MODE_DETAIL )
    {
      m_uppInfoViewMode = UPP_INFO_VIEW_MODE_RESULT;
    }
    else
    if( m_uppInfoViewMode == UPP_INFO_VIEW_MODE_RESULT )
    {
      m_uppInfoViewMode = UPP_INFO_VIEW_MODE_DETAIL;
    }

    SetUppInfoViewState( m_uppInfoViewMode );
  }

  //  上画面のインフォ表示設定
  void UIViewPool::SetVisibleUppInfoView( const bool bFlg )
  {
    if( bFlg == false )
    {
      //  バージョンエラーが表記しているときがあるので非表示にする
      m_pUppRootView->HideMessageCenter();

      m_pUppGameInfoView->SetDrawVisible( bFlg );
      m_pUppGameResultView->SetDrawVisible( bFlg );
    }
    else
    {
      SetUppInfoViewState( m_uppInfoViewMode );

      GFL_ASSERT( m_pVideoData );

      //  バージョンエラーで選択できない
      if( m_pVideoData->bErrorVersionFlg == 1 )
      {
        //  上画面のタイトルは非表示
        m_pUppRootView->HideMessageUpper();

        //  情報画面は非表示
        m_pUppGameInfoView->SetDrawVisible( false );
        m_pUppGameResultView->SetDrawVisible( false );

        //  上画面にバージョンエラーを表記
        {
          gfl2::str::StrBuf*  pMesssageStrBuf = m_pUppRootView->GetG2DUtil()->GetTempStrBuf( 0 );
          m_initParam.pAppMsgData->GetString( msg_bvp_sys_up_15, *pMesssageStrBuf );
          m_pUppRootView->ShowMessageCenter( pMesssageStrBuf );
        }
      }
      else
      {
        //  バージョンエラーが表記しているときがあるので非表示にする
        m_pUppRootView->HideMessageCenter();

        //  上画面の情報更新
        //  タイトル２がなければ一行のタイトル表示
        if( m_pVideoData->pTitleName02->GetLength() <= 0 )
        {
          m_pUppRootView->ShowMessageUpperOneLine( m_pVideoData->pTitleName01 );
        }
        else
        {
          m_pUppRootView->ShowMessageUpperTwoLine( m_pVideoData->pTitleName01, m_pVideoData->pTitleName02 );
        }
      }
    }
  }

  //  上画面のインフォのアップロード情報を更新
  void UIViewPool::UpdateInfoUploadData()
  {
    m_pUppGameInfoView->UpdateVideoDataUpload( m_pVideoData );
  }

  //  下画面のシステムメッセージ表示
  void UIViewPool::ShowLowerSystemMessage( gfl2::str::MsgData* pMsgData, const u32 strId, const bool bImmediateMessage, const bool bShowTimerIcon, const bool bShowBG )
  {
    if( pMsgData == NULL )
    {
      GFL_ASSERT( 0 );
      return;
    }

    App::Tool::MessageMenuView* pMessageMenuView  = m_pLowMessageMenuView;

    pMessageMenuView->SetYNListMode( false );
    pMessageMenuView->SetFinishMode_None();

    pMessageMenuView->ShowMessage( *pMsgData, strId, bImmediateMessage, bShowTimerIcon, bShowBG );
    //  メッセージ表示だけなのでモーダル設定は不要
    pMessageMenuView->SetModal( false );
    pMessageMenuView->SetEventHandler( this );

    m_lowerSystemMessageEventCode = App::Tool::MessageMenuView::IEventHandler::EVENT_;

    m_pLowRootView->AddSubView( pMessageMenuView );
  }

  //------------------------------------------------------------------
  /**
    *  @func     メッセージはStrBuf*だけどメニューリストはid指定版
    *  @brief    
    */
  //------------------------------------------------------------------
  void  UIViewPool::ShowMessageWithListMenu(
    /*  ウインドウ設定  */
    const gfl2::str::StrBuf*                      pWindowMessage,
    const bool                                    isImmediateMessage,
    const bool                                    bYesNoMode,
    /*  リスト設定  */
    const gfl2::str::MsgData&                     rListMessageData,
    const u32                                     listMessageID_0,
    const u32                                     listMessageID_1,
    const u32                                     listMessageID_2,
    const u32                                     listMessageID_3,
    const u32                                     listMessageID_4,
    const u32                                     listMessageID_5
  )
  {
    if( pWindowMessage == NULL )
    {
      GFL_ASSERT( 0 );
      return;
    }

    App::Tool::MessageMenuView* pMessageMenuView  = m_pLowMessageMenuView;

    pMessageMenuView->SetYNListMode( bYesNoMode );
    pMessageMenuView->SelectMenu( 0 );
    pMessageMenuView->SetModal( true );
    pMessageMenuView->SetInputEnabledAllView( true );

    pMessageMenuView->ShowMessageWithListMenu(
      /*  ウインドウ設定  */
      pWindowMessage,
      isImmediateMessage,

      /*  リスト設定  */
      rListMessageData,
      listMessageID_0,
      listMessageID_1,
      listMessageID_2,
      listMessageID_3,
      listMessageID_4,
      listMessageID_5
    );

    pMessageMenuView->SetEventHandler( this );

    m_pLowRootView->AddSubView( pMessageMenuView );

    m_lowerSystemMessageEventCode = App::Tool::MessageMenuView::IEventHandler::EVENT_;
  }

  //------------------------------------------------------------------
  /**
    *  @func     メッセージはStrBuf*メニューリストのStrBuf*指定版
    *  @brief
    *  @note     他人のバトルビデオデータの選択
    */
  //------------------------------------------------------------------
  bool  UIViewPool::ShowMessageWithListMenuByDual( const NetAppLib::BattleVideoPlayer::VIDEO_DATA* pVideoData )
  {
    if( pVideoData == NULL )
    {
      GFL_ASSERT( 0 );
      return  false;
    }
    else
    if( pVideoData->bServerDataFlg == 0 )
    {
      //  他人のデータのみ選択可能
      GFL_ASSERT( 0 );
      return  false;
    }

    gfl2::str::StrBuf*  pTitleStrBuf  = m_pLowRootView->GetG2DUtil()->GetTempStrBuf( 0 );
    //  タイトル名作成
    {
      m_initParam.pAppMsgData->GetString( msg_bvp_win_24, *pTitleStrBuf );
    }

    //  項目１作成
    gfl2::str::StrBuf*  pItem00StrBuf  = m_pLowRootView->GetG2DUtil()->GetTempStrBuf( 1 );
    {
      pItem00StrBuf->SetStr( &pVideoData->sdCardData.upload.header.name[ 0 ][ 0 ] );
    }

    //  項目２作成
    gfl2::str::StrBuf*  pItem01StrBuf  = m_pLowDataCtrlView->GetG2DUtil()->GetTempStrBuf( 0 );
    {
      // プレイヤー名
      pItem01StrBuf->SetStr( &pVideoData->sdCardData.upload.header.name[ 1 ][ 0 ] );
    }

    //  項目３作成(やめる)
    gfl2::str::StrBuf*  pItem02StrBuf  = m_pLowDataCtrlView->GetG2DUtil()->GetTempStrBuf( 1 );
    {
      m_initParam.pAppMsgData->GetString( msg_bvp_sys_low_20, *pItem02StrBuf );
    }

    App::Tool::MessageMenuView* pMessageMenuView  = m_pLowMessageMenuView;

    pMessageMenuView->SetYNListMode( true );
    pMessageMenuView->SelectMenu( 0 );
    pMessageMenuView->SetModal( true );
    pMessageMenuView->SetInputEnabledAllView( true );

    pMessageMenuView->AddItem( *pItem00StrBuf, false, false, 0 );
    pMessageMenuView->AddItem( *pItem01StrBuf, false, false, 0 );
    pMessageMenuView->AddItem( *pItem02StrBuf, true, false, 0 );

    pMessageMenuView->ShowMessageWithListMenu(
      /*  ウインドウ設定  */
      pTitleStrBuf,
      false,

      /*  リスト設定  */
      NULL,
      NULL,
      NULL
    );

    pMessageMenuView->SetEventHandler( this );

    m_pLowRootView->AddSubView( pMessageMenuView );

    m_lowerSystemMessageEventCode = App::Tool::MessageMenuView::IEventHandler::EVENT_;

    return  true;
  }

  //  下画面のメニューウィンドウで押したボタンidxを取得
  u32 UIViewPool::GetLowerSystemMessageMenuSelectIdx()
  {
    App::Tool::MessageMenuView* pMessageMenuView  = m_pLowMessageMenuView;
    return  pMessageMenuView->GetSelectedItemIndex();
  }

  //  下画面のシステムメッセージ非表示
  void UIViewPool::HideLowerSystemMessage()
  {
    App::Tool::MessageMenuView* pMessageMenuView  = m_pLowMessageMenuView;
    pMessageMenuView->SetEventHandler( NULL );
    pMessageMenuView->RemoveFromSuperView();
  }

  //  下画面のシステムメッセージが表示か
  bool UIViewPool::IsEnableLowerSystemMessage()
  {
    App::Tool::MessageMenuView* pMessageMenuView  = m_pLowMessageMenuView;

    return  m_pLowRootView->IsExistSubView( pMessageMenuView );
  }

  //  下画面のシステムメッセージ処理が終了しているか
  bool UIViewPool::IsEndProcLowerSystemMessage()
  {
    if( m_lowerSystemMessageEventCode != App::Tool::MessageMenuView::IEventHandler::EVENT_ )
    {
      return  true;
    }

    return  false;
  }

  //  MessageMenuWindowのイベント
  void  UIViewPool::MessageMenuView_OnEvent(const EventCode event)
  {
    m_lowerSystemMessageEventCode = event;
  }

GFL_NAMESPACE_END(BattleVideoPlayer)
GFL_NAMESPACE_END(NetApp)
