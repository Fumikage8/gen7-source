#if !defined(TESTPROC_DEVICECAMERA_H_INCLUDED)
#define TESTPROC_DEVICECAMERA_H_INCLUDED
#pragma once
#if PM_DEBUG
//==============================================================================
/**
 * @file   TestProc_DeviceCamera.h
 * @date   2015/07/28 Tue. 19:08:44
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

#include  "AppLib/include/Ui/UIView.h"


#include  "../MutoTest_TestProc.h"
#include  "../../source/Common/TextDrawer.h"


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
#if defined(GF_PLATFORM_CTR)
/*
   for CTR
*/


class CameraImageView
  : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(CameraImageView);

public:
  CameraImageView(app::util::Heap * heap, app::util::AppRenderingManager* pRenderManager, void* pLayoutBuffer);
  virtual ~CameraImageView();

  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

private:
  app::util::AppRenderingManager*   m_pRenderManager;
};




class TestProc_DeviceCamera
  : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN(TestProc_DeviceCamera);

public:
  /** ctor  */
  TestProc_DeviceCamera(void);

  /** dtor  */
  virtual ~TestProc_DeviceCamera();


  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager);
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pManager);
  virtual void PreDrawFunc( gfl2::proc::Manager* pManager );
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);



private:
  void  CreateHeap(void);
  void  DeleteHeap(void);


  app::util::Heap                   m_Heap;
  TextDrawer                        m_TextDrawer;
  u32                               m_UpdateSeq;

  /*  layout  */
  void*                             m_pLayoutBuffer;
  app::util::AppRenderingManager*   m_pRenderManager;
  CameraImageView*                  m_pCameraImageView;
};



#else   /*  #if defined(GF_PLATFORM_CTR)    */
/*
   for notCTR
*/
class TestProc_DeviceCamera : public TestProc
{
  GFL_FORBID_COPY_AND_ASSIGN(TestProc_DeviceCamera);
public:
  TestProc_DeviceCamera(void){}
  /** dtor  */
  virtual ~TestProc_DeviceCamera(){}
};

#endif  /*  #if defined(GF_PLATFORM_CTR)    */



} /*  namespace MutoTest  */
} /*  namespace Test  */
#endif  /*  #if PM_DEBUG  */
#endif  /*  #if !defined(TESTPROC_DEVICECAMERA_H_INCLUDED)  */
