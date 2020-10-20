
//==============================================================================
/**
 * @file   QRReaderFrame_Tutorial.cpp
 * @date   2016/01/14 Thu. 16:58:03
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include  "../System/QRReaderWork.h"
#include  "../QRReaderResource.h"

#include  "./QRReaderFrame_Tutorial.h"
// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_Entry = 0,

    STATE_Confirm,
    STATE_StartTutorial,
    STATE_Tutorial,

    STATE_Exit,

    STATE_     /*  terminator  */
  };
} /*  namespace  */



namespace NetApp      {
namespace QRReader {
// =============================================================================
/**/
//==============================================================================



//------------------------------------------------------------------
/**
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_Tutorial::Frame_Tutorial(QRReaderWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->m_pViewContainer)
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  /*  view  */
  , m_pUpperView(NULL)
  , m_pLowerView(NULL)
  , m_pMessageMenu(NULL)
{
}


//------------------------------------------------------------------
/**
  *  @func     Frame_Tutorial
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_Tutorial::~Frame_Tutorial()
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
bool  Frame_Tutorial::startup()
{
  /*  view  */
  m_pUpperView    = m_pViewContainer->m_pUpperView_Main;
  m_pLowerView    = m_pViewContainer->m_pLowerView_Main;
  m_pMessageMenu  = m_pLowerView->GetMessageMenu();

  if(GetQRReaderSaveData()->IsTutorialFinished())
  {
    ExitFrame();
  }
  else
  {
    m_pViewContainer->SetScene(QRReaderViewContainer::SCENE_Tutorial);
    m_pViewContainer->SetAllInputEnable(true);

    m_pUpperView->SetViewMode(UpperView_Main::VIEWMODE_None);
    m_pLowerView->SetViewMode(LowerView_Main::VIEWMODE_None);
    m_pMessageMenu->Show(MessageMenu::MODE_ConfirmTutorial);

    m_State = STATE_Confirm;
  }


  return true;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_Tutorial::setListener()
{
 // m_pUpperView->SetEventHandler(this);
  m_pLowerView->SetEventHandler(this);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_Tutorial::removeListener()
{
//  m_pUpperView->SetEventHandler(NULL);
  m_pLowerView->SetEventHandler(NULL);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_Tutorial::updateSequence()
{
  switch(m_State)
  {
    case  STATE_Entry:
      /*  fall through  */

    case  STATE_Confirm:
      break;

    case STATE_StartTutorial:
      m_State = STATE_Tutorial;
      /*  fall through  */

    case STATE_Tutorial:
      break;



    case  STATE_Exit:
      /*  フラグを立てて終了  */
      GetQRReaderSaveData()->TutorialFinished();
      m_pWork->SetFrameResult( m_Result );
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  Frame_Tutorial::cleanup()
{
  bool  bAllDone = false;


  bAllDone  = true;


  return  bAllDone;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_Tutorial::ExitFrame(const FrameResult frameResult)
{
  m_Result  = frameResult;
  m_State   = STATE_Exit;
  m_pViewContainer->SetAllInputEnable(false);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_Tutorial::LowerView_Main_OnEvent(const LowerView_Main::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  LowerView_Main::IEventHandler::EVENT_Message_Yes:
    {
      if(m_State  == STATE_Confirm)
      {
        m_pLowerView->ShowInformation();
        m_State = STATE_StartTutorial;
      }
    }
      break;
    case  LowerView_Main::IEventHandler::EVENT_Message_No:
    {
      if(m_State  == STATE_Confirm)
      {
        ExitFrame();
      }
    }
      break;
      
    case  LowerView_Main::IEventHandler::EVENT_InformationEnd:
    {
      if(m_State  == STATE_Tutorial)
      {
        ExitFrame();
      }
    }
      break;
  };
}



} /*  namespace QRReader */
} /*  namespace NetApp   */


