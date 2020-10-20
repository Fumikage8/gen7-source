
#if !defined(GIFTFRAME_RECEIVERESULT_H_INCLUDED)
#define GIFTFRAME_RECEIVERESULT_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   GiftFrame_ReceiveResult.h
 * @date   2015/08/20 Thu. 16:01:45
 * @author muto_yu
 * @brief  受信結果
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
#include  "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include  "../GiftFrameDef.h"

#include  "../Views/GiftViewContainer.h"

namespace NetApp  {
namespace Gift {
  class GiftWork;
  class GiftViewContainer;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  */
//==============================================================================
class Frame_ReceiveResult
  : public    NetAppLib::System::NetAppFrameBase
  , protected LowerView_Message::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_ReceiveResult );

public:









  Frame_ReceiveResult(GiftWork* pWork);
  virtual ~Frame_ReceiveResult();


private:
  /*  LowerView_Message  */
  virtual void  LowerView_Message_OnEvent(LowerView_Message* pSender, const LowerView_Message::IEventHandler::EventCode eventCode);

  /*
    override  NetAppFrameBase I/F
  */
  virtual bool  startup();
  virtual void  setListener();
  virtual void  removeListener();
  virtual void  updateSequence();
  virtual bool  cleanup();

  void  ExitFrame(const FrameResult = RESULT_RECEIVERESULT_Exit);


  /*
    members
  */
  GiftWork*               m_pWork;
  GiftViewContainer*      m_pViewContainer;
  u32                     m_State;
  FrameResult             m_Result;

  u32                     m_WaitFrame;

  UpperView_Card*         m_pUpperView_Card;
  LowerView_Message*      m_pLowerView_Message;
};  /*  class GiftFrame  */





} /*  namespace Gift */
} /*  namespace NetApp      */
#endif  /*  #if !defined(GIFTFRAME_RECEIVERESULT_H_INCLUDED)  */
