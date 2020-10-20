//==============================================================================
/**
 * @file   LiveFrame_Registration.cpp
 * @date   2015/07/24 Fri. 16:27:49
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "GameSys/include/GameData.h"

#include  "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include  "../ApplicationSystem/LiveWork.h"
#include  "../LiveResource.h"

#include  "./LiveFrame_Registration.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_Entry = 0,

    STATE_ConfirmRegistration,        /**<  チーム登録：｛登録する｝                        */

    STATE_ConfirmCancel,              /**<  チーム登録：登録を中止する？                    */
    STATE_ConfirmDeregistration,      /**<  チーム登録：登録解除する？                      */
    STATE_ConfirmDeregistration2,     /**<  チーム登録：復帰できなくなるが登録解除する？    */

    STATE_CancelRegistration,         /**<  チーム登録：登録解除キャンセル                  */
    STATE_DeregistrationDone,         /**<  チーム登録：登録解除                            */

    STATE_SaveStart,                  /**<  チーム登録：セーブ開始                          */
    STATE_Saving,                     /**<  チーム登録：セーブ中                            */
    STATE_SaveStart_Deregistration,   /**<  チーム登録：セーブ開始（登録解除）              */
    STATE_Saving_Deregistration,      /**<  チーム登録：セーブ中（登録解除）                */

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
  *  @func     Frame_Registration
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_Registration::Frame_Registration(LiveWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->GetViewContainer())
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_SaveUtil()
  , m_pUpperView_Main(NULL)
  , m_pLowerView_Selection(NULL)
  , m_IsRegistered(false)

{
  m_pUpperView_Main    = m_pViewContainer->m_pUpperView_Main;
  m_pLowerView_Selection  = m_pViewContainer->m_pLowerView_Selection;
}


//------------------------------------------------------------------
/**
  *  @func     Frame_Registration
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_Registration::~Frame_Registration()
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
bool  Frame_Registration::startup()
{
  m_pViewContainer->SetScene(LiveViewContainer::SCENE_Registration);


  /** ライブ大会経由の呼び出しか、チーム選択からの戻り（決定）かで分岐する  */
  {
    m_pUpperView_Main->SetViewMode(UpperView_Main::VIEWMODE_TournamentName);
    if(m_pWork->m_pAppParam->m_BootMode == BOOTMODE_TeamSelect_Selected)
    {
      /*  チーム選択からの決定戻り  */
      m_pWork->m_pAppParam->ResetBootMode();   /*  起動モードを通常にリセット  */
      OnSaveStart();
    }
    else
    {
      OnEntry();
    }
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
void  Frame_Registration::setListener()
{
  m_pLowerView_Selection->SetEventHandler(this);
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
void  Frame_Registration::removeListener()
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
void  Frame_Registration::updateSequence()
{
  switch(m_State)
  {
    case  STATE_Entry:
      OnEntry();
      /*  fallthrough  */

    case  STATE_ConfirmRegistration:        /**<  チーム登録：｛登録する｝                        */
      break;

    case  STATE_ConfirmCancel:              /**<  チーム登録：登録を中止する？                    */
      break;

    case  STATE_ConfirmDeregistration:      /**<  チーム登録：登録解除する？                      */
      break;

    case  STATE_ConfirmDeregistration2:     /**<  チーム登録：復帰できなくなるが登録解除する？    */
      break;

    case  STATE_CancelRegistration:       /**<  チーム登録：登録キャンセル                      */
      break;

    case  STATE_DeregistrationDone:         /**<  チーム登録：登録解除                            */
      break;


    case  STATE_SaveStart:
      OnSaveStart();
      /*  fallthrough  */

    case  STATE_Saving:                     /**<  チーム登録：セーブ中                            */
    {
      if(m_SaveUtil.SaveMain()) ExitFrame(RESULT_REGISTRATION_Registered);
    }
      break;


    case  STATE_SaveStart_Deregistration:
      m_pWork->m_pAppParam->SetLiveEntryWork_NotParticipate();    /*  参加解除状態に  */
      m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_Registration_Saving);
      m_SaveUtil.SaveStart(m_pWork);
      m_State = STATE_Saving_Deregistration;
      /*  fallthrough  */

    case  STATE_Saving_Deregistration:      /**<  チーム登録：セーブ中（登録解除）                */
    {
      if(m_SaveUtil.SaveMain())
      {
        m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_Registration_DeregistrationDone);
        m_State = STATE_DeregistrationDone;
      }
    }
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
bool  Frame_Registration::cleanup()
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
void  Frame_Registration::LowerView_Selection_OnEvent(const EventCode eventCode)
{
  switch(eventCode)
  {
    case  LowerView_Selection::IEventHandler::EVENT_MessageFinish:
    {
      if(m_State == STATE_DeregistrationDone)
      {
        ExitFrame();
      }
      else if(m_State == STATE_CancelRegistration)
      {
        ExitFrame();
    }
    }
      break;

    case  LowerView_Selection::IEventHandler::EVENT_Registration_ConfirmRegistration_Start:  /**<  チーム登録：｛登録する｝                        */
      m_pWork->m_pAppParam->m_Result  = APPRESULT_CallTeamSelect;
      ExitFrame(RESULT_REGISTRATION_TeamSelect);
      break;

    case  LowerView_Selection::IEventHandler::EVENT_Registration_ConfirmCancel_Yes:          /**<  チーム登録：登録を中止する？                    */
      m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_Registration_ConfirmDeregistration);
      m_State = STATE_ConfirmDeregistration;
      break;

    case  LowerView_Selection::IEventHandler::EVENT_Registration_ConfirmCancel_No:           /**<  チーム登録：登録を中止する？                    */
      m_State = STATE_Entry;
      break;

    case  LowerView_Selection::IEventHandler::EVENT_Registration_ConfirmDeregistration_Yes:  /**<  チーム登録：登録解除する？                      */
      m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_Registration_ConfirmDeregistration2);
      m_State = STATE_ConfirmDeregistration2;
      break;

    case  LowerView_Selection::IEventHandler::EVENT_Registration_ConfirmDeregistration2_Yes: /**<  チーム登録：復帰できなくなるが登録解除する？    */
      /*  参加解除  */
      m_pWork->m_pAppParam->SetLiveEntryWork_NotParticipate();
      m_State = STATE_SaveStart_Deregistration;
      break;


    case  LowerView_Selection::IEventHandler::EVENT_Registration_ConfirmDeregistration_No:   /**<  チーム登録：登録解除する？                      */  /*  through  */
      m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_Registration_CancelRegistration);
      m_State = STATE_CancelRegistration;
      break;


    case  LowerView_Selection::IEventHandler::EVENT_Registration_ConfirmDeregistration2_No:  /**<  チーム登録：復帰できなくなるが登録解除する？    */
      m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_Registration_CancelRegistration);
      m_State = STATE_DeregistrationDone;
      break;


    case  LowerView_Selection::IEventHandler::EVENT_Exit:
      m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_Registration_ConfirmCancel);
      m_State = STATE_ConfirmCancel;
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
void  Frame_Registration::ExitFrame(const FrameResult frameResult)
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
void  Frame_Registration::OnSaveStart(void)
{
  m_pWork->m_pAppParam->SetLiveEntryWork_Participating();    /*  参加状態に  */
  /*  replayについては、大会メニュー側で試合数が1以上の場合に有効とするためここでは削除しない  */
  m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_Registration_Saving);
  m_SaveUtil.SaveStart(m_pWork);
  m_State = STATE_Saving;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_Registration::OnEntry(void)
{
  m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_Registration_ConfirmRegistration);
  m_State = STATE_ConfirmRegistration;
}




} /*  namespace Live    */
} /*  namespace NetApp  */
