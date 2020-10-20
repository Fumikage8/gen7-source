//======================================================================
/**
 * @file	KawaigariCamera.h
 * @brief	Kawaigari Camera Control
 * @author	PETE
 * @data	2015.10.22
 */
//======================================================================

#if !defined __KAWAIGARI_CAMERA_H_INCLUDED__
#define __KAWAIGARI_CAMERA_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>
#include <Math/include/gfl2_Matrix.h>
#include <Math/include/gfl2_Quaternion.h>

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

enum
{
  KG_CAMERA_DEFAULT = 0,
  KG_CAMERA_FAR = 0,
  KG_CAMERA_WHOLE_BODY,
  KG_CAMERA_UP,
  KG_CAMERA_DO_UP,
  KG_CAMERA_LEFT,
  KG_CAMERA_RIGHT,
  KG_CAMERA_PEEP,
  KG_CAMERA_BEHIND,
  KG_CAMERA_HIT1,
  KG_CAMERA_EAT,
  KG_CAMERA_BUST_UP,  
  KG_CAMERA_EFFECT,  
  KG_CAMERA_ALL_MOTION,  
  KG_CAMERA_EAT_FIXED,  
  KG_CAMERA_MAX
};


class KawaigariMain;

class KawaigariCamera
{

public:
  KawaigariCamera(void);
  ~KawaigariCamera(void);

  int GetCameraType(void) { return(cameraType); }
  bool CheckCameraType(int n) { return(cameraType==n); }
  void SetCoreSys(KawaigariMain *core) { coreSys = core; }

  void GetPosition(gfl2::math::Vector3 &eyePos,gfl2::math::Vector3 &focusPos,gfl2::math::Vector3 &upVec);
  void SetCameraUpVector(float x,float y,float z);

  void Update(void);

  // State Functions:
  void SetState(u32 s) { mStateReg=s; mStateRegSub=0; }
  void State_Init(void);
  void State_Main(void);
  void State_Cleanup(void);
  void State_Finish(void);

  // Set a new Target for the camera to move towards:
  void SetNewTarget(int type,f32 moveSpeed=0.05f);
  void SetLocationDirect(int type);

  // Idle Camera:
  void ResetIdleCamera(void);
  void SetRandomIdleCameraPosition(void);
  void SetUseIdleCamera(bool b) { useIdleCamera = b; }
  void UpdateIdleCamera(void);
  
  // Ported Functions:
  bool GetDualScreenMonsterCameraMode(void) { return(false); } // TEMP!

  void SaveCurrentCamera(void) { eyePosTargetBackup=eyePosTarget; focusPosTargetBackup=focusPosTarget; eyePosCurrentBackup=eyePosCurrent; focusPosCurrentBackup=focusPosCurrent; cameraTypeBackup = cameraType; }
  void RestoreCurrentCamera(void) { eyePosTarget=eyePosTargetBackup; focusPosTarget=focusPosTargetBackup; eyePosCurrent=eyePosCurrentBackup; focusPosCurrent=focusPosCurrentBackup; cameraType = cameraTypeBackup; }

  void SaveCameraMoveSpeed(void) { cameraMoveSpeedBackup = cameraMoveSpeed; }
  void RestoreCameraMoveSpeed(void) { cameraMoveSpeed = cameraMoveSpeedBackup; }
  void SetCameraMoveSpeed(float f) { cameraMoveSpeed = f; }

  void GetCamDirect(gfl2::math::Vector3 &camPos,gfl2::math::Vector3 &camTgt) { camPos=eyePosCurrent; camTgt=focusPosCurrent; }
  void SetCamDirect(gfl2::math::Vector3 camPos,gfl2::math::Vector3 camTgt) { eyePosCurrent=camPos; focusPosCurrent=camTgt; }

  float GetCameraRangeDist(gfl2::math::Vector3 tgt);

private:

  void MoveTowardsTarget(void);

  KawaigariMain *coreSys;

  // State:  
  u32 mStateReg,mStateRegSub;
  
  // Current Position:
  gfl2::math::Vector3 eyePosCurrent,focusPosCurrent;
  gfl2::math::Vector3 eyePosCurrentBackup,focusPosCurrentBackup;

  // Target Position:
  gfl2::math::Vector3 eyePosTarget,focusPosTarget;
  gfl2::math::Vector3 eyePosTargetBackup,focusPosTargetBackup;
  gfl2::math::Vector3 upDirection;

  // Idle Camera:
  gfl2::math::Vector3 idleCamera;
  gfl2::math::Vector3 idleCameraOffset;
  bool useIdleCamera;
  u32 idleCameraFrame;
  int cameraType,cameraTypeBackup;

  f32 cameraMoveSpeed,cameraMoveSpeedBackup;
  
};

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)

#endif // __KAWAIGARI_CAMERA_H_INCLUDED__
