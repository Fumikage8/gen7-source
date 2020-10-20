
#if defined(GF_PLATFORM_CTR)
#if defined(NN_SWITCH_ENABLE_HOST_IO)
#if PM_DEBUG
//==============================================================================
/**
 * @file   HIOViewModule_Motion.cpp
 * @date   2016/09/23 Fri. 15:26:02
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./HIOViewModule_Motion.h"


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
void  HIOViewModule_Motion::OnEvent(NetAppLib::Debug::HIOUtil* pSender, const NetAppLib::Debug::HIOUtil::IEventHandler::EventID eventID)
{
  if(!m_pUtilHeap)  return;

  switch(eventID)
  {
    case NetAppLib::Debug::HIOUtil::IEventHandler::EVENT_ReadEnd:
    {
      DataHandling(pSender);
    }
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  HIOViewModule_Motion::DataHandling(NetAppLib::Debug::HIOUtil* pHIOUtil)
{
  const NetAppLib::Debug::HIOUtil::DataInfo&  rDataInfo = pHIOUtil->GetDataInfo();

  switch(rDataInfo.userdata[0])
  {
    /*
      addMotion
      [0]:0
      [1]:modelHash
      [2]:motionHash
    */
    case  0:
    {
      void* pData = pHIOUtil->GetData();

      GFL_PRINT("add Motion\n");
      if(m_pHandler)
      {
        const u32 modelHash   = rDataInfo.userdata[1];
        const u32 motionHash  = rDataInfo.userdata[2];

        m_pHandler->OnEvent_AddMotion(this, pData, modelHash, motionHash);
      }
      else
      {
        GflHeapFreeMemory(pData);
      }
    }
      break;
      
    /*
      deleteMotion
      [0]:1
      [1]:modelHash
      [2]:motionHash
    */
    case  1:
    {
      void* pData = pHIOUtil->GetData();

      GFL_PRINT("remove Motion\n");
      if(m_pHandler)
      {
        const u32 modelHash   = rDataInfo.userdata[1];
        const u32 motionHash  = rDataInfo.userdata[2];

        m_pHandler->OnEvent_RemoveMotion(this, modelHash, motionHash);
      }
      else
      {
        GflHeapFreeMemory(pData);
      }
        
    }
      break;
  }

}


} /*  namespace MutoTest  */
} /*  namespace Test      */
#endif  /*  #if defined(NN_SWITCH_ENABLE_HOST_IO)  */
#endif  /*  #if PM_DEBUG                  */
#endif  /*  #if defined(GF_PLATFORM_CTR)    */
