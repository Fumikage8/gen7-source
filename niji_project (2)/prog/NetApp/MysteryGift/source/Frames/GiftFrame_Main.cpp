//==============================================================================
/**
 * @file   GiftFrame_Main.cpp
 * @date   2015/07/31 Fri. 16:26:50
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
#include  "../ApplicationSystem/GiftWork.h"
#include  "../GiftResource.h"

#include  "./GiftFrame_Main.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_Entry = 0,
    STATE_Main,
    STATE_LowVacancyInformation,
    STATE_Exit,

    STATE_     /*  terminator  */
  };

} /*  namespace  */


namespace NetApp  {
namespace Gift    {
// =============================================================================
/**/
//==============================================================================



//------------------------------------------------------------------
/**
  *  @func     Frame_Main
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_Main::Frame_Main(GiftWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->GetViewContainer())
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_pUpperView_Message(NULL)
  , m_pLowerView_Selection(NULL)

{
  m_pUpperView_Message    = m_pViewContainer->m_pUpperView_Message;
  m_pLowerView_Selection  = m_pViewContainer->m_pLowerView_Selection;
}


//------------------------------------------------------------------
/**
  *  @func     Frame_Main
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_Main::~Frame_Main()
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
bool  Frame_Main::startup()
{
  m_pWork->m_pAppParam->procResult  = APP_PARAM::PROCRESULT_Exit;

  m_pWork->m_pAppParam->m_bReceiveOneData   = false;
  m_pWork->m_IsReceiveViaSerialAuth         = false;

  m_pViewContainer->SetScene(GiftViewContainer::SCENE_MainMenu);
  m_pViewContainer->SetAllInputEnable(true);
  m_pUpperView_Message->SetViewMode(UpperView_Message::VIEWMODE_MainMenu_ChooseFunction);
  m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_MainMenu_ChooseFunction);

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
void  Frame_Main::setListener()
{
  m_pLowerView_Selection->SetEventHandler(this);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_Main::removeListener()
{
  m_pLowerView_Selection->SetEventHandler(NULL);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_Main::updateSequence()
{
  switch(m_State)
  {
    case  STATE_Entry:
      m_State = STATE_Main;
      /*  fallthrough  */
    
    
    case  STATE_Main:
      break;

    case  STATE_LowVacancyInformation:
      break;

    case  STATE_Exit:
      /*
        workにFrameResultを設定することでSwitcherが呼ばれる
      */
      m_pWork->SetFrameResult( m_Result );
      break;

    default:
      break;
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
bool  Frame_Main::cleanup()
{
  return true;
}




//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
/*  implement LowerView_Selection::IEventHandler  */
void  Frame_Main::LowerView_Selection_OnEvent(const EventCode eventCode)
{
  switch(eventCode)
  {
    case  LowerView_Selection::IEventHandler::EVENT_MessageFinish:
    {
      if(m_State  == STATE_LowVacancyInformation)
      {
        ExitFrame(RESULT_MAIN_Gift);
      }
    }
      break;

    case  LowerView_Selection::IEventHandler::EVENT_MainMenu_SelectedReceive:
    {
      const u32 vacancies = GetMysteryGiftSave()->GetGiftDataVacancies();
      if(!vacancies)
      {
        m_pLowerView_Selection->ShowMessage_AlbumFull();
      }
      else if(vacancies <= 5)
      {
        m_pLowerView_Selection->ShowMessage_AlbumLowVacancy(vacancies);
        m_State = STATE_LowVacancyInformation;
      }
      else
      {
        ExitFrame(RESULT_MAIN_Gift);
      }
    }
      break;
    
    case  LowerView_Selection::IEventHandler::EVENT_MainMenu_SelectedCardAlbum:
      if(!GetMysteryGiftSave()->GetGiftDataNum())
      {
        /*  カードがない  */
        m_pLowerView_Selection->ShowMessage_AlbumEmpty();
      }
      else
      {
        ExitFrame(RESULT_MAIN_Album);
      }
      break;

    case  LowerView_Selection::IEventHandler::EVENT_MainMenu_SelectedInstruction:
      ExitFrame(RESULT_MAIN_Instruction);
      break;

    case  LowerView_Selection::IEventHandler::EVENT_Exit:
      ExitFrame();
      break;

    default:
      break;
  }

}
/*  endimplement LowerView_Selection::IEventHandler  */



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_Main::ExitFrame(const FrameResult frameResult)
{
  m_Result  = frameResult;
  m_State   = STATE_Exit;
  m_pViewContainer->SetAllInputEnable(false);
}



} /*  namespace Gift    */
} /*  namespace NetApp  */
