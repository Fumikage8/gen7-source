//==============================================================================
/**
 * @file   CommonMessageWindowManipulator.cpp
 * @date   2015/07/03 Fri. 12:36:04
 * @author muto_yu
 * @brief  共通メッセージウインドウ操作クラス
 *         リソースは持たず、外部で構築され提供されたリソースに対する操作のみを行う
 * @note   NetAppLibより移動
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <AppLib/include/ui/UIView.h>
#include  <AppLib/include/tool/app_tool_TimeIcon.h>


#include  "AppLib/include/Tool/CommonMessageWindowManipulator.h"


namespace App {
namespace Tool  {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     CommonMessageWindowManipulator
  *  @brief    ctor
  */
//------------------------------------------------------------------
CommonMessageWindowManipulator::CommonMessageWindowManipulator(void)
  : m_SetupInfo()
  , m_pPane_Window(NULL)
  , m_pPane_TextBoxImmediate(NULL)
  , m_pPane_Stream(NULL)
  , m_pParts_TimerIcon(NULL)
  , m_IsNoticedEOM(false)
  , m_IsNoticedFinish(false)
{
}

//------------------------------------------------------------------
/**
  *  @func     ~CommonMessageWindowManipulator
  *  @brief    dtor
  */
//------------------------------------------------------------------
CommonMessageWindowManipulator::~CommonMessageWindowManipulator()
{
}


//------------------------------------------------------------------
/**
  *  @func     Setup
  *  @brief    初期設定
  *  @note     内部でG2DUtilのCreateMessageWindowなどを行う
  */
//------------------------------------------------------------------
void  CommonMessageWindowManipulator::Setup(const SSetupInfo& rSetupInfo)
{
  app::util::G2DUtil*   pG2DUtil        = rSetupInfo.pTargetView->GetG2DUtil();
  gfl2::lyt::LytWk*     pLayoutWork     = pG2DUtil->GetLayoutWork(rSetupInfo.layoutWorkID);


  Setup( rSetupInfo, (rSetupInfo.paneID_WindowParts == gfl2::lyt::LYTSYS_PANEINDEX_ERROR) ? NULL : pLayoutWork->GetPartsPane(rSetupInfo.paneID_WindowParts) );
}

//------------------------------------------------------------------
/**
  *  @func     Setup
  *  @brief    初期設定
  *  @brief    ウィンドウのパーツペインを指定
  *  @note     内部でG2DUtilのCreateMessageWindow,CreateMsgCursorなどを行う。Delete～は外部に依存する（通常UIView::Delete2D()で解体される）。
  *            Setup処理内で rSeetupInfo.paneID_WindowParts 変数は使わないで変わりに pWindowsPartsPane を使う
  */
//------------------------------------------------------------------
void  CommonMessageWindowManipulator::Setup(const SSetupInfo& rSetupInfo, gfl2::lyt::LytParts* pWindowsPartsPane)
{
  GFL_ASSERT( rSetupInfo.pHeap );
  GFL_ASSERT( rSetupInfo.pTargetView );

  m_SetupInfo     = rSetupInfo;

  app::util::G2DUtil*               pG2DUtil        = m_SetupInfo.pTargetView->GetG2DUtil();
  const gfl2::lyt::LytMultiResID&   rLytMultiResID  = pG2DUtil->GetLayoutResourceID(m_SetupInfo.layoutWorkID);
  gfl2::lyt::LytWk*                 pLayoutWork     = pG2DUtil->GetLayoutWork(m_SetupInfo.layoutWorkID);


  m_pPane_Window = pWindowsPartsPane;
  if(m_pPane_Window)
  {
    m_pPane_Window->SetVisible(false);
  }

  /*
    DeleteMsgCursor
    DeleteMessageWindow
      はUIView任せ
  */


  /*  汎用win  */
  pG2DUtil->CreateMessageWindow(
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager(),
    m_SetupInfo.layoutWorkID,
    pWindowsPartsPane ? pLayoutWork->GetTextBoxPane(pWindowsPartsPane, m_SetupInfo.paneID_TextBox1a, &rLytMultiResID ) : pLayoutWork->GetTextBoxPane(m_SetupInfo.paneID_TextBox1a),
    pWindowsPartsPane ? pLayoutWork->GetTextBoxPane(pWindowsPartsPane, m_SetupInfo.paneID_TextBox2a, &rLytMultiResID ) : pLayoutWork->GetTextBoxPane(m_SetupInfo.paneID_TextBox2a),
    m_SetupInfo.pHeap
  );
  SetFinishMode_None();

  /*  messageCursor  */
  if(m_SetupInfo.paneID_MsgCursor != gfl2::lyt::LYTSYS_PANEINDEX_ERROR)
  {
    if(m_SetupInfo.paneID_WindowParts == gfl2::lyt::LYTSYS_PANEINDEX_ERROR)
    {
      pG2DUtil->CreateMsgCursor(m_SetupInfo.pHeap, m_SetupInfo.layoutWorkID, m_SetupInfo.paneID_MsgCursor);
    }
    else
    {
      pG2DUtil->CreateMsgCursor(m_SetupInfo.pHeap, m_SetupInfo.layoutWorkID, m_SetupInfo.paneID_WindowParts, m_SetupInfo.paneID_MsgCursor);
    }
    pG2DUtil->SetMsgCursorUserVisible(false, true);

    if(m_SetupInfo.animID_MsgCursor != UINT_MAX)
    {
      pG2DUtil->StartAnime(m_SetupInfo.layoutWorkID, m_SetupInfo.animID_MsgCursor);
    }
  }

  /*  各種Paneを保持  */
  m_pPane_TextBoxImmediate  = pWindowsPartsPane ? pLayoutWork->GetTextBoxPane( pWindowsPartsPane, m_SetupInfo.paneID_TextBox2, &rLytMultiResID ) : pLayoutWork->GetTextBoxPane(m_SetupInfo.paneID_TextBox2);
  m_pPane_Stream            = pWindowsPartsPane ? pLayoutWork->GetPane( pWindowsPartsPane, m_SetupInfo.paneID_NullText1, &rLytMultiResID ) : pLayoutWork->GetPane(m_SetupInfo.paneID_NullText1);

  /*  timerIcon  */
  if(m_SetupInfo.paneID_TimerIcon == gfl2::lyt::LYTSYS_PANEINDEX_ERROR)
  {
    m_pParts_TimerIcon  = NULL;
  }
  else
  {
    m_pParts_TimerIcon  = pWindowsPartsPane ? pLayoutWork->GetPartsPane( pWindowsPartsPane, m_SetupInfo.paneID_TimerIcon, &rLytMultiResID ) : pLayoutWork->GetPartsPane(m_SetupInfo.paneID_TimerIcon);
  }

  m_IsNoticedEOM    = false;
  m_IsNoticedFinish = false;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  CommonMessageWindowManipulator::SetMessageSpeed(Savedata::ConfigSave::MSG_SPEED spd)
{
  if(m_SetupInfo.pTargetView)
  {
    app::util::G2DUtil*   pG2DUtil  = m_SetupInfo.pTargetView->GetG2DUtil();

    if(pG2DUtil)
    {
      print::MessageWindow* pMessageWindow  = pG2DUtil->GetMsgWin();

      if(pMessageWindow)
      {
        pMessageWindow->SetMessageSpeed(spd);
      }
    }
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    G2DUtilに設定されているMessageData内のメッセージ（messageID）を表示
  */
//------------------------------------------------------------------
void  CommonMessageWindowManipulator::ShowMessage(const u32 messageID, const bool isImmediateMessage, const bool isShowTimerIcon)
{
  app::util::G2DUtil*   pG2DUtil      = m_SetupInfo.pTargetView->GetG2DUtil();
  gfl2::str::MsgData*   pMessageData  = pG2DUtil->GetMsgData();
  gfl2::str::StrBuf*    pStrBuf       = pG2DUtil->GetTempStrBuf( 0 );


  pMessageData->GetString( messageID, *pStrBuf );
  ShowMessage(pStrBuf, isImmediateMessage, isShowTimerIcon);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  CommonMessageWindowManipulator::ShowMessage(const gfl2::str::StrBuf* pStrBuf, const bool isImmediateMessage, const bool isShowTimerIcon)
{
  app::util::G2DUtil*   pG2DUtil  = m_SetupInfo.pTargetView->GetG2DUtil();

  m_pPane_TextBoxImmediate->SetVisible(isImmediateMessage);
  m_pPane_Stream->SetVisible(!isImmediateMessage);

  if(pStrBuf) {
    if(isImmediateMessage)
    {
      m_SetupInfo.pTargetView->SetTextboxPaneString( m_pPane_TextBoxImmediate, const_cast<gfl2::str::StrBuf*>(pStrBuf) );
    }
    else
    {
    //  void GetG2DUtil()->SetMessageCallback( print::MsgCallback * cb );
    //  void GetG2DUtil()->SetMessageFinishMode( gfl2::str::StrWin::FinishType type, u8 frame );
      pG2DUtil->SetMessage( pStrBuf );
    }
  }
//  pG2DUtil->StartAnime(m_SetupInfo.layoutWorkID, m_SetupInfo.animID_In);
  if(m_pPane_Window)
  {
    m_pPane_Window->SetVisible(true);    /*  ウインドウ可視化  */
  }

  /*  TimerIcon  */
  if(m_pParts_TimerIcon)
  {
    m_pParts_TimerIcon->SetVisible(isShowTimerIcon);
    if(isShowTimerIcon)
    {
      if(m_SetupInfo.animID_TimerIcon_In != UINT_MAX)
      {
        pG2DUtil->StartAnime(m_SetupInfo.layoutWorkID, m_SetupInfo.animID_TimerIcon_In);

        if(m_SetupInfo.animID_TimerIcon_Out != UINT_MAX)
        {
          pG2DUtil->StopAnime(m_SetupInfo.layoutWorkID, m_SetupInfo.animID_TimerIcon_Out);
        }
      }

      /*  timerアイコンを表示する際、アニメーションが設定されていれば専用メソッドでアニメーションを開始する  */
      if(m_SetupInfo.animID_TimerIcon != UINT_MAX)
      {
        pG2DUtil->StopAnime( m_SetupInfo.layoutWorkID, m_SetupInfo.animID_TimerIcon);
        App::Tool::TimeIcon::StartAnime( pG2DUtil->GetLayoutWork(m_SetupInfo.layoutWorkID), m_SetupInfo.animID_TimerIcon);
      }
    }
  }

  m_IsNoticedEOM    = false;
  m_IsNoticedFinish = false;

  pG2DUtil->Update2D();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  CommonMessageWindowManipulator::HideMessage(void)
{
  app::util::G2DUtil*   pG2DUtil  = m_SetupInfo.pTargetView->GetG2DUtil();

  if(m_pPane_Window)
  {
    m_pPane_Window->SetVisible(false);    /*  ウインドウ不可視化  */
  }
  if(m_pParts_TimerIcon)
  {

    if(m_SetupInfo.animID_TimerIcon_Out != UINT_MAX)
    {
      pG2DUtil->StartAnime(m_SetupInfo.layoutWorkID, m_SetupInfo.animID_TimerIcon_Out);
      if(m_SetupInfo.animID_TimerIcon_In != UINT_MAX)
      {
        pG2DUtil->StopAnime(m_SetupInfo.layoutWorkID, m_SetupInfo.animID_TimerIcon_In);
      }
    }

    m_pParts_TimerIcon->SetVisible(false);
  }

  pG2DUtil->Update2D();
}


/* -------------------------------------------------------------------------*/
/**
 * @brief キー入力可否設定
 * @note  trueで一時キー入力無効
 */
/* -------------------------------------------------------------------------*/
void  CommonMessageWindowManipulator::SetSuspendKey( bool flag )
{
  app::util::G2DUtil*   pG2DUtil  = m_SetupInfo.pTargetView->GetG2DUtil();
  if( pG2DUtil->GetMsgWin() )
  {
    pG2DUtil->GetMsgWin()->SetSuspendKey( flag );
  }
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  CommonMessageWindowManipulator::UpdateWindow(void)
{
  app::util::G2DUtil*         pG2DUtil      = m_SetupInfo.pTargetView->GetG2DUtil();
  gfl2::str::StrWin::Result   strWinResult  = pG2DUtil->PrintMessage();


  if((!m_IsNoticedEOM) && (strWinResult == gfl2::str::StrWin::RES_DONE))
  {
    m_IsNoticedEOM = true;
    if(m_SetupInfo.pHandler)
    {
      m_SetupInfo.pHandler->CommonMessageWindowManipulator_OnEvent(EVENT_OnEOM);
    }
  }
  else if((!m_IsNoticedFinish) && (strWinResult == gfl2::str::StrWin::RES_FINISH))
  {
    m_IsNoticedFinish = true;
    if(m_SetupInfo.pHandler)
    {
      m_SetupInfo.pHandler->CommonMessageWindowManipulator_OnEvent(EVENT_OnFinish);
    }
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  CommonMessageWindowManipulator::SetFinishMode_None(void)
{
  m_SetupInfo.pTargetView->GetG2DUtil()->GetMsgWin()->SetFinishMode(gfl2::str::StrWin::FINISH_NONE, 0);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  CommonMessageWindowManipulator::SetFinishMode_UserInput(void)
{
  m_SetupInfo.pTargetView->GetG2DUtil()->GetMsgWin()->SetFinishMode(gfl2::str::StrWin::FINISH_USER, 0);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  CommonMessageWindowManipulator::SetFinishMode_Timer(const u8 timeoutFrame)
{
  m_SetupInfo.pTargetView->GetG2DUtil()->GetMsgWin()->SetFinishMode(gfl2::str::StrWin::FINISH_TIMER,timeoutFrame );
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  CommonMessageWindowManipulator::SetFinishMode_UserInputOrTimer(const u8 timeoutFrame)
{
  m_SetupInfo.pTargetView->GetG2DUtil()->GetMsgWin()->SetFinishMode(gfl2::str::StrWin::FINISH_HYB, timeoutFrame);
}


//------------------------------------------------------------------
/**
  *  @brief    SetupInfo.pTargetView->GetG2DUtil()を返す
  */
//------------------------------------------------------------------
app::util::G2DUtil* CommonMessageWindowManipulator::GetG2DUtil(void) const
{
  if(m_SetupInfo.pTargetView)
  {
    return m_SetupInfo.pTargetView->GetG2DUtil();
  }
  return NULL;
}


} /*  namespace Tool */
} /*  namespace App  */
