
#if !defined(LIVEFRAME_MATCHING_H_INCLUDED)
#define LIVEFRAME_MATCHING_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LiveFrame_Matching.h
 * @date   2015/07/28 Tue. 11:20:28
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
#include  "NetStatic/NetAppLib/include/Live/LiveNet.h"
#include  "../LiveFrameDef.h"
#include  "../Views/Base/UpperView_Main.h"
#include  "../Views/Base/LowerView_Selection.h"

// バトル
#include "Battle/include/battle_SetupParam.h"
#include  "NetStatic/NetEvent/include/NetworkBattleEvent.h"       /*  netbattle   */

namespace NetApp  {
namespace Live {
  class LiveWork;
  class LiveViewContainer;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  Frame_Matching
  *  @brief  Live大会Frame:main
  */
//==============================================================================
class Frame_Matching
  : public    NetAppLib::System::NetAppFrameBase
  , protected LowerView_Selection::IEventHandler
  , protected LiveNet::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_Matching );

public:

  //------------------------------------------------------------------
  /**
    *  @func     Frame_Matching
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  Frame_Matching(LiveWork* pWork);

  //------------------------------------------------------------------
  /**
    *  @func     ~Frame_Matching
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~Frame_Matching();


private:
  /*
    override  NetAppFrameBase I/F
  */
  virtual bool  startup();
  virtual void  setListener();
  virtual void  removeListener();
  virtual void  updateSequence();
  virtual bool  cleanup();

  void  ExitFrame(const FrameResult = RESULT_MATCHING_Cancel);

  /*  implement LowerView_Selection::IEventHandler  */
  virtual void  LowerView_Selection_OnEvent(const EventCode eventCode);
  /*  endimplement LowerView_Selection::IEventHandler  */

  /*  implement LiveNet::IEventHandler  */
  virtual void  OnReceive_Regulation(     LiveNet* pSender, const SerializedRegulation& rSerializedRegulation ) {};
  virtual void  OnReceive_LiveBattleInfo( LiveNet* pSender, const LiveBattleInfo& rMyLiveBattleInfo, const LiveBattleInfo& rOpponentLiveBattleInfo);

  void  SendLiveRecInitParam( void );

  /*
    members
  */
  LiveWork*             m_pWork;
  LiveViewContainer*    m_pViewContainer;
  u32                   m_State;
  FrameResult           m_Result;

  UpperView_Main*       m_pUpperView_Main;
  LowerView_Selection*  m_pLowerView_Selection;

  BATTLE_SETUP_PARAM    m_battleSetupParam;
  NetEvent::NetworkBattle::EVENT_PARAM          m_NetworkBattleParam;   /**<  networkbattle   */
};  /*  class LiveFrame  */





} /*  namespace Live */
} /*  namespace NetApp      */
#endif  /*  #if !defined(LIVEFRAME_MATCHING_H_INCLUDED)  */

