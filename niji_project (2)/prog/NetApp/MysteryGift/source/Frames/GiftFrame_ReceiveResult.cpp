
//==============================================================================
/**
 * @file   GiftFrame_ReceiveResult.cpp
 * @date   2015/08/20 Thu. 16:15:42
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

#include  "./GiftFrame_ReceiveResult.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    /*  init  */
    STATE_Entry = 0,
    STATE_Main,
    STATE_Wait,
    STATE_Exit,

    STATE_     /*  terminator  */
  };

  static const u32  waitFrame = (3 * 30);
} /*  namespace  */


namespace NetApp  {
namespace Gift {
// =============================================================================
/**/
//==============================================================================



//------------------------------------------------------------------
/**
  *  @func     Frame_ReceiveResult
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_ReceiveResult::Frame_ReceiveResult(GiftWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->GetViewContainer())
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_WaitFrame(waitFrame)
  , m_pUpperView_Card(NULL)
  , m_pLowerView_Message(NULL)

{
  m_pUpperView_Card     = m_pViewContainer->m_pUpperView_Card;
  m_pLowerView_Message  = m_pViewContainer->m_pLowerView_Message;
}


//------------------------------------------------------------------
/**
  *  @func     Frame_ReceiveResult
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_ReceiveResult::~Frame_ReceiveResult()
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
bool  Frame_ReceiveResult::startup()
{
  const Savedata::MysteryGiftSave::MYSTERY_GIFT_DATA*       pGiftData     = GetMysteryGiftSave()->GetLastGiftData();

  GFL_ASSERT(pGiftData);

  m_pViewContainer->SetScene(GiftViewContainer::SCENE_ReceiveResult);
  m_pViewContainer->SetAllInputEnable(true);

  m_pUpperView_Card->UpdateCardInfo(pGiftData);
  m_pUpperView_Card->HideMessage();
  m_pLowerView_Message->UpdateReceivedDataInfo(m_pWork->m_pAppParam->GetLastRemovedDeliveryData(), false);
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
void  Frame_ReceiveResult::setListener()
{
  m_pLowerView_Message->SetEventHandler(this);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_ReceiveResult::removeListener()
{
  m_pUpperView_Card->HidePokeModel();
  m_pLowerView_Message->SetEventHandler(NULL);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Frame_ReceiveResult::updateSequence()
{

  switch(m_State)
  {
    case  STATE_Entry:
      m_State = STATE_Main;
      /*  fallthrough  */

    case  STATE_Main:
    {
      if(m_WaitFrame)
      {
        --m_WaitFrame;
      }
      
      if(!m_WaitFrame)
      {
        m_pUpperView_Card->ShowMessage(UpperView_Card::MESSAGEMODE_Exit);

        m_State = STATE_Wait;
      }
    }
      break;


    case  STATE_Wait:
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
bool  Frame_ReceiveResult::cleanup()
{
  return true;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_ReceiveResult::ExitFrame(const FrameResult frameResult)
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
void  Frame_ReceiveResult::LowerView_Message_OnEvent(LowerView_Message* pSender, const LowerView_Message::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  LowerView_Message::IEventHandler::EVENT_Exit:
    {
      if(m_State  == STATE_Wait)
      {
        /*  ここで受け取りデータをリストから削除  */
        if(!m_pWork->m_pAppParam->GetReceivedDataNum() || m_pWork->m_pAppParam->m_bReceiveOneData)
        {
          /*  データがなくなった or ひとつしか受け取れないモード  */
          ExitFrame(RESULT_RECEIVERESULT_Nodata);
        }
        else
        {
          ExitFrame();
        }
        m_pUpperView_Card->HideMessage();
        Sound::PlaySE(SEID_DECIDE);
      }
    }
      break;
  }
}



} /*  namespace Gift    */
} /*  namespace NetApp  */
