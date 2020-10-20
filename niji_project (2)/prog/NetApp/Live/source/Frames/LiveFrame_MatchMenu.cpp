//==============================================================================
/**
 * @file   LiveFrame_MatchMenu.cpp
 * @date   2015/07/27 Mon. 17:41:17
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

#include  "./LiveFrame_MatchMenu.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    /*  init  */
    STATE_Entry = 0,

    STATE_Main,
    STATE_NoReplayData,
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
  *  @func     Frame_MatchMenu
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_MatchMenu::Frame_MatchMenu(LiveWork* pWork)
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
  *  @func     Frame_MatchMenu
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_MatchMenu::~Frame_MatchMenu()
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
bool  Frame_MatchMenu::startup()
{
  m_pViewContainer->SetScene(LiveViewContainer::SCENE_MatchMenu);
//  m_pUpperView_Main->SetPokeParty(m_pWork->m_pAppParam->m_pSelectedParty);
//  m_pUpperView_Main->SetLiveMatchData(m_pWork->m_pAppParam->m_RegulationInfo.pRegulation, m_pWork->m_pAppParam->m_pLiveMatchData)
//  m_pUpperView_Main->UpdateIDCard();
  m_pUpperView_Main->SetViewMode(UpperView_Main::VIEWMODE_TournamentNameWithID);

  OnEntry();

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
void  Frame_MatchMenu::setListener()
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
void  Frame_MatchMenu::removeListener()
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
void  Frame_MatchMenu::updateSequence()
{
  switch(m_State)
  {
    case  STATE_Entry:
      OnEntry();
      /*  fallthrough  */

    case  STATE_Main:
      break;

    case  STATE_NoReplayData:
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
bool  Frame_MatchMenu::cleanup()
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
void  Frame_MatchMenu::LowerView_Selection_OnEvent(const EventCode eventCode)
{
  switch(eventCode)
  {
    case  LowerView_Selection::IEventHandler::EVENT_MessageFinish:
      if(m_State == STATE_NoReplayData)
      {
        /*  大会メニュー： 対戦の　記録がみつかりませんでした▼ のFinish          */
        m_State = STATE_Entry;
      }
      break;
    

    case  LowerView_Selection::IEventHandler::EVENT_MatchMenu_SelectedStart:          /**<  マッチメニュー：挑戦する  */
      ExitFrame(RESULT_MATCHMENU_Start);
      break;

    case  LowerView_Selection::IEventHandler::EVENT_MatchMenu_SelectedReplay:         /**<  マッチメニュー：振り返る  */
    {
      const Savedata::LiveMatchData::LIVE_RESULTS*  pResults    = m_pWork->m_pAppParam->m_pLiveMatchData->GetLiveResults();

      if(pResults->battle_num)
      {
        ExitFrame(RESULT_MATCHMENU_Replay);
      }
      else
      {
        /*  大会メニュー： 対戦の　記録がみつかりませんでした▼          */
        m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_MatchMenu_NoReplayData);
        m_State = STATE_NoReplayData;
      }
    }
      break;

    case  LowerView_Selection::IEventHandler::EVENT_MatchMenu_SelectedDeregistration: /**<  マッチメニュー：解除する  */
      ExitFrame(RESULT_MATCHMENU_UnregisterTeam);
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
void  Frame_MatchMenu::ExitFrame(const FrameResult frameResult)
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
void  Frame_MatchMenu::OnEntry(void)
{
  if(m_pWork->m_pAppParam->IsBattleOrRetireAvailable())
  {
    /**<  大会メニュー：｛挑戦する|振り返る|解除する｝  */
    m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_MatchMenu_Full);
  }
  else
  {
    /**<  大会メニュー：｛振り返る｝           */
    m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_MatchMenu_ReplayOnly);
  }
  m_State = STATE_Main;
}



} /*  namespace Live    */
} /*  namespace NetApp  */
