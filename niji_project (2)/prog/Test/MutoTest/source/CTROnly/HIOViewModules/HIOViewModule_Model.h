#if !defined(HIOVIEWMODULE_MODEL_H_INCLUDED)
#define HIOVIEWMODULE_MODEL_H_INCLUDED
#pragma once

#if defined(GF_PLATFORM_CTR)
#if defined(NN_SWITCH_ENABLE_HOST_IO)
#if PM_DEBUG
//==============================================================================
/**
 * @file   HIOViewModule_Model.h
 * @date   2016/09/14 Wed. 17:55:32
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
  *  @date   2016/09/14 Wed. 17:56:33
  */
//==============================================================================
class HIOViewModule_Model
  : public  HIOViewModuleBase
{
  GFL_FORBID_COPY_AND_ASSIGN(HIOViewModule_Model);
public:
  static const u32  moduleID  = 1;
  static const s32  channel   = 1;


  class IEventHandler
  {
  public:
    virtual void  OnEvent_AddModel(HIOViewModule_Model* pSender, EasyModel& rEasyModel) = 0;
    virtual void  OnEvent_RemoveModel(HIOViewModule_Model* pSender, EasyModel& rEasyModel) = 0;
  
    virtual ~IEventHandler(){}
  };
  void  SetHandler(IEventHandler* pHandler)         {m_pHandler   = pHandler;}
  void  SetUtilHeap(app::util::Heap* pUtilHeap)     {m_pUtilHeap  = pUtilHeap;}

  HIOViewModule_Model(const u32 modelCapacity)
    : HIOViewModuleBase(moduleID)
    , m_pUtilHeap(NULL)
    , m_pModelPool(NULL)
    , m_pHandler(NULL)
    , m_ModelCapacity(modelCapacity)
  {}
  virtual ~HIOViewModule_Model() {}


  void  DisposeModel(EasyModel* pModel) {if(m_pModelPool) m_pModelPool->Dispose(pModel);}
  void  DisposeAll(void);


  EasyModel*  FindModel(const u32 hash = 0u)  {return (m_pModelPool ? m_pModelPool->FindBusyModel(hash) : NULL);}


protected:
  /*  派生ポイント  */
  virtual s32   GetCh(void) const {return channel;}
  virtual void  OnInitialize(gfl2::heap::HeapBase* pHeapBase);
  virtual void  OnFinalize(void);
  virtual void  OnUpdate(void);
  virtual void  OnEvent(NetAppLib::Debug::HIOUtil* pSender, const NetAppLib::Debug::HIOUtil::IEventHandler::EventID eventID);

  void  DataHandling(NetAppLib::Debug::HIOUtil* pHIOUtil);


  app::util::Heap*  m_pUtilHeap;
  EasyModelPool*    m_pModelPool;
  IEventHandler*    m_pHandler;
  u32               m_ModelCapacity;

private:
  HIOViewModule_Model(void): HIOViewModuleBase(0){}
};


} /*  namespace MutoTest  */
} /*  namespace Test      */
#endif  /*  #if defined(NN_SWITCH_ENABLE_HOST_IO)  */
#endif  /*  #if PM_DEBUG                  */
#endif  /*  #if defined(GF_PLATFORM_CTR)    */
#endif  /*  #if !defined(HIOVIEWMODULE_MODEL_H_INCLUDED)  */
