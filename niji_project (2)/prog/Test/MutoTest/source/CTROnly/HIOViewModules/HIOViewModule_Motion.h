

#if !defined(HIOVIEWMODULE_MOTION_H_INCLUDED)
#define HIOVIEWMODULE_MOTION_H_INCLUDED
#pragma once

#if defined(GF_PLATFORM_CTR)
#if defined(NN_SWITCH_ENABLE_HOST_IO)
#if PM_DEBUG
//==============================================================================
/**
 * @file   HIOViewModule_Motion.h
 * @date   2016/09/23 Fri. 11:51:07
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================



// =============================================================================
/**/
//==============================================================================
#include  "../../Common/EasyModel.h"
#include  "../../Common/Scene3D.h"
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
  *  @date   2016/09/23 Fri. 11:51:33
  */
//==============================================================================
class HIOViewModule_Motion
  : public  HIOViewModuleBase
{
  GFL_FORBID_COPY_AND_ASSIGN(HIOViewModule_Motion);
public:
  static const u32  moduleID  = 4;
  static const s32  channel   = 4;


  class IEventHandler
  {
  public:
    virtual void  OnEvent_AddMotion(HIOViewModule_Motion* pSender, const void* pData, const u32 modelHash, const u32 motionHash) = 0;
    virtual void  OnEvent_RemoveMotion(HIOViewModule_Motion* pSender, const u32 modelHash, const u32 motionHash) = 0;
  
    virtual ~IEventHandler(){}
  };
  void  SetHandler(IEventHandler* pHandler)         {m_pHandler   = pHandler;}
  void  SetUtilHeap(app::util::Heap* pUtilHeap)     {m_pUtilHeap  = pUtilHeap;}

  HIOViewModule_Motion(void)
    : HIOViewModuleBase(moduleID)
    , m_pUtilHeap(NULL)
    , m_pHandler(NULL)
  {}
  virtual ~HIOViewModule_Motion() {}


protected:
  /*  派生ポイント  */
  virtual s32   GetCh(void) const {return channel;}
//  virtual void  OnInitialize(gfl2::heap::HeapBase* pHeapBase);
//  virtual void  OnFinalize(void);
//  virtual void  OnUpdate(void);
  virtual void  OnEvent(NetAppLib::Debug::HIOUtil* pSender, const NetAppLib::Debug::HIOUtil::IEventHandler::EventID eventID);

  void  DataHandling(NetAppLib::Debug::HIOUtil* pHIOUtil);


  app::util::Heap*  m_pUtilHeap;
  IEventHandler*    m_pHandler;

private:
};


} /*  namespace MutoTest  */
} /*  namespace Test      */
#endif  /*  #if defined(NN_SWITCH_ENABLE_HOST_IO)  */
#endif  /*  #if PM_DEBUG                  */
#endif  /*  #if defined(GF_PLATFORM_CTR)    */
#endif  /*  #if !defined(HIOVIEWMODULE_MOTION_H_INCLUDED)  */
