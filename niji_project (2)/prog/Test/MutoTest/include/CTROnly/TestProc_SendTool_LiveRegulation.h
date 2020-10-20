#if !defined(TESTPROC_SENDTOOL_LIVEREGULATION_H_INCLUDED)
#define TESTPROC_SENDTOOL_LIVEREGULATION_H_INCLUDED
#pragma once
#if PM_DEBUG
//==============================================================================
/**
 * @file   TestProc_SendTool_LiveRegulation.h
 * @date   2016/01/06 Wed. 15:26:09
 * @author muto_yu
 * @brief  ライブ大会レギュレーション送信ツール
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "GameSys/include/GameProc.h"


#include  "AppLib/include/Util/app_util_heap.h"
#include  "NetStatic/NetAppLib/include/Live/LiveNet.h"
#include  "../../source/Common/TextDrawer.h"


namespace Test      {
namespace MutoTest  {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/01/06 Wed. 15:27:37
  */
//==============================================================================
#if defined(GF_PLATFORM_CTR)


class TestProc_SendTool_LiveRegulation
  : public    GameSys::GameProc
  , protected NetApp::Live::LiveNet::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN(TestProc_SendTool_LiveRegulation);
public:
  TestProc_SendTool_LiveRegulation(void);
  virtual ~TestProc_SendTool_LiveRegulation();


  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager);
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pManager);
  virtual void PreDrawFunc( gfl2::proc::Manager* pManager );
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);


private:
  void  CreateHeap(void);
  void  DeleteHeap(void);

  /*  implement NetApp::Live::LiveNet::IEventHandler  */
  virtual void  OnReceive_Regulation(     NetApp::Live::LiveNet* pSender, const NetApp::Live::SerializedRegulation& rSerializedRegulation );
  virtual void  OnReceive_LiveBattleInfo( NetApp::Live::LiveNet* pSender, const NetApp::Live::LiveBattleInfo& rMyLiveBattleInfo, const NetApp::Live::LiveBattleInfo& rOpponentLiveBattleInfo);

  struct  ToolInfo;


  app::util::Heap         m_Heap;
  TextDrawer              m_TextDrawer;
  u32                     m_State;

  ToolInfo*               m_pToolInfo;

  NetApp::Live::LiveNet*  m_pLiveNet;
};


#else   /*  #if defined(GF_PLATFORM_CTR)    */
/*
   for notCTR
*/
class TestProc_SendTool_LiveRegulation
  : public TestProc
{
  GFL_FORBID_COPY_AND_ASSIGN(TestProc_SendTool_LiveRegulation);
public:
  TestProc_SendTool_LiveRegulation(void){}
  /** dtor  */
  virtual ~TestProc_SendTool_LiveRegulation(){}
};
#endif  /*  #if defined(GF_PLATFORM_CTR)    */




} /*  namespace MutoTest  */
} /*  namespace Test      */
#endif  /*  #if PM_DEBUG  */
#endif  /*  #if !defined(TESTPROC_SENDTOOL_LIVEREGULATION_H_INCLUDED)  */
