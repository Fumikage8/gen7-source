//==============================================================================
/**
 * @file   PhotoViewMessageMenu.cpp
 * @date   2017/02/07 Tue. 15:45:40
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../../ApplicationSystem/PhotoViewWork.h"

#include  <print/include/PrintSystem.h>

#include  "../../PhotoViewResource.h"
#include  "./PhotoViewMessageMenu.h"


// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_PhotoViewMessageMenu
namespace FILELOCAL {
  enum  FinishMode
  {
    FINISH_None,
    FINISH_UserInput,
    FINISH_Timeout,
    FINISH_UserInputOrTimeout
  };


  static const struct MessageDefinition
  {
    u32         messageID;
    bool        isImmediate;
    bool        isYesNo;
    bool        isShowTimerIcon;
    bool        isShowBG;
    FinishMode  finishMode;
    u8          timeoutFrame;
  }messageDefinitions[NetApp::PhotoView::MessageMenu::MODE_MAX]  =  {
                           /*  messageID, immediate,  yesNo,  timericon,  BG,     finishMode,   timeoutFrame    */
#define   MSGDEF_Immediate(_id)   {(_id), true,       false,  false,      false,  FINISH_None,  0}
#define   MSGDEF_YN(_id)          {(_id), false,      true,   false,      true,   FINISH_None,  0}
#define   MSGDEF_Timer(_id)       {(_id), true,       false,  true,       true,   FINISH_None,  0}
#define   MSGDEF_TimerNoBG(_id)   {(_id), true,       false,  true,       false,  FINISH_None,  0}
#define   MSGDEF_Stream(_id)      {(_id), false,      false,  false,      true,   FINISH_None,  0}
#define   MSGDEF_StreamNoBG(_id)  {(_id), false,      false,  false,      false,  FINISH_None,  0}
    
    MSGDEF_Immediate(   UINT_MAX),              //    MODE_None
    MSGDEF_YN(          jf_playerlist_win_18),  //    MODE_ConfirmUGC,    /**<  UGC閲覧に関する確認  */

#undef    MSGDEF_Immediate
#undef    MSGDEF_YN
#undef    MSGDEF_Timer
#undef    MSGDEF_TimerNoBG
#undef    MSGDEF_Stream
  };


  static const gfl2::fs::ArcFile::ARCDATID  MESSAGE_ARCDAT  = NetApp::PhotoView::MESSAGE_GRAC_ID;
}

namespace NetApp    {
namespace PhotoView  {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
MessageMenu::MessageMenu(PhotoViewWork* pWork, NetAppLib::UI::NetAppMessageMenu::IEventHandler* pHandler)
  : NetAppLib::UI::NetAppMessageMenu(pWork, pHandler)
  , m_pWork(pWork)
  , m_Mode(MODE_None)
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
MessageMenu::~MessageMenu()
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  MessageMenu::Show(const Mode mode)
{
  const FILELOCAL::MessageDefinition&  rMessageDefinition  = FILELOCAL::messageDefinitions[mode];

  if(m_Mode == mode)  return;


  m_Mode  = mode;

  PreShow();
  if(!m_pWork->GetMessageUtility().IsValidMessageID(FILELOCAL::MESSAGE_ARCDAT, rMessageDefinition.messageID)) return;

  switch(rMessageDefinition.finishMode)
  {
    case  FILELOCAL::FINISH_None:                SetFinishMode_None();                                             break;
    case  FILELOCAL::FINISH_UserInput:           SetFinishMode_UserInput();                                        break;
    case  FILELOCAL::FINISH_Timeout:             SetFinishMode_Timer(rMessageDefinition.timeoutFrame);             break;
    case  FILELOCAL::FINISH_UserInputOrTimeout:  SetFinishMode_UserInputOrTimer(rMessageDefinition.timeoutFrame);  break;
  }

  if(rMessageDefinition.isYesNo)
  {
    ShowYN(
      rMessageDefinition.messageID,
      rMessageDefinition.isImmediate
    );
  }
  else
  {
    ShowMessage(
      FILELOCAL::MESSAGE_ARCDAT,
      rMessageDefinition.messageID,
      rMessageDefinition.isImmediate,
      rMessageDefinition.isShowTimerIcon,
      rMessageDefinition.isShowBG
    );
  }
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    showの前に共通でやっておくこと
  */
//------------------------------------------------------------------
void  MessageMenu::PreShow(void)
{
  SetYNListMode(false);
  SetFinishMode_None();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  MessageMenu::ShowYN(const u32 messageID, const bool isImmediate)
{
  SetYNListMode(true);
  ShowMessageWithListMenu(
    FILELOCAL::MESSAGE_ARCDAT,
    messageID,
    isImmediate,
    FILELOCAL::MESSAGE_ARCDAT,
    jf_playerlist_sel_06,
    jf_playerlist_sel_07
  );
}


} /*  namespace PhotoView  */
} /*  namespace NetApp    */
