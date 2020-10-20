//======================================================================
/**
 * @file  debug_pause.h
 * @brief 11/06/03
 * @author  ariizumi
 * @data  ポーズ・コマ送り機能
 */
//======================================================================
#ifndef __DEBUG_PAUSE_H__
#define __DEBUG_PAUSE_H__

#include <macro/include/gfl2_Macros.h>
#include <base/include/gfl2_Singleton.h>
#include <heap/include/gfl2_Heap.h>


namespace Debug
{
namespace DebugWin
{

class PauseSystem :public gfl2::base::Singleton<PauseSystem>
{
  GFL_FORBID_COPY_AND_ASSIGN(PauseSystem);
public:
  PauseSystem();

  void Update(void);
  bool IsPause(void);

  //取得系(DebugWinで操作できるようにポインタで返す)
  inline bool* GetIsEnablePtr(void){return &mIsEnable;}
  inline bool* GetIsCheckDebugWin(void){return &mIsCheckDebugWin;}
  inline int* GetHoldKey(void){return &mHoldKey;}
  inline int* GetStepKey(void){return &mStepKey;}


private:
  bool mIsEnable;
  bool mCanUpdate;
  bool mIsCheckDebugWin;

  int mHoldKey;
  int mStepKey;
};

extern void PauseSystem_Initialize(gfl2::heap::HeapBase* pDebugHeap);
extern void PauseSystem_Terminate(void);
extern void PauseSystem_Update(void);
extern bool PauseSystem_IsPause(void);



}//namespace DebugWin
}//namespace Debug


/** ---------------------------------------------------------------------
//! @brief  SINGLETONの前方参照宣言
// ---------------------------------------------------------------------*/
SINGLETON_INSTANCE_DECLARATION( Debug::DebugWin::PauseSystem );



#endif //__DEBUG_PAUSE_H__
