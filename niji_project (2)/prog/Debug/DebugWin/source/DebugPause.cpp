//======================================================================
/**
 * @file  debug_pause.cpp
 * @brief 11/06/03
 * @author  ariizumi
 * @data  ポーズ・コマ送り機能
 */
//======================================================================

#include "Debug/DebugWin/include/DebugPause.h"

#include "Debug/DebugWin/include/DebugWinSystem.h"
#include "GameSys/include/GameManager.h"


//-----------------------------------------------------------------------------
/**
*    Singletonインスタンスの実体を定義
*/
//-----------------------------------------------------------------------------
SINGLETON_INSTANCE_DEFINITION(Debug::DebugWin::PauseSystem)
template class gfl2::base::SingletonAccessor<Debug::DebugWin::PauseSystem>;


namespace Debug
{
namespace DebugWin
{

PauseSystem::PauseSystem()
:mIsEnable(false)
,mCanUpdate(true)
,mIsCheckDebugWin(false)
,mHoldKey(gfl2::ui::BUTTON_Y)
,mStepKey(gfl2::ui::BUTTON_A)
{
}

void PauseSystem::Update(void)
{
#if GFL_DEBUGWIN_USE
  gfl2::ui::DeviceManager *uiMng = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  gfl2::ui::Button *btn = uiMng->GetButton(0);

  if( (btn->IsHold( mHoldKey ) && !btn->IsTrigger( mStepKey )) ||
      (mIsCheckDebugWin && gfl2::debug::DebugWin_IsOpen() ) )
  {
    mCanUpdate = false;
  }
  else
  {
    mCanUpdate = true;
  }
#endif
}

bool PauseSystem::IsPause(void)
{
  if( mIsEnable && !mCanUpdate )
  {
    return true;
  }
  return false;
}


//ラッパー
#if PM_DEBUG
void PauseSystem_Initialize(gfl2::heap::HeapBase* pDebugHeap)
{
  Debug::DebugWin::PauseSystem* pSys = GFL_NEW(pDebugHeap) Debug::DebugWin::PauseSystem();
}

void PauseSystem_Terminate(void)
{
  Debug::DebugWin::PauseSystem *pauseSys = GFL_SINGLETON_INSTANCE(Debug::DebugWin::PauseSystem);
  if( pauseSys )
  {
    GFL_DELETE(pauseSys);
  }
  else
  {
    GFL_ASSERT(pauseSys);
  }
}

void PauseSystem_Update(void)
{
  Debug::DebugWin::PauseSystem *pauseSys = GFL_SINGLETON_INSTANCE(Debug::DebugWin::PauseSystem);
  pauseSys->Update();
}


bool PauseSystem_IsPause(void)
{
#if GFL_DEBUGWIN_USE
  Debug::DebugWin::PauseSystem *pauseSys = GFL_SINGLETON_INSTANCE(Debug::DebugWin::PauseSystem);
  return pauseSys->IsPause();
#else
  return false;
#endif
}
#else //PM_DEBUG
void PauseSystem_Initialize(gfl2::heap::HeapBase* /*pDebugHeap*/){}
void PauseSystem_Terminate(void){}
void PauseSystem_Update(void){}
bool PauseSystem_IsPause(void){return false;}
#endif //PM_DEBUG

}//namespace DebugWin
}//namespace Debug
