//======================================================================
/**
 * @file Proc.cpp
 * @date 2016/09/05 19:58:22
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <debug/include/gfl2_DebugPrint.h>
#include <System/include/GflUse.h>
#include <System/include/nijiAllocator.h>
#include "Applib/include/Util/AppRenderingManager.h"
#include "App/LunaSolDive/include/Proc.h"
#include "./Assert.h"
#include "./Manager.h"
#include "./RenderingPipeline.h"

namespace LunaSolDive{
// parameter
static const u32 LOCAL_HEAP_SIZE = 0x02600000;// 38Mi byte
static const u32 LOCAL_APP_HEAP_SIZE = 0x00500000;// 5Mi byte
static const u32 LOCAL_APP_DEVICE_HEAP_SIZE = 0x02000000;// 32Mi byte
// constructor
Proc::Proc()
    : GameProc(),
      m_pHeap(NULL),
      m_pAppHeap(NULL),
      m_pRenderingManager(NULL),
      m_pLunaSolDiveManager(NULL),
      m_ProcessCounter(0),
      m_pIOParameter(NULL)
{
#if PM_DEBUG
  m_IsDebugMode = false;
#endif// PM_DEBUG
}
// destructor
Proc::~Proc(){}
// Initialize (override)
gfl2::proc::Result Proc::InitFunc(
            gfl2::proc::Manager* pManager)
{
  GFL_PRINT("Call InitFunc\n");
  switch(m_ProcessCounter)
  {
  case 0:
    {
      // Initialize Heap
      gfl2::heap::HeapBase* const pAppDeviceHeap =
              gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
      GFL_PRINT("App Device Heap\n");
      GFL_PRINT("  ID: %d\n", pAppDeviceHeap->GetHeapId());
      GFL_PRINT("  Size: 0x%lx\n", pAppDeviceHeap->GetTotalSize());
      GFL_PRINT("  Free Size: 0x%lx\n", pAppDeviceHeap->GetTotalFreeSize());
      GFL_PRINT("  Allocatable Size: %0xlx\n", pAppDeviceHeap->GetTotalAllocatableSize());
      GFL_PRINT("  Fragment : %s\n", pAppDeviceHeap->IsFragment()? "True": "False");
      m_pHeap = GFL_CREATE_LOCAL_HEAP(
                  pAppDeviceHeap,
                  LOCAL_HEAP_SIZE,
                  gfl2::heap::HEAP_TYPE_EXP,
                  false);
      GFL_PRINT("Locap Heap\n");
      GFL_PRINT("  ID: %d\n", m_pHeap->GetHeapId());
      GFL_PRINT("  Size: 0x%lx\n", m_pHeap->GetTotalSize());
      GFL_PRINT("  Free Size: 0x%lx\n", m_pHeap->GetTotalFreeSize());
      GFL_PRINT("  Allocatable Size: %0xlx\n", m_pHeap->GetTotalAllocatableSize());
      GFL_PRINT("  Fragment : %s\n", m_pHeap->IsFragment()? "True": "False");
      // Initialize Application Heap
      m_pAppHeap = GFL_NEW(m_pHeap) app::util::Heap();
      m_pAppHeap->LocalHeapCreate(
                  m_pHeap,
                  m_pHeap,
                  LOCAL_APP_HEAP_SIZE,
                  LOCAL_APP_DEVICE_HEAP_SIZE);
      GFL_PRINT("Locap App System Heap\n");
      GFL_PRINT("  ID: %d\n", m_pAppHeap->GetSystemHeap()->GetHeapId());
      GFL_PRINT("  Size: 0x%lx\n", m_pAppHeap->GetSystemHeap()->GetTotalSize());
      GFL_PRINT("  Free Size: 0x%lx\n", m_pAppHeap->GetSystemHeap()->GetTotalFreeSize());
      GFL_PRINT("  Allocatable Size: %0xlx\n", m_pAppHeap->GetSystemHeap()->GetTotalAllocatableSize());
      GFL_PRINT("  Fragment : %s\n", m_pAppHeap->GetSystemHeap()->IsFragment()? "True": "False");
      GFL_PRINT("Locap App Device Heap\n");
      GFL_PRINT("  ID: %d\n", m_pAppHeap->GetDeviceHeap()->GetHeapId());
      GFL_PRINT("  Size: 0x%lx\n", m_pAppHeap->GetDeviceHeap()->GetTotalSize());
      GFL_PRINT("  Free Size: 0x%lx\n", m_pAppHeap->GetDeviceHeap()->GetTotalFreeSize());
      GFL_PRINT("  Allocatable Size: %0xlx\n", m_pAppHeap->GetDeviceHeap()->GetTotalAllocatableSize());
      GFL_PRINT("  Fragment : %s\n", m_pAppHeap->GetDeviceHeap()->IsFragment()? "True": "False");
      break;
    }
  case 1:
    {
      // Initialize RenderingManager
      m_pRenderingManager = GFL_NEW(m_pAppHeap->GetDeviceHeap()) app::util::AppRenderingManager();
      app::util::AppRenderingManager::Description description;
      description.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder =
              app::util::AppRenderingManager::RenderOrderType::LAYOUT_EFFECT;
      m_pRenderingManager->StartAsyncCreate(
                  description,
                  m_pAppHeap,
                  NULL,
                  GetGameManager()->GetAsyncFileManager());
      break;
    }
  case 2:
    {
      if(! m_pRenderingManager->IsCreated())
      {
        return gfl2::proc::RES_CONTINUE;
      }
      // Initialize LunaSolDiveManager
      m_pLunaSolDiveManager = GFL_NEW(m_pAppHeap->GetDeviceHeap()) LunaSolDive::Manager();
      if(m_pIOParameter != NULL)
      {
        m_pLunaSolDiveManager->SetIOParameter(
                    m_pIOParameter);
      }
#if PM_DEBUG
      if(m_IsDebugMode)
      {
        m_pLunaSolDiveManager->SwitchToDebugMode();
      }
#endif// PM_DEBUG
      m_pLunaSolDiveManager->Initialize(
                  m_pAppHeap->GetDeviceHeap(),
                  GetGameManager(),
                  m_pRenderingManager);
      return gfl2::proc::RES_FINISH;
    }
  default:
    GFL_PRINT("LunaSolDive::Proc::InitFunc unexpected stage\n");
    break;
  }
  ++m_ProcessCounter;
  return gfl2::proc::RES_CONTINUE;
}
// Terminate (override)
gfl2::proc::Result Proc::EndFunc(
            gfl2::proc::Manager* pManager)
{
  GFL_PRINT("Call EndFunc\n");
  // delete LunaSolDiveManager
  if(m_pLunaSolDiveManager != NULL)
  {
    LUNA_SOL_DIVE_ASSERT(m_pLunaSolDiveManager->IsTerminated());
    GFL_DELETE(m_pLunaSolDiveManager);
    m_pLunaSolDiveManager = NULL;
  }
  // delete RenderingManager
  GFL_SAFE_DELETE(m_pRenderingManager);
  // delete Heap
  if(m_pAppHeap != NULL)
  {
    m_pAppHeap->Delete();
    GFL_SAFE_DELETE(m_pAppHeap);
  }
  GFL_DELETE_HEAP(m_pHeap);
  m_pHeap = NULL;
  // reset Parameter
  m_ProcessCounter = 0;
  m_pIOParameter = NULL;
  return gfl2::proc::RES_FINISH;
}
// Update (override)
gfl2::proc::Result Proc::UpdateFunc(
            gfl2::proc::Manager* /*pManager*/)
{
  // Update LunaSolDiveManager
  LUNA_SOL_DIVE_ASSERT(m_pLunaSolDiveManager != NULL);
  m_pLunaSolDiveManager->Update();
  return m_pLunaSolDiveManager->IsTerminated()
          ? gfl2::proc::RES_FINISH
          : gfl2::proc::RES_CONTINUE;
}
// Draw (override)
void Proc::DrawFunc(
            gfl2::proc::Manager* pManager,
            gfl2::gfx::CtrDisplayNo displayNo)
{
  LUNA_SOL_DIVE_ASSERT(m_pLunaSolDiveManager != NULL);
  m_pLunaSolDiveManager->Draw(displayNo);
}
// Set pointer to input Parameter & output the Result
void Proc::SetIOParameter(
            App::Event::LUNASOL_DIVE_PARAM* pIOParameter)
{
  m_pIOParameter = pIOParameter;
  LUNA_SOL_DIVE_ASSERT(m_pIOParameter);
}
#if PM_DEBUG
// Switch to Debug Mode
void Proc::SwitchToDebugMode()
{
  m_IsDebugMode = true;
}
#endif// PM_DEBUG
}// namespace LunaSolDive
