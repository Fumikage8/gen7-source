#if !defined(HIOVIEWMODULEBASE_H_INCLUDED)
#define HIOVIEWMODULEBASE_H_INCLUDED
#pragma once

#if defined(GF_PLATFORM_CTR)
#if defined(NN_SWITCH_ENABLE_HOST_IO)
#if PM_DEBUG
//==============================================================================
/**
 * @file   HIOViewModuleBase.h
 * @date   2016/09/09 Fri. 19:24:30
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <NetStatic/NetAppLib/include/Debug/NetAppHIOUtil.h>


namespace Test  {
namespace MutoTest  {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/09/09 Fri. 19:25:14
  */
//==============================================================================
class HIOViewModuleBase
  : protected NetAppLib::Debug::HIOUtil::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN(HIOViewModuleBase);
public:
  class IEventHandler
  {
  public:
    virtual void  OnEvent(HIOViewModuleBase* pSender, const u32 eventID) =0;    /**<  pSender->GetModuleID()でどのModuleからのEventであるのかを識別する  */
    virtual ~IEventHandler(){}
  };
  void  SetHandler(IEventHandler* pHandler)         {m_pHandler = pHandler;}

  void  Initialize(gfl2::heap::HeapBase* pHeapBase) {m_HIOUtil.Initialize(pHeapBase, GetCh()); m_HIOUtil.SetHandler(this); OnInitialize(pHeapBase);}
  void  Finalize(void)                              {m_HIOUtil.Finalize(); OnFinalize();}
  void  Update(void)                                {m_HIOUtil.Update(); OnUpdate();}
  u32   GetModuleID(void) const                     {return m_ModuleID;}
  void* GetData(u32* pDataBytes=NULL)               {return m_HIOUtil.GetData(pDataBytes);}
  int   GetHIOUtilState(void) const                 {return m_HIOUtil.GetState();}
  //------------------------------------------------------------------
  /**
    *  @brief    受信データ情報の取得
    *  @note     有効期間はOnEvent(EVENT_ReadEnd)後、GetData()の後のUpdateまで
    */
  //------------------------------------------------------------------
  const NetAppLib::Debug::HIOUtil::DataInfo& GetHIODataInfo(void) const {return m_HIOUtil.GetDataInfo();}

  

  static f32  Fx20_12ToF32(const s32 fx20_12Val) {return (static_cast<f32>(fx20_12Val) / static_cast<f32>(0x01 << 12));}

protected:
  /*  派生ポイント  */
  virtual s32   GetCh(void) const                             {return 1;}
  virtual void  OnInitialize(gfl2::heap::HeapBase* pHeapBase) {}
  virtual void  OnFinalize(void)                              {}
  virtual void  OnUpdate(void)                                {}
  virtual void  OnEvent(NetAppLib::Debug::HIOUtil* pSender, const NetAppLib::Debug::HIOUtil::IEventHandler::EventID eventID){}

  HIOViewModuleBase(const u32 moduleID)
    : m_HIOUtil() 
    , m_ModuleID(moduleID)
    , m_pHandler(NULL)
  {}
  virtual ~HIOViewModuleBase()    {Finalize();}
  void  Event(const u32 eventID)  {if(m_pHandler) m_pHandler->OnEvent(this, eventID);}

  NetAppLib::Debug::HIOUtil m_HIOUtil;
  u32                       m_ModuleID;
  IEventHandler*            m_pHandler;

private:
  HIOViewModuleBase(void){}
};


} /*  namespace MutoTest  */
} /*  namespace Test      */
#endif  /*  #if defined(NN_SWITCH_ENABLE_HOST_IO)  */
#endif  /*  #if PM_DEBUG                  */
#endif  /*  #if defined(GF_PLATFORM_CTR)    */
#endif  /*  #if !defined(HIOVIEWMODULEBASE_H_INCLUDED)  */
