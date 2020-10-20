//==============================================================================
/**
 * @file   BattleSpotFrame_Welcome.cpp
 * @date   2015/10/02 Fri. 13:21:51
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
#include  "../System/BattleSpotWork.h"

#include  "../BattleSpotResource.h"

#include  "./BattleSpotFrame_Welcome.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_Entry = 0,

    STATE_Selection,

    STATE_Exit,

    STATE_     /*  terminator  */
  };
} /*  namespace  */


namespace NetApp      {
namespace BattleSpot  {
// =============================================================================
/**/
//==============================================================================



//------------------------------------------------------------------
/**
  *  @func     Frame_Welcome
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_Welcome::Frame_Welcome(BattleSpotWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->m_pViewContainer)
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_pUpperView(NULL)
  , m_pLowerView(NULL)
{
}


//------------------------------------------------------------------
/**
  *  @func     Frame_Welcome
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_Welcome::~Frame_Welcome()
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
bool  Frame_Welcome::startup()
{
  m_pViewContainer->SetAllInputEnabled( true );
  m_pViewContainer->SetScene(BattleSpotViewContainer::SCENE_Welcome);

  /*  views  */
  {
    m_pUpperView  = m_pViewContainer->m_pUpperView_Information;
    m_pLowerView  = m_pViewContainer->m_pLowerView_Selection;

    m_pUpperView->SetViewMode(UpperView_Information::VIEWMODE_Welcome);
    m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Welcome_SelectMode);
  }

  //レギュレーションクリアしておく
  m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().DeleteData();

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
void  Frame_Welcome::setListener()
{
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
void  Frame_Welcome::removeListener()
{
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
void  Frame_Welcome::updateSequence()
{
  switch(m_State)
  {
    case  STATE_Entry:
      m_State =  STATE_Selection;
      break;

    case  STATE_Selection:
      break;

    case  STATE_Exit:
      exitFrame(m_Result);
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  Frame_Welcome::cleanup()
{
  bool  bAllDone = false;


  bAllDone  = true;


  return  bAllDone;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
/*  implement LowerView_Selection::IEventHandler  */
void  Frame_Welcome::LowerView_Selection_OnEvent(LowerView_Selection* pSender, const LowerView_Selection::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  LowerView_Selection::IEventHandler::EVENT_Back:                     
      {
        m_pWork->m_pAppParam->nextApp = NEXT_APP_NONE;
        ExitSelf();
      }
    break;
    case  LowerView_Selection::IEventHandler::EVENT_WelCome_FreeBattle:
      m_pWork->m_pAppParam->m_topMenuCurorIndex = 0;
      ExitSelf(RESULT_Welcome_FreeBattle);
    break;
    case  LowerView_Selection::IEventHandler::EVENT_WelCome_RatingBattle:
      m_pWork->m_pAppParam->m_topMenuCurorIndex = 1;
      ExitSelf(RESULT_Welcome_RatingBattle);
    break;
    case  LowerView_Selection::IEventHandler::EVENT_WelCome_InternetMatch:
      m_pWork->m_pAppParam->m_topMenuCurorIndex = 2;
      m_pWork->SetRule( BattleSpotWork::RULE_NET_CUP );
      ExitSelf(RESULT_Welcome_InternetMatch);
    break;
    case  LowerView_Selection::IEventHandler::EVENT_WelCome_FriendMatch:
      m_pWork->m_pAppParam->m_topMenuCurorIndex = 3;
      m_pWork->SetRule( BattleSpotWork::RULE_FRIEND_CUP );
      ExitSelf(RESULT_Welcome_FriendMatch);
    break;
      
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_Welcome::ExitSelf(const FrameResult frameResult)
{
  m_pViewContainer->SetAllInputEnabled( false );
  m_Result  = frameResult;
  m_State   = STATE_Exit;
}



} /*  namespace Live    */
} /*  namespace NetApp  */


