#if !defined(NETAPPSAVEUTILITY_H_INCLUDED)
#define NETAPPSAVEUTILITY_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   NetAppSaveUtility.h
 * @date   2015/12/15 Tue. 16:06:33
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>
#include  <heap/include/gfl2_Heap.h>
#include  <fs/include/gfl2_Fs.h>

#include  "GameSys/include/GameManager.h"
#include  "GameSys/include/GameData.h"

#include  "../System/ApplicationWorkBase.h"

namespace NetAppLib {
namespace Util      {
// =============================================================================
/**/
//==============================================================================
class NetAppSaveUtility
{
  GFL_FORBID_COPY_AND_ASSIGN(NetAppSaveUtility);
public:
  NetAppSaveUtility(void)
    : m_FrameCount(0)
    , m_IsSaveEnd(false)
  {}

  virtual ~NetAppSaveUtility(){}


  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  void  SaveStart(NetAppLib::System::ApplicationWorkBase* pWorkBase)
  {
    SaveStart(pWorkBase->GetDeviceHeap());
  }


  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  void  SaveStart(gfl2::heap::HeapBase* pHeapBase)
  {
    m_IsSaveEnd = false;
    ResetWait();
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->SaveStart(pHeapBase);
  }


  //------------------------------------------------------------------
  /**
    *  @brief   
    *  @note    失敗は下位モジュールでFatalなどに落ちる想定
    */
  //------------------------------------------------------------------
  bool  SaveMain(void)
  {
    const bool  isWaited  = LeastWait();
    
    if(!m_IsSaveEnd)
    {
      m_IsSaveEnd = (GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->SaveMain() == gfl2::fs::FsCtrBackupSystem::RESULT_OK);
    }
    
    return (isWaited && m_IsSaveEnd);
  }


protected:
  void  ResetWait(void)   {m_FrameCount = LeastWaitFrame;}
  bool  LeastWait(void)   {if(m_FrameCount){--m_FrameCount;} return (!m_FrameCount);}
  
  static const u32  LeastWaitFrame  = 90;

  /*  members  */
  u32   m_FrameCount;
  bool  m_IsSaveEnd;
};


} /*  namespace Util      */
} /*  namespace NetAppLib */
#endif  /*  #if !defined(NETAPPSAVEUTILITY_H_INCLUDED)  */
