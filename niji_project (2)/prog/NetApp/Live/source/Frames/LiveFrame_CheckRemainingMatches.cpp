
//==============================================================================
/**
 * @file   LiveFrame_CheckRemainingMatches.cpp
 * @date   2015/07/27 Mon. 16:09:01
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
#include  "../ApplicationSystem/LiveWork.h"
#include  "../LiveResource.h"

#include  "./LiveFrame_CheckRemainingMatches.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_Entry = 0,
    STATE_CheckRemains,

    STATE_SaveStart,
    STATE_Saving,

    STATE_CompleteInformation,
    STATE_Exit,

    STATE_     /*  terminator  */
  };
} /*  namespace  */


namespace NetApp  {
namespace Live {
// =============================================================================
/**/
//==============================================================================



//------------------------------------------------------------------
/**
  *  @func     Frame_CheckRemainingMatches
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_CheckRemainingMatches::Frame_CheckRemainingMatches(LiveWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->GetViewContainer())
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_pUpperView_Main(NULL)
  , m_pLowerView_Selection(NULL)

{
  m_pUpperView_Main       = m_pViewContainer->m_pUpperView_Main;
  m_pLowerView_Selection  = m_pViewContainer->m_pLowerView_Selection;
}


//------------------------------------------------------------------
/**
  *  @func     Frame_CheckRemainingMatches
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_CheckRemainingMatches::~Frame_CheckRemainingMatches()
{
}














/*
  override  NetAppFrameBase I/F
*/

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  Frame_CheckRemainingMatches::startup()
{
  m_pViewContainer->SetScene(LiveViewContainer::SCENE_CheckRemainingMatches);

  m_pUpperView_Main->SetViewMode(UpperView_Main::VIEWMODE_TournamentNameWithID);
  m_pLowerView_Selection->SetEventHandler(this);
  m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_CheckRemainingMatches_NotComplete);

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
void  Frame_CheckRemainingMatches::setListener()
{
  m_pViewContainer->SetAllInputEnable(true);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_CheckRemainingMatches::removeListener()
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
void  Frame_CheckRemainingMatches::updateSequence()
{
  switch(m_State)
  {
    case  STATE_Entry:
      m_State = STATE_CheckRemains;
      /*  fallthrough  */

    case  STATE_CheckRemains:
    {
      if(m_pWork->m_pAppParam->GetRemainingMatches())
      {
        /*  未達  */
        ExitFrame();
      }
      else
      {
        /*  達成  */
        m_pWork->m_pAppParam->SetLiveEntryWork_AllMatchOver();
        m_State = STATE_SaveStart;
      }
    }
      break;





    case  STATE_SaveStart:
      m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_CheckRemainingMatches_Saving);
      m_SaveUtil.SaveStart(m_pWork);
      m_State = STATE_Saving;
      /*  fall through  */

    case  STATE_Saving:
      if(m_SaveUtil.SaveMain())
      {
        /*  お疲れ様でした  */
        m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_CheckRemainingMatches_Completed);
        m_State = STATE_CompleteInformation;
      }
      break;




    case  STATE_CompleteInformation:
      break;

    case  STATE_Exit:
      /*
        workにFrameResultを設定することでSwitcherが呼ばれる
      */
      m_pWork->SetFrameResult( m_Result );
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
bool  Frame_CheckRemainingMatches::cleanup()
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
void  Frame_CheckRemainingMatches::LowerView_Selection_OnEvent(const EventCode eventCode)
{
  switch(eventCode)
  {
    case  LowerView_Selection::IEventHandler::EVENT_MessageFinish:
      if(m_State == STATE_CompleteInformation)
      {
        ExitFrame(RESULT_CHECK_REMAINING_MATCHES_Complete);
      }
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
void  Frame_CheckRemainingMatches::ExitFrame(const FrameResult frameResult)
{
  m_Result  = frameResult;
  m_State   = STATE_Exit;
  m_pViewContainer->SetAllInputEnable(false);
}



} /*  namespace Live    */
} /*  namespace NetApp  */
