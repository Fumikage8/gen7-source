#if !defined(HIOVIEWMODULE_LIGHT_H_INCLUDED)
#define HIOVIEWMODULE_LIGHT_H_INCLUDED
#pragma once

#if defined(GF_PLATFORM_CTR)
#if defined(NN_SWITCH_ENABLE_HOST_IO)
#if PM_DEBUG
//==============================================================================
/**
 * @file   HIOViewModule_Light.h
 * @date   2016/09/14 Wed. 11:09:23
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================



// =============================================================================
/**/
//==============================================================================
#include  "../../Common/EasyLight.h"
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
  *  @date   2016/09/14 Wed. 11:09:51
  */
//==============================================================================
class HIOViewModule_Light
  : public  HIOViewModuleBase
{
  GFL_FORBID_COPY_AND_ASSIGN(HIOViewModule_Light);
public:
  static const u32  moduleID  = 3;
  static const s32  channel   = 3;

  struct  ControlData
  {
    u32 lightID;

    s32 fx20_12_Color_R;
    s32 fx20_12_Color_G;
    s32 fx20_12_Color_B;

    s32 fx20_12_Direction_X;
    s32 fx20_12_Direction_Y;
    s32 fx20_12_Direction_Z;
  };


  HIOViewModule_Light(void)
    : HIOViewModuleBase(moduleID)
    , m_Light()
  {}
  virtual ~HIOViewModule_Light() {}


  EasyLight&  GetEasyLight(void)  {return m_Light;}


protected:
  /*  派生ポイント  */
  virtual s32   GetCh(void) const {return channel;}
//  virtual void  OnUpdate(void);
  virtual void  OnEvent(NetAppLib::Debug::HIOUtil* pSender, const NetAppLib::Debug::HIOUtil::IEventHandler::EventID eventID);

  EasyLight   m_Light;

};


} /*  namespace MutoTest  */
} /*  namespace Test      */
#endif  /*  #if defined(NN_SWITCH_ENABLE_HOST_IO)  */
#endif  /*  #if PM_DEBUG                  */
#endif  /*  #if defined(GF_PLATFORM_CTR)    */
#endif  /*  #if !defined(HIOVIEWMODULE_LIGHT_H_INCLUDED)  */
