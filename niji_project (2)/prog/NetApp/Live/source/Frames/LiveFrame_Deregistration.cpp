
//==============================================================================
/**
 * @file   LiveFrame_Deregistration.cpp
 * @date   2015/07/27 Mon. 19:41:15
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

#include  "./LiveFrame_Deregistration.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    /*  init  */
    STATE_Entry = 0,

    STATE_ConfirmUnregister,
    STATE_ConfirmUnregister2,
    STATE_Saving,
    STATE_DeregistrationInformation,
    STATE_Exit,

    SUBSEQ_     /*  terminator  */
  };
} /*  namespace  */


namespace NetApp  {
namespace Live {
// =============================================================================
/**/
//==============================================================================



//------------------------------------------------------------------
/**
  *  @func     Frame_Deregistration
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_Deregistration::Frame_Deregistration(LiveWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->GetViewContainer())
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_SaveUtil()
  , m_pUpperView_Main(NULL)
  , m_pLowerView_Selection(NULL)

{
  m_pUpperView_Main       = m_pViewContainer->m_pUpperView_Main;
  m_pLowerView_Selection  = m_pViewContainer->m_pLowerView_Selection;
  
}


//------------------------------------------------------------------
/**
  *  @func     Frame_Deregistration
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_Deregistration::~Frame_Deregistration()
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
bool  Frame_Deregistration::startup()
{
  m_pViewContainer->SetScene(LiveViewContainer::SCENE_Deregistration);
  m_pLowerView_Selection->SetEventHandler(this);
  m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_Deregistration_ConfirmUnregister);

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
void  Frame_Deregistration::setListener()
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
void  Frame_Deregistration::removeListener()
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
void  Frame_Deregistration::updateSequence()
{

  switch(m_State)
  {
    case  STATE_Entry:
      /*  fallthrough  */

    case  STATE_ConfirmUnregister:
      break;

    case  STATE_ConfirmUnregister2:
      break;

    case  STATE_Saving:
    {
      if(m_SaveUtil.SaveMain())
      {
        m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_Deregistration_DeregistrationInformation);
        m_State = STATE_DeregistrationInformation;
      }
    }
      break;

    case  STATE_DeregistrationInformation:
      break;

    case  STATE_Exit:
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
bool  Frame_Deregistration::cleanup()
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
void  Frame_Deregistration::LowerView_Selection_OnEvent(const EventCode eventCode)
{
  switch(eventCode)
  {
    case  LowerView_Selection::IEventHandler::EVENT_MessageFinish:
      if(m_State  == STATE_DeregistrationInformation)
      {
        ExitFrame(RESULT_DEREGISTRATION_Unregistered);
      }
      break;

    case  LowerView_Selection::IEventHandler::EVENT_Deregistration_ConfirmUnregister_Yes:           /**<  登録解除：復帰できませんが解除しますか？  */
      m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_Deregistration_ConfirmUnregister2);
      m_State = STATE_ConfirmUnregister2;
      break;

    case  LowerView_Selection::IEventHandler::EVENT_Deregistration_ConfirmUnregister2_Yes:          /**<  登録解除：本当に解除しますか？  */
      /*  save開始  */
      m_pWork->m_pAppParam->SetLiveEntryWork_NotParticipate();    /*  参加解除状態に  */
      m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_Deregistration_Saving);
      m_SaveUtil.SaveStart(m_pWork);
      m_State = STATE_Saving;
      break;

    case  LowerView_Selection::IEventHandler::EVENT_Deregistration_ConfirmUnregister_No:            /**<  登録解除：復帰できませんが解除しますか？  */
    case  LowerView_Selection::IEventHandler::EVENT_Deregistration_ConfirmUnregister2_No:           /**<  登録解除：本当に解除しますか？  */
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
void  Frame_Deregistration::ExitFrame(const FrameResult frameResult)
{
  m_Result  = frameResult;
  m_State   = STATE_Exit;
  m_pViewContainer->SetAllInputEnable(false);
}



} /*  namespace Live    */
} /*  namespace NetApp  */
