//==============================================================================
/**
 * @file   BattleSpotFrame_InternetAfterBattle.cpp
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

#include  "./BattleSpotFrame_InternetAfterBattle.h"

#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"

#include  "GameSys/include/GameData.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_Entry = 0,
    STATE_WaitBattleVideo,
    STATE_UpdateRank,
    STATE_Dissconnect,
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
  *  @func     Frame_InternetAfterBattle
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_InternetAfterBattle::Frame_InternetAfterBattle(BattleSpotWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->m_pViewContainer)
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
//  , m_pUpperView(NULL)
  , m_pLowerView(NULL)
  , m_seq(0)
{
}


//------------------------------------------------------------------
/**
  *  @func     Frame_InternetAfterBattle
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_InternetAfterBattle::~Frame_InternetAfterBattle()
{
#if defined(GF_PLATFORM_CTR)
  m_pWork->EndBattleVideo();
#endif
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  Frame_InternetAfterBattle::startup()
{

  m_pViewContainer->SetAllInputEnabled( true );

  m_pViewContainer->SetScene(BattleSpotViewContainer::SCENE_InternetMatchEntry);

  /*  views  */
  {
//    m_pUpperView  = m_pViewContainer->m_pUpperView_Demo;
    m_pLowerView  = m_pViewContainer->m_pLowerView_Selection;
  }

  if( m_pWork->m_pAppParam->nextApp != NEXT_APP_BATTLEVIDEO_SAVE )
  {//ログ送信
    m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_Connecting);
  }
  else
  {//選択
    m_pWork->m_pAppParam->nextApp = NEXT_APP_NONE;
    m_pLowerView->SetViewMode(LowerView_Selection::VIEWMODE_Message_ContinueConfirmCup);
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
void  Frame_InternetAfterBattle::setListener()
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
void  Frame_InternetAfterBattle::removeListener()
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
void  Frame_InternetAfterBattle::updateSequence()
{
  switch(m_State)
  {
  case  STATE_Entry:
    SendBattleVideo();
    break;
  case STATE_WaitBattleVideo:
    break;
  case  STATE_UpdateRank:
    {
      UpdateRankData();
    }
    break;
  case  STATE_Dissconnect:
    {
      if( DisconnectWait() )
      {
        m_pWork->m_pAppParam->nextApp = NEXT_APP_BATTLEVIDEO_SAVE;
        m_pWork->m_pAppParam->nextFrame = FRAME_InternetAfterBattle;
        m_pWork->DisconnectFlagOff( m_pWork->GetCommonData() );

        ExitSelf(RESULE_InternetAfterBattle_BattleVideo);
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
bool  Frame_InternetAfterBattle::cleanup()
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
void  Frame_InternetAfterBattle::LowerView_Selection_OnEvent(LowerView_Selection* pSender, const LowerView_Selection::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  LowerView_Selection::IEventHandler::EVENT_Continue_Next:             
      {
        ExitSelf(RESULT_InternetAfterBattle_Next);
      }
    break;
    case  LowerView_Selection::IEventHandler::EVENT_Continue_Exit:             
      {
        ExitSelf(RESULT_InternetAfterBattle_Exit);
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
void  Frame_InternetAfterBattle::ExitSelf(const FrameResult frameResult)
{
  m_pViewContainer->SetAllInputEnabled( false );
  m_Result  = frameResult;
  m_State   = STATE_Exit;
}

static u32 s_wait = 0;

bool Frame_InternetAfterBattle::DisconnectWait()
{
#if defined(GF_PLATFORM_CTR)
  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);

  if( s_wait++ > 30 )
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

void Frame_InternetAfterBattle::UpdateRankData()
{
#if defined(GF_PLATFORM_CTR)
  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE( NetLib::P2P::P2pConnectionManager );

  switch( m_seq )
  {
  case SEQ_SEND_REPORT:
    {
      // レーティングサーバーに送信
      bool ret = m_pWork->SendPglRecord();
      if( ret == true )
      {
        m_seq = SEQ_WAIT;
      }else
      {
        m_seq = SEQ_ERROR_RETURN;
      }
    }
    break;

  case SEQ_WAIT:
    // リスナー待ち
    if( m_pWork->IsSendPglRecordFinish() == true )
    {
      s_wait = 0;
#if defined(GF_PLATFORM_CTR)
      pP2pConnectionManager->DisconnectStart(m_pWork->m_pAppParam->m_isSessionLeft);
#endif
      m_State =  STATE_Dissconnect;
    }

    break;

  case SEQ_ERROR_RETURN:                     ///< 通信エラー以外のエラー
    m_seq = SEQ_WAIT;
    break;

  ///< メッセージ待ち
  case SEQ_MESSAGE_WAIT:
    m_seq = SEQ_WAIT;
    break;
  }
#endif
}




void Frame_InternetAfterBattle::SendBattleVideo()
{
#if defined(GF_PLATFORM_CTR)
  if( m_pWork->IsAutoSend() )
  {
    m_pWork->SendBattleVideo( this );
    m_State = STATE_WaitBattleVideo;
  }
  else
  {
    m_State = STATE_UpdateRank;
  }
#endif
}


//--------------------------------------------------------------------------------------------
/**
  * @brief   通信エラー
  */
//--------------------------------------------------------------------------------------------
void Frame_InternetAfterBattle::OnError( BattleVideoResponseListener::ERROR_TYPE errorType )
{
  m_State = STATE_UpdateRank;
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   アップロード成功
  */
//--------------------------------------------------------------------------------------------
void Frame_InternetAfterBattle::OnUploadSuccess( void )
{
  m_State = STATE_UpdateRank;
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   バトルビデオが不正
  */
//--------------------------------------------------------------------------------------------
void Frame_InternetAfterBattle::OnUploadInvalid( void )
{
  m_State = STATE_UpdateRank;
}




} /*  namespace Live    */
} /*  namespace NetApp  */


