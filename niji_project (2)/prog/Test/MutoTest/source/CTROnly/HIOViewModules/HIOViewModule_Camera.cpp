
#if defined(GF_PLATFORM_CTR)
#if defined(NN_SWITCH_ENABLE_HOST_IO)
#if PM_DEBUG
//==============================================================================
/**
 * @file   HIOViewModule_Camera.cpp
 * @date   2016/09/09 Fri. 19:53:58
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./HIOViewModule_Camera.h"


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
void  HIOViewModule_Camera::OnEvent(NetAppLib::Debug::HIOUtil* pSender, const NetAppLib::Debug::HIOUtil::IEventHandler::EventID eventID)
{
  switch(eventID)
  {
    case NetAppLib::Debug::HIOUtil::IEventHandler::EVENT_ReadEnd:
    {
      struct  ControlDataF32
      {
        f32 moveAroundGazePoint_Speed[2];    /**<  horizontal, vertical  */
        f32 moveEyeDirection_Speed;
        f32 moveGazePoint_Speed[2];          /**<  horizontal, vertical  */
        
        ControlDataF32(const ControlData& rFx20_12Data)
        {
          moveAroundGazePoint_Speed[0]  = Fx20_12ToF32(rFx20_12Data.fx20_12_MoveAroundGazePoint_Speed[0]);
          moveAroundGazePoint_Speed[1]  = Fx20_12ToF32(rFx20_12Data.fx20_12_MoveAroundGazePoint_Speed[1]);
          moveEyeDirection_Speed        = Fx20_12ToF32(rFx20_12Data.fx20_12_MoveEyeDirection_Speed);
          moveGazePoint_Speed[0]        = Fx20_12ToF32(rFx20_12Data.fx20_12_MoveGazePoint_Speed[0]);
          moveGazePoint_Speed[1]        = Fx20_12ToF32(rFx20_12Data.fx20_12_MoveGazePoint_Speed[1]);
        }
      };

      ControlData*    pControlData = static_cast<ControlData*>(pSender->GetData());
      ControlDataF32  f32Data(*pControlData);

      GflHeapFreeMemory(pControlData);

      m_Camera.Move_AroundGazePoint(f32Data.moveAroundGazePoint_Speed[0], f32Data.moveAroundGazePoint_Speed[1]);
      m_Camera.Move_EyeDirection(f32Data.moveEyeDirection_Speed);
      m_Camera.Move_GazePoint(f32Data.moveGazePoint_Speed[0], f32Data.moveGazePoint_Speed[1]);
    }
      break;
  }
}


} /*  namespace MutoTest  */
} /*  namespace Test      */
#endif  /*  #if defined(NN_SWITCH_ENABLE_HOST_IO)  */
#endif  /*  #if PM_DEBUG                  */
#endif  /*  #if defined(GF_PLATFORM_CTR)    */
