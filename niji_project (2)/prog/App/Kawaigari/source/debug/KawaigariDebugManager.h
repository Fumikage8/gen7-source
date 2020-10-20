//======================================================================
/**
 * @file	KawaigariDebugManager.h
 * @brief	Kawaigari All Debug Mode Handling (Camera, CareMode, etc)
 * @author	PETE
 * @data	2015.10.27
 */
//======================================================================

#if defined(GF_PLATFORM_WIN32)

#if PM_DEBUG 

#if !defined __KAWAIGARI_DEBUGMANAGER_H_INCLUDED__
#define __KAWAIGARI_DEBUGMANAGER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>
#include <Math/include/gfl2_Matrix.h>
#include <Math/include/gfl2_Quaternion.h>

#include <util/include/gfl2_GLHeapAllocator.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <fs/include/gfl2_Fs.h>

// Text
#include <str/include/gfl2_Font.h>
#include <util/include/gfl2_UtilTextRenderingPipeLine.h>

#include "../data/KawaigariParamCareManager.h"
#include "../data/KawaigariParamCameraManager.h"

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

const int WRITEBUFFER_SIZE = 1024*10; // 10K

enum
{
  DEBUGMANAGER_STATE_INIT= 0,
  DEBUGMANAGER_STATE_MAIN,
  DEBUGMANAGER_STATE_IDLE,
  DEBUGMANAGER_STATE_RELOADPOKEMON,
  DEBUGMANAGER_STATE_CAREMODE,
  DEBUGMANAGER_STATE_CAMERASET,
  DEBUGMANAGER_STATE_EATVIBRATIONSET,
  DEBUGMANAGER_STATE_END,
  DEBUGMANAGER_STATE_CLEANUP,
  DEBUGMANAGER_STATE_FINISH,
};

class KawaigariMain;

class KawaigariDebugManager
{

public:
  KawaigariDebugManager(void);
  ~KawaigariDebugManager(void);
  
  bool Finalize(void);
  void SetCoreSys(KawaigariMain *core) { coreSys = core; }
  void SetPokemonIDForm(int monsNo,int formNo,int sexNo) { mMonsNo = monsNo; mFormNo = formNo; mSexNo = sexNo; }
  void ToggleEnable(void);

  void Update(void);
  
  // State Functions:
  int GetState(void) { return(mStateReg); }
  int GetSubState(void) { return(mStateRegSub); }
  void SetState(int s);
  void SetSubState(int s);
  void SaveState(void);
  void RestoreState(void);
  void PrintState(void);

  // Main States:
  void State_Init(void);
  void State_Idle(void);
  void State_ReloadPokemon(void);
  void State_CareMode(void);
  void State_CameraSet(void);
  void State_EatVibrationSet(void);
  void State_Main(void) {}  
  void State_End(void) {}
  void State_Cleanup(void) {}
  void State_Finish(void) {}

private:
  void UpdateAgain(void) { mRequireUpdate = true; }
  bool mRequireUpdate;

  KawaigariMain *coreSys;  
  int mMonsNo,mFormNo,mSexNo;
  int mReloadMonsNo,mReloadFormNo,mReloadSexNo;
  bool mEnable;

  // State:  
  int mStateReg,mStateRegSub,mStateRegs[MAX_STATE_REGS];
  int mStateRegBackup,mStateRegSubBackup,mStateRegsBackup[MAX_STATE_REGS];
  int mSelection;

  // CareMode:
  KWParamData_Care careData;
  KawaigariObject_Effect *mEffect[4];
  int mEffectType;
  void ExportCareDataToPC(void);

  // CameraMode:
  KWParamData_Camera cameraData;
  void ExportCameraDataToPC(void);

#if defined(GF_PLATFORM_WIN32)
  // Eat Vibrate Mode
  void ExportEatVibrationDataToPC(void);
#endif

  // Write Buffer:
  char *mWriteBuffer;
  int mWritePos;
  void ResetWriteBuffer(void);
  void AddWriteBufferString(char *str);

};

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)

#endif // __KAWAIGARI_DataManager_H_INCLUDED__

#endif // PM_DEBUG

#endif // WIN_32