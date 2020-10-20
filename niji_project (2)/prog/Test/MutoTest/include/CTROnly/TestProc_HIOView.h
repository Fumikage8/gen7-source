
#if !defined(TESTPROC_HIOVIEW_H_INCLUDED)
#define TESTPROC_HIOVIEW_H_INCLUDED
#pragma once

#if PM_DEBUG
//==============================================================================
/**
 * @file   TestProc_HIOView.h
 * @date   2016/09/06 Tue. 15:34:41
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_Macros.h>
#include  <GameSys/include/GameProcManager.h>
#include  <AppLib/include/Frame/AppFrameManager.h>
#include  <AppLib/include/Util/app_util_heap.h>
#include  <AppLib/include/Util/AppRenderingManager.h>

#if defined(GF_PLATFORM_CTR)
  #include  <nn/hio.h>
#endif


#include  "../MutoTest_TestProc.h"
#include  "../../source/Common/Scene3D.h"
#include  "../../source/Common/TextDrawer.h"

#include  "../../source/CTROnly/HIOViewModules/HIOViewModule_Camera.h"
#include  "../../source/CTROnly/HIOViewModules/HIOViewModule_Light.h"
#include  "../../source/CTROnly/HIOViewModules/HIOViewModule_Model.h"
#include  "../../source/CTROnly/HIOViewModules/HIOViewModule_Motion.h"

namespace Test  {
namespace MutoTest  {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/07/28 Tue. 19:11:21
  */
//==============================================================================
#if defined(GF_PLATFORM_CTR) && defined(NN_SWITCH_ENABLE_HOST_IO)
/*
   for CTR
*/
class TestProc_HIOView
  : public    GameSys::GameProc
  , protected HIOViewModule_Model::IEventHandler
  , protected HIOViewModule_Motion::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN(TestProc_HIOView);

public:
  /** ctor  */
  TestProc_HIOView(void);

  /** dtor  */
  virtual ~TestProc_HIOView();


  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager);
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pManager);
  virtual void PreDrawFunc( gfl2::proc::Manager* pManager );
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);



private:
  void  CreateHeap(void);
  void  DeleteHeap(void);
  /*  HIOViewModule_Model::IEventHandler  */
  virtual void  OnEvent_AddModel(HIOViewModule_Model* pSender, EasyModel& rEasyModel);
  virtual void  OnEvent_RemoveModel(HIOViewModule_Model* pSender, EasyModel& rEasyModel);
  /*  HIOViewModule_Motion::IEventHandler  */
  virtual void  OnEvent_AddMotion(HIOViewModule_Motion* pSender, const void* pData, const u32 modelHash, const u32 motionHash);
  virtual void  OnEvent_RemoveMotion(HIOViewModule_Motion* pSender, const u32 modelHash, const u32 motionHash);



  app::util::Heap                   m_Heap;
  Scene3D                           m_Scene3D;
  TextDrawer                        m_TextDrawer;
  u32                               m_UpdateSeq;
  /*  HIOUtil  */
  HIOViewModule_Camera              m_HIOUtil_Camera;
  HIOViewModule_Light               m_HIOUtil_Light;
  HIOViewModule_Model               m_HIOUtil_Model;
  HIOViewModule_Motion              m_HIOUtil_Motion;

  u32                               m_FrameCount;

  /*  debug  */
  gfl2::debug::DebugWinGroup*       m_pGroup_Root;
};



#else   /*  #if defined(GF_PLATFORM_CTR) && defined(NN_SWITCH_ENABLE_HOST_IO)    */
/*
   for notCTR
*/
class TestProc_HIOView : public TestProc
{
  GFL_FORBID_COPY_AND_ASSIGN(TestProc_HIOView);
public:
  TestProc_HIOView(void){}
  /** dtor  */
  virtual ~TestProc_HIOView(){}
};

#endif  /*  #if defined(GF_PLATFORM_CTR) && defined(NN_SWITCH_ENABLE_HOST_IO)    */



} /*  namespace MutoTest  */
} /*  namespace Test  */
#endif  /*  #if PM_DEBUG                  */

#endif  /*  #if !defined(TESTPROC_HIOVIEW_H_INCLUDED)  */
