//==============================================================================
/**
 * @file   app_tool_MessageMenuView.cpp
 * @date   2015/07/03 Fri. 16:37:46
 * @author muto_yu
 * @brief  汎用メッセージウインドウ&リストメニュー
 * @note   NetAppLibより移動
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "AppLib/include/Tool/app_tool_MenuWindow.h"
#include  "AppLib/include/Tool/app_tool_MessageMenuView.h"


// =============================================================================
/**/
//==============================================================================





namespace App {
namespace Tool  {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     MessageMenuView
  *  @brief    ctor
  */
//------------------------------------------------------------------
MessageMenuView::MessageMenuView(const PARAM& rParam, IEventHandler* pHandler)
  : app::ui::UIView(rParam.pHeap)
  , m_Param(rParam)
  , m_pHandler(pHandler)
  /*  メッセージwindow  */
  , m_pMessageWindowView(NULL)
  /*  汎用メニュー  */
  , m_pCommonMenu(NULL)
  , m_SelectedIndex(0)
  , m_YNListNegativeIndex(0)
  , m_YNListMode(false)
  , m_IsNeedShowListMenu(false)
  , m_IsShowListOnFinishEvent(false)
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    dtor
  */
//------------------------------------------------------------------
MessageMenuView::~MessageMenuView()
{
  /*  remove  */
  {
    if(m_pCommonMenu)
    {
      m_pCommonMenu->RemoveFromSuperView();
    }
    if(m_pMessageWindowView)
    {
      m_pMessageWindowView->RemoveFromSuperView();
    }
  }

  GFL_SAFE_DELETE(m_pCommonMenu);
  GFL_SAFE_DELETE(m_pMessageWindowView);

  /*  不要  */
//  Delete2D();
//  DeleteGraphicsSystem();
}


//------------------------------------------------------------------
/**
  *  @func     Setup
  *  @brief    使用準備
  *  @param   [in]  pLayoutBuffer   layoutリソースデータ
  */
//------------------------------------------------------------------
void  MessageMenuView::Setup(void* pLayoutResourceData)
{
  app::util::Heap*  pHeap = m_Param.pHeap;


  /*
      this(EmptyView)
        |
        +- MessageWindow
            |
            +- CommonMenu
  */


  /*  メッセージウインドウ  */
  {
    m_pMessageWindowView  = CreateCommonMessageWindowView(CommonMessageWindowView::PARAM(m_Param.pHeap, m_Param.pRenderingManager), this);
    this->AddSubView(m_pMessageWindowView);
  }

  /*  Menu  */
  {
    m_pCommonMenu     = GFL_NEW(pHeap->GetDeviceHeap()) app::tool::MenuWindow(pHeap, m_Param.pRenderingManager);
    m_pCommonMenu->SetListener(this);

    m_pMessageWindowView->AddSubView(m_pCommonMenu);
  }

  SetModal(false);
  m_pCommonMenu->SetModal(false);

  m_pMessageWindowView->Setup(pLayoutResourceData);
}


void  MessageMenuView::Setup(void* pLayoutResourceData, void* pCursorResourceData)
{
  app::util::Heap*  pHeap = m_Param.pHeap;


  /*
      this(EmptyView)
        |
        +- MessageWindow
            |
            +- CommonMenu
  */


  /*  メッセージウインドウ  */
  {
    m_pMessageWindowView  = CreateCommonMessageWindowView(CommonMessageWindowView::PARAM(m_Param.pHeap, m_Param.pRenderingManager), this);
    this->AddSubView(m_pMessageWindowView);
  }

  /*  Menu  */
  {
    m_pCommonMenu     = GFL_NEW(pHeap->GetDeviceHeap()) app::tool::MenuWindow(pHeap, m_Param.pRenderingManager, pCursorResourceData, app::util::G2DUtil::SETUP_LOWER, pLayoutResourceData);
    m_pCommonMenu->SetListener(this);

    m_pMessageWindowView->AddSubView(m_pCommonMenu);
  }

  SetModal(false);
  m_pCommonMenu->SetModal(false);

  m_pMessageWindowView->Setup(pLayoutResourceData);
}


//------------------------------------------------------------------
/**
  *  @func     IsSetup
  *  @brief    使用準備が完了したか
  *  @retval   true:完了
  *  @retval   false:未完了
  */
//------------------------------------------------------------------
bool  MessageMenuView::IsSetup()
{
  return m_pCommonMenu->IsCreate();
}


//------------------------------------------------------------------
/**
  *  @func     SetYNListMode
  *  @brief    ListMenuをYesNoモードで表示するか
  *            trueに設定しておくと、ShowMessageWithListMenuでYes/No選択専用モードで表示され、EventHandlerでは...Selected{Yes|No}が通知される
  *  @param    [in] negativeIndex   否定系として扱う選択肢番号（{0|1}）、Bボタンでのショートカットに割り当てたりする
  */
//------------------------------------------------------------------
void  MessageMenuView::SetYNListMode(const bool isEnable, const u32 negativeIndex)
{
  m_YNListMode = isEnable;
  m_YNListNegativeIndex = negativeIndex % 2;
}


//------------------------------------------------------------------
/**
  *  @func     SetMessageSpeed
  *  @brief    メッセージスピードを設定する
  */
//------------------------------------------------------------------
void  MessageMenuView::SetMessageSpeed(Savedata::ConfigSave::MSG_SPEED spd)
{
  m_pMessageWindowView->SetMessageSpeed(spd);
}


//------------------------------------------------------------------
/**
  *  @brief    メッセージ時の自動送りフラグを設定
  */
//------------------------------------------------------------------
void MessageMenuView::SetMessageAutoFlg( const bool bFlg )
{
  m_pMessageWindowView->GetG2DUtil()->GetMsgWin()->SetMessageAutoFlg( bFlg );
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  MessageMenuView::ShowMessage(const gfl2::str::MsgData& rWindowMessageData, const u32 messageID, const bool isImmediateMessage, const bool isShowTimerIcon, const bool isShowBG)
{
  m_pMessageWindowView->App::Tool::CommonMessageWindowView::ShowMessage(rWindowMessageData, messageID, isImmediateMessage, isShowTimerIcon, isShowBG);
  this->resetMessageWindowTextBoxPaneAlpha();  // @fix NMCat[3868] テキストの一部が表示されなくなることがある  →テキストペインのアルファが変更されている最中にメッセージを変えることもあるので、ここで元に戻しておく。
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  MessageMenuView::ShowMessage(const gfl2::str::StrBuf* pStrBuf, const bool isImmediateMessage, const bool isShowTimerIcon, const bool isShowBG)
{
  m_pMessageWindowView->App::Tool::CommonMessageWindowView::ShowMessage(pStrBuf, isImmediateMessage, isShowTimerIcon, isShowBG);
  this->resetMessageWindowTextBoxPaneAlpha();  // @fix NMCat[3868] テキストの一部が表示されなくなることがある  →テキストペインのアルファが変更されている最中にメッセージを変えることもあるので、ここで元に戻しておく。
  
  m_IsNeedShowListMenu  = false;
  SetModal(isShowBG);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @note    要messageUtil
  */
//------------------------------------------------------------------
void  MessageMenuView::ShowMessageWithListMenu(
  const gfl2::str::MsgData&   rWindowMessageData,
  const u32                   windowMessageID,
  const bool                  isImmediateMessage,
  /*  リスト設定  */
  const gfl2::str::MsgData&   rListMessageData,
  const u32                   listMessageID_0,
  const u32                   listMessageID_1,
  const u32                   listMessageID_2,
  const u32                   listMessageID_3,
  const u32                   listMessageID_4,
  const u32                   listMessageID_5
)
{
  gfl2::heap::HeapBase*   pHeapBase               = m_Param.pHeap->GetDeviceHeap();
  const u32               maxLength               = 256;
  const u32               listMessageResourceNum  = rListMessageData.GetStrCount();
  gfl2::str::MsgData&     rListMessageDataNotConst  = const_cast<gfl2::str::MsgData&>(rListMessageData);
  gfl2::str::StrBuf*      pMessageStrBuf  = GFL_NEW(pHeapBase) gfl2::str::StrBuf(maxLength, pHeapBase);
  gfl2::str::StrBuf*      pStrBufs[]  = {
                            ((listMessageID_0 < listMessageResourceNum) ? (GFL_NEW(pHeapBase) gfl2::str::StrBuf(maxLength, pHeapBase)) : NULL),
                            ((listMessageID_1 < listMessageResourceNum) ? (GFL_NEW(pHeapBase) gfl2::str::StrBuf(maxLength, pHeapBase)) : NULL),
                            ((listMessageID_2 < listMessageResourceNum) ? (GFL_NEW(pHeapBase) gfl2::str::StrBuf(maxLength, pHeapBase)) : NULL),
                            ((listMessageID_3 < listMessageResourceNum) ? (GFL_NEW(pHeapBase) gfl2::str::StrBuf(maxLength, pHeapBase)) : NULL),
                            ((listMessageID_4 < listMessageResourceNum) ? (GFL_NEW(pHeapBase) gfl2::str::StrBuf(maxLength, pHeapBase)) : NULL),
                            ((listMessageID_5 < listMessageResourceNum) ? (GFL_NEW(pHeapBase) gfl2::str::StrBuf(maxLength, pHeapBase)) : NULL),
                          };

  rListMessageDataNotConst.GetString(windowMessageID, *pMessageStrBuf);
  if(pStrBufs[0]) rListMessageDataNotConst.GetString( listMessageID_0, *pStrBufs[0] );
  if(pStrBufs[1]) rListMessageDataNotConst.GetString( listMessageID_1, *pStrBufs[1] );
  if(pStrBufs[2]) rListMessageDataNotConst.GetString( listMessageID_2, *pStrBufs[2] );
  if(pStrBufs[3]) rListMessageDataNotConst.GetString( listMessageID_3, *pStrBufs[3] );
  if(pStrBufs[4]) rListMessageDataNotConst.GetString( listMessageID_4, *pStrBufs[4] );
  if(pStrBufs[5]) rListMessageDataNotConst.GetString( listMessageID_5, *pStrBufs[5] );

  ShowMessageWithListMenu(
    pMessageStrBuf,
    isImmediateMessage,
    pStrBufs[0],
    pStrBufs[1],
    pStrBufs[2],
    pStrBufs[3],
    pStrBufs[4],
    pStrBufs[5]
  );

  for(int num = 0; num < GFL_NELEMS(pStrBufs); ++num)
  {
    GFL_SAFE_DELETE(pStrBufs[num]);
  }
  GFL_SAFE_DELETE(pMessageStrBuf);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  MessageMenuView::ShowMessageWithListMenu(
  /*  ウインドウ設定  */
  const gfl2::str::StrBuf*  pWindowMessage,
  const bool                isImmediateMessage,
  /*  リスト設定  */
  const gfl2::str::StrBuf*  pListMessage_0,
  const gfl2::str::StrBuf*  pListMessage_1,
  const gfl2::str::StrBuf*  pListMessage_2,
  const gfl2::str::StrBuf*  pListMessage_3,
  const gfl2::str::StrBuf*  pListMessage_4,
  const gfl2::str::StrBuf*  pListMessage_5
)
{
  {
    if(m_pCommonMenu->GetMenuItemNum())
    {
      /*  既に選択肢が存在する  */
      if(
        (!pListMessage_0)
        &&  (!pListMessage_1)
        &&  (!pListMessage_2)
        &&  (!pListMessage_3)
        &&  (!pListMessage_4)
        &&  (!pListMessage_5)
      )
      {
        /*  外部で直接AddItemしているとみなし、チェック対象外  */
      }
      else
      {
        /*  AddItemリークが疑われる  */
        GFL_ASSERT_MSG(false, "選択肢が残っている状態でShowMessageWithListMenuがコールされました。\nこれより以前のShowMessageWithListMenuなどで、AddItemした選択肢がクリアされていません。\n選択肢を強制的にクリアします。\n");
        ClearMenu();
      }
    }
  }
  if(pListMessage_0)  m_pCommonMenu->AddItem(*pListMessage_0, (m_YNListMode && (m_YNListNegativeIndex == 0)));
  if(pListMessage_1)  m_pCommonMenu->AddItem(*pListMessage_1, (m_YNListMode && (m_YNListNegativeIndex == 1)));
  if(pListMessage_2)  m_pCommonMenu->AddItem(*pListMessage_2);
  if(pListMessage_3)  m_pCommonMenu->AddItem(*pListMessage_3);
  if(pListMessage_4)  m_pCommonMenu->AddItem(*pListMessage_4);
  if(pListMessage_5)  m_pCommonMenu->AddItem(*pListMessage_5);

  ShowMessage(pWindowMessage, isImmediateMessage, false, true);
  /*  listmenu自体はメッセージ表示後に  */
  m_IsNeedShowListMenu  = true;
}

//------------------------------------------------------------------
/**
  *  @func     メッセージはStrBuf*だけどメニューリストはid指定版
  *  @brief    
  */
//------------------------------------------------------------------
void  MessageMenuView::ShowMessageWithListMenu(
  /*  ウインドウ設定  */
  const gfl2::str::StrBuf*                      pWindowMessage,
  const bool                                    isImmediateMessage,
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
  gfl2::heap::HeapBase*   pHeapBase               = m_Param.pHeap->GetDeviceHeap();
  const u32               maxLength               = 256;
  const u32               listMessageResourceNum  = rListMessageData.GetStrCount();
  gfl2::str::MsgData&     rListMessageDataNotConst  = const_cast<gfl2::str::MsgData&>(rListMessageData);
  gfl2::str::StrBuf*      pStrBufs[]  = {
                            ((listMessageID_0 < listMessageResourceNum) ? (GFL_NEW(pHeapBase) gfl2::str::StrBuf(maxLength, pHeapBase)) : NULL),
                            ((listMessageID_1 < listMessageResourceNum) ? (GFL_NEW(pHeapBase) gfl2::str::StrBuf(maxLength, pHeapBase)) : NULL),
                            ((listMessageID_2 < listMessageResourceNum) ? (GFL_NEW(pHeapBase) gfl2::str::StrBuf(maxLength, pHeapBase)) : NULL),
                            ((listMessageID_3 < listMessageResourceNum) ? (GFL_NEW(pHeapBase) gfl2::str::StrBuf(maxLength, pHeapBase)) : NULL),
                            ((listMessageID_4 < listMessageResourceNum) ? (GFL_NEW(pHeapBase) gfl2::str::StrBuf(maxLength, pHeapBase)) : NULL),
                            ((listMessageID_5 < listMessageResourceNum) ? (GFL_NEW(pHeapBase) gfl2::str::StrBuf(maxLength, pHeapBase)) : NULL),
                          };

  if(pStrBufs[0]) rListMessageDataNotConst.GetString( listMessageID_0, *pStrBufs[0] );
  if(pStrBufs[1]) rListMessageDataNotConst.GetString( listMessageID_1, *pStrBufs[1] );
  if(pStrBufs[2]) rListMessageDataNotConst.GetString( listMessageID_2, *pStrBufs[2] );
  if(pStrBufs[3]) rListMessageDataNotConst.GetString( listMessageID_3, *pStrBufs[3] );
  if(pStrBufs[4]) rListMessageDataNotConst.GetString( listMessageID_4, *pStrBufs[4] );
  if(pStrBufs[5]) rListMessageDataNotConst.GetString( listMessageID_5, *pStrBufs[5] );

  ShowMessageWithListMenu(
    pWindowMessage,
    isImmediateMessage,
    pStrBufs[0],
    pStrBufs[1],
    pStrBufs[2],
    pStrBufs[3],
    pStrBufs[4],
    pStrBufs[5]
  );

  for(int num = 0; num < GFL_NELEMS(pStrBufs); ++num)
  {
    GFL_SAFE_DELETE(pStrBufs[num]);
  }
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  MessageMenuView::ShowMessageWithListMenu(
  /*  ウインドウ設定  */
  const gfl2::str::StrBuf*                      pWindowMessage,
  const bool                                    isImmediateMessage,
  IAddItemHandler*                              pAddItemHandler
)
{
  if( pAddItemHandler )
  {
    pAddItemHandler->MessageMenuView_OnAddItem( m_pCommonMenu );
  }

  ShowMessage(pWindowMessage, isImmediateMessage, false, true);
  /*  listmenu自体はメッセージ表示後に  */
  m_IsNeedShowListMenu  = true;
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  MessageMenuView::HideMessage(void)
{
  m_pMessageWindowView->HideMessage();
  ClearMenu();

  SetModal(false);
  m_pCommonMenu->SetModal(false);
  m_pCommonMenu->SetInputEnabled(false);
}




/*  implement UIView I/F  */
//void  MessageMenuView::Update(void)
//{
//}

//void  MessageMenuView::Draw(void)
//{
//	app::util::AppRenderingManager* pAppRenderingManager  = m_pWork->GetAppRenderingManager();
//
//
//  GetG2DUtil()->AddDrawableLytWk( m_pWork->GetAppRenderingManager(), LAYOUTWORK_MessageWindow );
//}
/*  end implement UIView I/F  */




/*
  implement MenuWindowListener I/F
*/
//------------------------------------------------------------------
/**
  *  @func     SelectMenu
  *  @brief    ListMenuで項目が選択された
  */
//------------------------------------------------------------------
void  MessageMenuView::SelectMenu(u32 selectedIndex)
{
  GFL_PRINT("NetAppMessageWindow::SelectMenu: %d\n", selectedIndex);

  HideMessage();

  m_SelectedIndex = selectedIndex;
  if(m_pHandler)
  {
    if(m_YNListMode)
    {
      m_pHandler->MessageMenuView_OnEvent((selectedIndex == 0) ? IEventHandler::EVENT_OnSelectedYes : IEventHandler::EVENT_OnSelectedNo);
    }
    else
    {
      m_pHandler->MessageMenuView_OnEvent(IEventHandler::EVENT_OnSelectedItem);
    }
  }
}
/*
  end implement MenuWindowListener I/F
*/

//------------------------------------------------------------------
/**
  *  @func     選択項目の追加設定
  *  @param[in]   pMsg           メッセージデータ
  *  @param[in]   id            文字列ID
  *  @param[in]   isCancel     true = Bボタンを割り当てる
  *  @param[in]   isMark       true = 戻るマークを表示する
  *  @param[in]   selectSeId  選択時のSEID
  *  @note
  *            ShowMessageWithListMenu()関数ではAddItem()での設定が自由にできないので使う側が個別で設定できるように関数を用意
  */
//------------------------------------------------------------------
void MessageMenuView::AddItem( gfl2::str::MsgData* pMsg, u32 id, bool isCancel, bool isMark, u32 selectSeId )
{
  if( pMsg == NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }

  m_pCommonMenu->AddItem( pMsg, id, isCancel, isMark, selectSeId );
}

void MessageMenuView::AddItem( gfl2::str::StrBuf& rStrBuf, bool isCancel, bool isMark, u32 selectSeId )
{
  m_pCommonMenu->AddItem( rStrBuf, isCancel, isMark, selectSeId );
}

//------------------------------------------------------------------
/**
  *  @func     選択項目の追加設定
  *  @param[in]   str          文字列
  *  @param[in]   isCancel     true = Bボタンを割り当てる
  *  @param[in]   isMark       true = 戻るマークを表示する
  *  @param[in]   selectSeId  選択時のSEID(0だとデフォルトのSEになる)
  *  @note
  *            ShowMessageWithListMenu()関数ではAddItem()での設定が自由にできないので使う側が個別で設定できるように関数を用意
  */
//------------------------------------------------------------------
void MessageMenuView::AddItem( const gfl2::str::StrBuf & str, bool isCancel, bool isMark, u32 selectSeId )
{
  m_pCommonMenu->AddItem( str, isCancel, isMark, selectSeId );
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  MessageMenuView::CommonMessageWindowView_OnEvent(const CommonMessageWindowView::IEventHandler::EventCode event)
{
  switch(event)
  {
  case  CommonMessageWindowView::IEventHandler::EVENT_OnEOM:
      if(!m_IsShowListOnFinishEvent && m_IsNeedShowListMenu)
      {
        ShowListMenu();
        m_IsNeedShowListMenu  = false;
      }
      if(m_pHandler)
      {
        m_pHandler->MessageMenuView_OnEvent(IEventHandler::EVENT_OnEOM);
      }
      break;

    case  CommonMessageWindowView::IEventHandler::EVENT_OnFinish:
    {
      if(m_IsNeedShowListMenu)
      {
        ShowListMenu();
        m_IsNeedShowListMenu  = false;
      }
      if(m_pHandler)
      {
        m_pHandler->MessageMenuView_OnEvent(IEventHandler::EVENT_OnFinish);
      }
    }
      break;


  }
}

/* -------------------------------------------------------------------------*/
/**
 * @brief キー入力可否設定
 * @note  trueで一時キー入力無効
 */
/* -------------------------------------------------------------------------*/
void MessageMenuView::SetSuspendKey( bool flag )
{
  m_pMessageWindowView->SetSuspendKey( flag );
}

//------------------------------------------------------------------
/**
  *  @brief     CommonMessageWindowViewを生成する
  *  @note      OverrideすることでメッセージウインドウをCommonMessageWindowViewを継承したクラスに変更し、別のリソースに対応することなどが可能
  */
//------------------------------------------------------------------
CommonMessageWindowView*  MessageMenuView::CreateCommonMessageWindowView(const CommonMessageWindowView::PARAM& rParam, CommonMessageWindowView::IEventHandler* pHandler)
{
  app::util::Heap*  pHeap = rParam.pHeap;

  return GFL_NEW(pHeap->GetDeviceHeap()) CommonMessageWindowView(rParam, pHandler);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  MessageMenuView::ShowListMenu(const u32 defaultSelectedIndex)
{
  m_pCommonMenu->SetInputEnabled(true);
  if(m_YNListMode)
  {
    m_pCommonMenu->StartYesNoMenu(defaultSelectedIndex);
  }else
  {
    m_pCommonMenu->StartMenu(defaultSelectedIndex);
  }

  m_pCommonMenu->SetModal(true);
}


/* -------------------------------------------------------------------------*/
/**
 * @brief  テキストボックスペインのアルファをリセット(不透明に)する
 */
/* -------------------------------------------------------------------------*/
void MessageMenuView::resetMessageWindowTextBoxPaneAlpha(void)
{
  // @fix NMCat[3868] テキストの一部が表示されなくなることがある  →テキストペインのアルファを元に戻す
  if(m_pMessageWindowView)
  {
    app::util::G2DUtil* pG2DUtil = m_pMessageWindowView->GetMessageWindowManipulatorG2DUtil();
    if(pG2DUtil)
    {
      print::MessageWindow* pMsgWin = pG2DUtil->GetMsgWin();
      if(pMsgWin)
      {
        for(u8 i=0; i<App::Tool::CommonMessageWindowView::MESSAGE_WINDOW_TEXT_BOX_PANE_NUM; ++i)
        {
          pMsgWin->ResetTextBoxPaneAlpha(i);
        }
      }
    }
  }
}



//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  MessageMenuView::ClearMenu(void)
{
  if(m_pCommonMenu)
  {
    if(m_pCommonMenu->GetMenuItemNum())
    {
      m_pCommonMenu->StartYesNoMenu(0);   /*  未startだとEndMenuがSkipされるので、強制的にCall  ※2重コールは関数途中で抜ける ※Y/N時の座標Assert回避のため、Y/N系でStartする  */
      m_pCommonMenu->EndMenu(true);       /*  未StartでなければCall可能                                                        */
    }
  }
  
  /*  m_IsShowListOnFinishEvent はSetFinishModeで設定されるため、副作用を避けるためここではクリアしない  */
  m_IsNeedShowListMenu      = false;
}





} /*  namespace Tool  */
} /*  namespace App   */
