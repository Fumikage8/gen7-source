
#if defined(GF_PLATFORM_CTR)
#if defined(NN_SWITCH_ENABLE_HOST_IO)
#if PM_DEBUG
//==============================================================================
/**
 * @file   HIOViewModule_Light.cpp
 * @date   2016/09/14 Wed. 15:22:19
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./HIOViewModule_Light.h"


namespace Test  {
namespace MutoTest  {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  HIOViewModule_Light::OnEvent(NetAppLib::Debug::HIOUtil* pSender, const NetAppLib::Debug::HIOUtil::IEventHandler::EventID eventID)
{
  switch(eventID)
  {
    case NetAppLib::Debug::HIOUtil::IEventHandler::EVENT_ReadEnd:
    {
      struct  ControlDataF32
      {
        u32 lightID;

        f32 color_R;
        f32 color_G;
        f32 color_B;

        f32 direction_X;
        f32 direction_Y;
        f32 direction_Z;

        ControlDataF32(const ControlData& rFx20_12Data)
        {
          lightID     = rFx20_12Data.lightID;

          color_R     = rFx20_12Data.fx20_12_Color_R;
          color_G     = rFx20_12Data.fx20_12_Color_G;
          color_B     = rFx20_12Data.fx20_12_Color_B;

          direction_X = rFx20_12Data.fx20_12_Direction_X;
          direction_Y = rFx20_12Data.fx20_12_Direction_Y;
          direction_Z = rFx20_12Data.fx20_12_Direction_Z;
        }
      };

      ControlData*    pControlData = static_cast<ControlData*>(pSender->GetData());
      ControlDataF32  f32Data(*pControlData);

      GFL_PRINT("recv light");
      GflHeapFreeMemory(pControlData);
    }
      break;
  }
}


} /*  namespace MutoTest  */
} /*  namespace Test      */
#endif  /*  #if defined(NN_SWITCH_ENABLE_HOST_IO)  */
#endif  /*  #if PM_DEBUG                  */
#endif  /*  #if defined(GF_PLATFORM_CTR)    */
