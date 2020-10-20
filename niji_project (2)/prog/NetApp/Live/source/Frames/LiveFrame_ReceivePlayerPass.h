#if !defined(LIVEFRAME_RECEIVEPLAYERPASS_H_INCLUDED)
#define LIVEFRAME_RECEIVEPLAYERPASS_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LiveFrame_ReceivePlayerPass.h
 * @date   2015/07/24 Fri. 11:49:11
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
#include  "../LiveFrameDef.h"
#include  "../Views/Base/UpperView_Message.h"
#include  "../Views/Base/LowerView_Selection.h"


namespace NetApp  {
namespace Live    {
// =============================================================================
/**/
//==============================================================================

//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/07/24 Fri. 11:49:48
  */
//==============================================================================
class Frame_ReceivePlayerPass
  : public    NetAppLib::System::NetAppFrameBase
  , protected LowerView_Selection::IEventHandler
  , protected NetApp::Live::LiveNet::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_ReceivePlayerPass );

public:
  Frame_ReceivePlayerPass(LiveWork* pWork);
  virtual ~Frame_ReceivePlayerPass();



private:
  /*
    override  NetAppFrameBase I/F
  */
  virtual bool  startup();
  virtual void  setListener();
  virtual void  removeListener();
  virtual void  updateSequence();
  virtual bool  cleanup();

  void  ExitFrame(const FrameResult = RESULT_RECEIVE_PLAYER_PASS_Exit);
  bool  CheckForcedOverwriteRegulation(void);

  /*  implement LowerView_Selection::IEventHandler  */
  virtual void  LowerView_Selection_OnEvent(const EventCode eventCode);
  /*  endimplement LowerView_Selection::IEventHandler  */


  /*  implement NetApp::Live::LiveNet::IEventHandler  */
  virtual void  OnReceive_Regulation(     NetApp::Live::LiveNet* pSender, const NetApp::Live::SerializedRegulation& rSerializedRegulation );
  virtual void  OnReceive_LiveBattleInfo( NetApp::Live::LiveNet* pSender, const NetApp::Live::LiveBattleInfo& rMyLiveBattleInfo, const NetApp::Live::LiveBattleInfo& rOpponentLiveBattleInfo) {}
  
  /*  QR読み込み用OnReceive  */
  void OnReceive_QRRegulation(void);

  void  RegulationCheckResultHandling(void);

  
  enum  ReceiveResult
  {
    RECVRESULT_Received,
    RECVRESULT_RegulationPassed,

    RECVRESULT_Failed_Cancel,
    RECVRESULT_Failed_InvalidROMVersion,
    RECVRESULT_Failed_HaveParticipated,
    RECVRESULT_Failed_NetworkError,

    RECVRESULT_Initial
  };




  /*
    members
  */
  LiveWork*             m_pWork;
  LiveViewContainer*    m_pViewContainer;
  u32                   m_State;
  FrameResult           m_Result;

  UpperView_Message*    m_pUpperView_Message;
  LowerView_Selection*  m_pLowerView_Selection;
  
  ReceiveResult         m_ReceiveResult;
};


} /*  namespace Live    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(LIVEFRAME_RECEIVEPLAYERPASS_H_INCLUDED)  */
