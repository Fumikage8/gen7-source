#if !defined(HIOVIEWMODULE_CAMERA_H_INCLUDED)
#define HIOVIEWMODULE_CAMERA_H_INCLUDED
#pragma once

#if defined(GF_PLATFORM_CTR)
#if defined(NN_SWITCH_ENABLE_HOST_IO)
#if PM_DEBUG
//==============================================================================
/**
 * @file   HIOViewModule_Camera.h
 * @date   2016/09/09 Fri. 19:46:20
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../../Common/EasyCamera.h"
#include  "./HIOViewModuleBase.h"


namespace Test  {
namespace MutoTest  {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/09/09 Fri. 19:48:05
  */
//==============================================================================
class HIOViewModule_Camera
  : public  HIOViewModuleBase
{
  GFL_FORBID_COPY_AND_ASSIGN(HIOViewModule_Camera);
public:
  static const u32  moduleID  = 2;
  static const s32  channel   = 2;


  struct  ControlData
  {
    s32 fx20_12_MoveAroundGazePoint_Speed[2];    /**<  horizontal, vertical  */
    s32 fx20_12_MoveEyeDirection_Speed;
    s32 fx20_12_MoveGazePoint_Speed[2];          /**<  horizontal, vertical  */
  };


  HIOViewModule_Camera(void)
    : HIOViewModuleBase(moduleID)
    , m_Camera()
  {}
  virtual ~HIOViewModule_Camera() {}


  EasyCamera&  GetEasyCamera(void)
  {
    return  m_Camera;
  }



protected:
  /*  派生ポイント  */
  virtual s32   GetCh(void) const {return channel;}
//  virtual void  OnUpdate(void);
  virtual void  OnEvent(NetAppLib::Debug::HIOUtil* pSender, const NetAppLib::Debug::HIOUtil::IEventHandler::EventID eventID);

  EasyCamera    m_Camera;
};



} /*  namespace MutoTest  */
} /*  namespace Test      */
#endif  /*  #if defined(NN_SWITCH_ENABLE_HOST_IO)  */
#endif  /*  #if PM_DEBUG                  */
#endif  /*  #if defined(GF_PLATFORM_CTR)    */
#endif  /*  #if !defined(HIOVIEWMODULE_CAMERA_H_INCLUDED)  */
