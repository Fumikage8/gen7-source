#if !defined(GIFTFRAME_RECEIVESERIAL_INPUTCODE_H_INCLUDED)
#define GIFTFRAME_RECEIVESERIAL_INPUTCODE_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   GiftFrame_ReceiveSerial_InputCode.h
 * @date   2015/09/01 Tue. 20:00:00
 * @author muto_yu
 * @brief  
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
#include  "../../GiftFrameDef.h"

#include  "../../Views/GiftViewContainer.h"


namespace NetApp  {
namespace Gift {
  class GiftWork;
  class GiftViewContainer;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  Frame_ReceiveSerial_InputCode
  */
//==============================================================================
class Frame_ReceiveSerial_InputCode
  : public NetAppLib::System::NetAppFrameBase
  , public LowerView_Back::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_ReceiveSerial_InputCode );

public:

  //------------------------------------------------------------------
  /**
    *  @func     Frame_ReceiveSerial_InputCode
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  Frame_ReceiveSerial_InputCode(GiftWork* pWork);

  //------------------------------------------------------------------
  /**
    *  @func     ~Frame_ReceiveSerial_InputCode
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~Frame_ReceiveSerial_InputCode();



private:
  /*
    override  NetAppFrameBase I/F
  */
  virtual bool  startup();
  virtual void  setListener();
  virtual void  removeListener();
  virtual void  updateSequence();
  virtual bool  cleanup();

  /*  implement LowerView_Back::IEventHandler  */
  virtual void  LowerView_Back_OnEvent(const LowerView_Back::IEventHandler::EventCode eventCode);


  void  ExitFrame(const FrameResult = RESULT_RECEIVESERIAL_INPUTCODE_Exit);




  /*
    members
  */
  GiftWork*             m_pWork;
  GiftViewContainer*    m_pViewContainer;
  u32                   m_State;
  FrameResult           m_Result;

  LowerView_Back*       m_pLowerView_Back;
};  /*  class GiftFrame  */





} /*  namespace Gift */
} /*  namespace NetApp      */
#endif  /*  #if !defined(GIFTFRAME_RECEIVESERIAL_INPUTCODE_H_INCLUDED)  */
