//==============================================================================
/**
 * @file   LiveFrame_Main.cpp
 * @date   2015/07/09 Thu. 15:22:02
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

#include  "./LiveFrame_Main.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_Entry = 0,
    STATE_Main,
    STATE_CheckRegulation,
    STATE_ConfirmReceivePlayerPass,
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
  *  @func     Frame_Main
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_Main::Frame_Main(LiveWork* pWork)
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
  m_pViewContainer->SetScene(LiveViewContainer::SCENE_MainMenu);


  MainState();
  
  m_pViewContainer->SetAllInputEnable(true);

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

    case  STATE_CheckRegulation:
      /*
        ※regulationが初期状態であることのチェックはしない
        →参加ワークの値のみで判定していくことになるので、当初の仕様書にあった右ルート（本体が異なっている可能性云々）に進むための条件がなくなる
        →右ルートを省略することとなった
      */
      if(m_pWork->m_pAppParam->IsNeedReceiveNewRegulation())
      {
        /* regulation受信が必要  */
        m_pLowerView_Selection->SetViewMode(LowerView_Selection::VIEWMODE_MainMenu_ReceivePlayerPass);
        m_State = STATE_ConfirmReceivePlayerPass;
      }
      else
      {
        /* regulationはあるので受信シーケンスはSkip   */
        if(m_pWork->m_pAppParam->IsNeedBattleTeamRegistration())  ExitFrame(RESULT_MAIN_RegisterTeam);      /*  チーム登録    */
        else                                                      ExitFrame(RESULT_MAIN_CheckRemainMatch);  /*  対戦回数確認  */
      }
      break;

    case  STATE_ConfirmReceivePlayerPass:
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
    /*  ライブ大会を始める  */
    case  LowerView_Selection::IEventHandler::EVENT_SelectedStart:
      m_State = STATE_CheckRegulation;
      break;

    /*
    */
    case  LowerView_Selection::IEventHandler::EVENT_ConfirmReceivePlayerPass_Ir:
      ExitFrame(RESULT_MAIN_ReceivePlayerPass);    /*  選手証受信へ  */
      break;

    case  LowerView_Selection::IEventHandler::EVENT_ConfirmReceivePlayerPass_QR:
      m_pWork->m_pAppParam->m_Result  = APPRESULT_CallQRReader;
      ExitFrame(RESULT_MAIN_ReceivePlayerPassQR);    /*  選手証QR読み込みへ  */
      break;

    case  LowerView_Selection::IEventHandler::EVENT_Exit: /*  through  */
      if(m_State == STATE_ConfirmReceivePlayerPass)
      {
        MainState();
        m_State = STATE_Main;
      }
      else
      {
        ExitFrame();
      }
      break;

    case  LowerView_Selection::IEventHandler::EVENT_SelectedReplay:
      ExitFrame(RESULT_MAIN_Replay);
      break;

    case  LowerView_Selection::IEventHandler::EVENT_SelectedUnregister:
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
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_Main::MainState(void)
{
  const LowerView_Selection::ViewMode lowerViewMode  = m_pWork->m_pAppParam->IsShowLastLiveMatchInfoAvailable() ? LowerView_Selection::VIEWMODE_MainMenu_Full : LowerView_Selection::VIEWMODE_MainMenu_NoReplay;

  m_pLowerView_Selection->SetViewMode(lowerViewMode);
}


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



} /*  namespace Live    */
} /*  namespace NetApp  */
