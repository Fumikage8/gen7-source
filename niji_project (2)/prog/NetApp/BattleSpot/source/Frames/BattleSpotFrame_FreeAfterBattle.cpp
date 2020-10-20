//==============================================================================
/**
 * @file   BattleSpotFrame_FreeAfterBattle.cpp
 * @date   2015/10/02 Fri. 13:32:57
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
#include  "../Views/BattleSpotViewContainer.h"

#include  "./BattleSpotFrame_FreeAfterBattle.h"

#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"

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
  *  @func     Frame_FreeAfterBattle
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_FreeAfterBattle::Frame_FreeAfterBattle(BattleSpotWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->m_pViewContainer)
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
//  , m_pUpperView(NULL)
  , m_pLowerView(NULL)
{
}


//------------------------------------------------------------------
/**
  *  @func     Frame_FreeAfterBattle
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_FreeAfterBattle::~Frame_FreeAfterBattle()
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
bool  Frame_FreeAfterBattle::startup()
{
  m_pViewContainer->SetAllInputEnabled( true );
  m_pViewContainer->SetScene(BattleSpotViewContainer::SCENE_FreeBattleEntry);

  /*  views  */
  {
//    m_pUpperView  = m_pViewContainer->m_pUpperView_Demo;
    m_pLowerView  = m_pViewContainer->m_pLowerView_Selection;

    
  }




  if( m_pWork->m_pAppParam->nextApp != NEXT_APP_BATTLEVIDEO_SAVE )
  {//ログ送信
    m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_Connecting);
#if defined(GF_PLATFORM_CTR)
    NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);
    pP2pConnectionManager->DisconnectStart( m_pWork->m_pAppParam->m_isSessionLeft );
#endif
  }
  else
  {//選択
    m_pWork->m_pAppParam->nextApp = NEXT_APP_NONE;
    m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_ContinueConfirm);
    m_State =  STATE_Selection;
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
void  Frame_FreeAfterBattle::setListener()
{
//  m_pUpperView->SetEventHandler(this);
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
void  Frame_FreeAfterBattle::removeListener()
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
void  Frame_FreeAfterBattle::updateSequence()
{
  switch(m_State)
  {
    case  STATE_Entry:
      {
        if( DisconnectWait() )
        {
          m_pWork->m_pAppParam->nextApp = NEXT_APP_BATTLEVIDEO_SAVE;
          m_pWork->m_pAppParam->nextFrame = FRAME_FreeAfterBattle;
          m_pWork->DisconnectFlagOff( m_pWork->GetCommonData() );
          ExitSelf(RESULE_FreeAfterBattle_BattleVideo);
        }
      }
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
bool  Frame_FreeAfterBattle::cleanup()
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
void  Frame_FreeAfterBattle::LowerView_Selection_OnEvent(LowerView_Selection* pSender, const LowerView_Selection::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  LowerView_Selection::IEventHandler::EVENT_Continue_Next:             
      {
        ExitSelf(RESULT_FreeAfterBattle_Next);
      }
    break;
    case  LowerView_Selection::IEventHandler::EVENT_Continue_Change:             
      {
        m_pWork->m_pAppParam->nextApp = NEXT_APP_TEAM_SELECT;
        m_pWork->m_pAppParam->nextFrame = FRAME_FreeBattleMatching;
        ExitSelf(RESULT_FreeAfterBattle_Change);
      }
    break;
    case  LowerView_Selection::IEventHandler::EVENT_Continue_Exit:             
      {
        ExitSelf(RESULT_FreeAfterBattle_Exit);
      }
    break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_FreeAfterBattle::ExitSelf(const FrameResult frameResult)
{
  m_pViewContainer->SetAllInputEnabled( false );
  m_Result  = frameResult;
  m_State   = STATE_Exit;
}

static u32 s_wait = 0;

bool Frame_FreeAfterBattle::DisconnectWait()
{
#if defined(GF_PLATFORM_CTR)
  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);
  if( s_wait++ > 90 )
  {
    if( pP2pConnectionManager->IsDisconnectSuccess() == true )
    {
      pP2pConnectionManager->Terminate();
      s_wait = 0;
      return true;
    }
  }
#endif

  return false;
}


} /*  namespace Live    */
} /*  namespace NetApp  */


