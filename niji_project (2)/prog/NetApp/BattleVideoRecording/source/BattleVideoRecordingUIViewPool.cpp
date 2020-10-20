//==============================================================================
/**
 @file    BattleVideoRecordingUIViewPool.cpp
 @date    2015.09.02
 */
//==============================================================================
#include "NetApp/BattleVideoRecording/source/BattleVideoRecordingUIViewPool.h"

//  nijiのインクルード
#include "System/include/HeapDefine.h"
#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Tool/app_tool_2ButtonsView.h"
#include "AppLib/include/Util/AppUtilFileControlState.h"

//  リソースのインクルード
#include "arc_def_index/arc_def.h"
#include "arc_index/MenuWindow.gaix"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideoRecording)

  UIViewPool::UIViewPool( const struct INIT_PARAM& rInitParam )
  {
    _Clear();

    GFL_ASSERT( rInitParam.pUtilHeap != NULL );
    GFL_ASSERT( rInitParam.pAppMsgData != NULL );
    GFL_ASSERT( rInitParam.pWordSet != NULL );

    m_initParam = rInitParam;
  }

  UIViewPool::~UIViewPool()
  {
    GFL_SAFE_DELETE( m_pUppRootView );
    GFL_SAFE_DELETE( m_pLowRootView );
    GFL_SAFE_DELETE( m_pButtonsView );
    GFL_SAFE_DELETE( m_pLowMessageMenuView );
    GFL_SAFE_DELETE( m_pMesssageMenuViewFileCtrl );
  }

  //------------------------------------------------------------------
  /**
    * @brief  初期化
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

        //  ２択のView
        {
          m_pButtonsView = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) app::tool::GeneralLowerView( m_initParam.pUtilHeap, m_initParam.pRenderManager );
          //  BUTTON_1が「いいえ」なのでＢボタン押して選択できるように
          m_pButtonsView->RegisterShortcut_B( app::tool::GeneralLowerView::BUTTON_1 );
        }

        //  MessageMenuView作成
        {
          App::Tool::MessageMenuView::PARAM param( m_initParam.pUtilHeap, m_initParam.pRenderManager );

          m_pMesssageMenuViewFileCtrl = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) app::util::AppUtilFileControlState( ARCID_MENU_WINDOW, GARC_MenuWindow_MenuWindow_applyt_COMP, m_initParam.pUtilHeap->GetDeviceHeap(), true, 128 );
          m_pMesssageMenuViewFileCtrl->Start();

          m_pLowMessageMenuView  = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) App::Tool::MessageMenuView( param );

          m_pLowMessageMenuView->SetEventHandler( this );
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

      if( m_pButtonsView->ObserveSetup() == false )
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
      m_pButtonsView->SetLayoutMode( app::tool::GeneralLowerView::LAYOUTMODE_2Buttons, false, false );
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

      if( bRet == true )
      {
        m_pLowMessageMenuView->SetEventHandler( NULL );
        ++m_endSeq;
      }
    }
    else
    {
      return  true;
    }

    return  false;
  }

  //  メッセージ表示開始
  App::Tool::MessageMenuView* UIViewPool::ShowSystemMessageLower( gfl2::str::MsgData* pMsgData, const u32 strId, app::ui::UIView* pAttachUIView, const bool bImmediateMessage, const bool bUserInput, const bool bShowTimerIcon, const bool bShowBG )
  {
    if( pMsgData == NULL )
    {
      GFL_ASSERT( 0 );
      return  NULL;
    }

    if( pAttachUIView == NULL )
    {
      GFL_ASSERT( 0 );
      return  NULL;
    }

    m_bMessageUserInput = bUserInput;
    m_bMessegeShow      = true;

    m_pLowMessageMenuView->SetYNListMode( false );

    if( m_bMessageUserInput == true )
    {
      m_pLowMessageMenuView->SetFinishMode_UserInput();
    }
    else
    {
      m_pLowMessageMenuView->SetFinishMode_None();
    }

    m_pLowMessageMenuView->ShowMessage( *pMsgData, strId, bImmediateMessage, bShowTimerIcon, bShowBG );
    m_pLowMessageMenuView->SetModal( true );

    pAttachUIView->AddSubView( m_pLowMessageMenuView );

    return m_pLowMessageMenuView;
  }

  void  UIViewPool::MessageMenuView_OnEvent( const App::Tool::MessageMenuView::IEventHandler::EventCode event )
  {
    if( m_bMessageUserInput == true )
    {
      if( event == App::Tool::MessageMenuView::IEventHandler::EVENT_OnFinish )
      {
        m_bMessegeShow  = false;
      }
    }
    else
    {
      if( event == App::Tool::MessageMenuView::IEventHandler::EVENT_OnEOM )
      {
        //  表示するメッセージに▼がある場合には終端として使える
        m_bMessegeShow  = false;
      }
    }
  }

GFL_NAMESPACE_END(BattleVideoRecording)
GFL_NAMESPACE_END(NetApp)
