//======================================================================
/**
 * @file	KawaigariCamera.C++
 * @brief	Camera Object Code
 * @author	PETE
 * @data	2015.10.22
 */
//======================================================================
#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>
#include <heap/include/gfl2_Heap.h>
#include "KawaigariCamera.h"

#include "KawaigariMain.h"

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

enum
{
  CAM_STATE_INIT= 0,
  CAM_STATE_MAIN,
  CAM_STATE_CLEANUP,
  CAM_STATE_FINISH,
};


KawaigariCamera::KawaigariCamera(void)
{  
  coreSys = NULL;
  SetState(CAM_STATE_INIT);
  cameraMoveSpeed = 0.05f;
  cameraType = cameraTypeBackup = KG_CAMERA_DEFAULT;

  upDirection.x = 0.0f;
  upDirection.y = 1.0f;
  upDirection.z = 0.0f;  // カメラの上方向

  ResetIdleCamera();
}

KawaigariCamera::~KawaigariCamera(void)
{  
}

void KawaigariCamera::State_Init(void)
{
  // Set Initial Pos to far:
  coreSys->GetDataManager()->GetCameraData(KG_CAMERA_FAR,eyePosCurrent,focusPosCurrent);
  coreSys->GetDataManager()->GetCameraData(KG_CAMERA_FAR,eyePosTarget,focusPosTarget);
  SetState(CAM_STATE_MAIN);

  ResetIdleCamera();
}

void KawaigariCamera::State_Main(void)
{  
  MoveTowardsTarget();

  // Idle Camera:
  if(useIdleCamera)
  {    
    eyePosCurrent.x += idleCameraOffset.x;
    eyePosCurrent.y += idleCameraOffset.y;
    if(eyePosCurrent.y<10.0f) eyePosCurrent.y=10.0f;
  }  
}

void KawaigariCamera::State_Cleanup(void)
{

}

void KawaigariCamera::State_Finish(void)
{

}

void KawaigariCamera::Update(void)
{
  switch(mStateReg)
  {
    case CAM_STATE_INIT: State_Init(); break;
    case CAM_STATE_MAIN: State_Main(); break;
    case CAM_STATE_CLEANUP: State_Cleanup(); break;
    case CAM_STATE_FINISH: State_Finish(); break;
  }
}

void KawaigariCamera::ResetIdleCamera(void)
{
  idleCamera.x = idleCamera.y = idleCamera.z = 0.0f;
  SetUseIdleCamera(false);
  idleCameraFrame = 0;
}

void KawaigariCamera::SetRandomIdleCameraPosition(void)
{
  int xMin,xMax,yMin,yMax;

  coreSys->GetDataManager()->GetIdleCamera(xMin,xMax,yMin,yMax);

  // -Xmin -> Xmax OR Xmin -> Xmax
  int xVal = xMin + coreSys->GetRandom(xMax-xMin);
  if(coreSys->GetRandom(2)==1) xVal = -xVal;

  // -Ymin -> Ymax OR Ymin -> Ymax
  int yVal = yMin + coreSys->GetRandom(yMax-yMin);
  if(coreSys->GetRandom(2)==1) yVal = -yVal;

  if(yVal<10) yVal = 10;

  idleCamera.x = (float)xVal;
  idleCamera.y = (float)yVal;
  idleCamera.z = 0.0f;

  idleCameraFrame = 0;

}

void KawaigariCamera::UpdateIdleCamera(void)
{
  f32 sinX = gfl2::math::SinDeg(idleCameraFrame>>1) * (idleCamera.x*1.00f);
  f32 sinY = gfl2::math::SinDeg(idleCameraFrame>>2)* (idleCamera.y*1.00f);

  sinX = eyePosTarget.x + sinX - eyePosCurrent.x;
  sinY = eyePosTarget.y + sinY - eyePosCurrent.y;

  idleCameraOffset.x = sinX*0.05f;
  idleCameraOffset.y = sinY*0.05f;
  
  idleCameraFrame++;
}

void KawaigariCamera::GetPosition(gfl2::math::Vector3 &eyePos,gfl2::math::Vector3 &focusPos,gfl2::math::Vector3 &upVec)
{  
  eyePos = eyePosCurrent;
  focusPos = focusPosCurrent; 
  upVec = upDirection;
}

void KawaigariCamera::MoveTowardsTarget(void)
{
  // Camera Pos:
  eyePosCurrent.x += (eyePosTarget.x - eyePosCurrent.x) * cameraMoveSpeed;
  eyePosCurrent.y += (eyePosTarget.y - eyePosCurrent.y) * cameraMoveSpeed;
  eyePosCurrent.z += (eyePosTarget.z - eyePosCurrent.z) * cameraMoveSpeed;

  // Focus Pos:
  focusPosCurrent.x += (focusPosTarget.x - focusPosCurrent.x) * cameraMoveSpeed;
  focusPosCurrent.y += (focusPosTarget.y - focusPosCurrent.y) * cameraMoveSpeed;
  focusPosCurrent.z += (focusPosTarget.z - focusPosCurrent.z) * cameraMoveSpeed;
}

void KawaigariCamera::SetCameraUpVector(float x,float y,float z)
{
  upDirection.x = x;
  upDirection.y = y;
  upDirection.z = z;
}

float KawaigariCamera::GetCameraRangeDist(gfl2::math::Vector3 tgt)
{
  gfl2::math::Vector3 pos = eyePosTarget;

  pos-=focusPosTarget;
  pos -= tgt;

  return(pos.Length());
}

void KawaigariCamera::SetNewTarget(int type,f32 moveSpeed)
{
  if(type!=cameraType) 
  {
     /// GFL_PRINT("Change Camera to: %f,%f,%f  %f,%f,%f\n",eyePosTarget.x,eyePosTarget.y,eyePosTarget.z,focusPosTarget.x,focusPosTarget.y,focusPosTarget.z);
  }

  cameraType = type;
  cameraMoveSpeed = moveSpeed * 2.0f; // 30 FPS 
  coreSys->GetDataManager()->GetCameraData(type,eyePosTarget,focusPosTarget);
  
}

void KawaigariCamera::SetLocationDirect(int type)
{
  if(type!=cameraType)
  {
     /// GFL_PRINT("Change Camera to: %f,%f,%f  %f,%f,%f\n",eyePosTarget.x,eyePosTarget.y,eyePosTarget.z,focusPosTarget.x,focusPosTarget.y,focusPosTarget.z);
  }
  cameraType = type;
//  cameraMoveSpeed = 0.0f;

  if(type==KG_CAMERA_EFFECT)
  {
    // 0	36	144	0	24	0 PIKACHUU WHOLE BODY:
    eyePosTarget.x = 0.0f;
    eyePosTarget.y = 36.0f;
    eyePosTarget.z = 144.0f;

    focusPosTarget.x = 0.0f;
    focusPosTarget.y = 24.0f;
    focusPosTarget.z = 0.0f;    

    eyePosCurrent = eyePosTarget;
    focusPosCurrent = focusPosTarget;
    return;
  }
  else if(type==KG_CAMERA_EAT_FIXED)
  {
    // 0	35	105	0	27	0 PIKACHUU EAT:
    eyePosTarget.x = 0.0f;
    eyePosTarget.y = 35.0f;
    eyePosTarget.z = 105.0f;

    focusPosTarget.x = 0.0f;
    focusPosTarget.y = 27.0f;
    focusPosTarget.z = 0.0f;    

    eyePosCurrent = eyePosTarget;
    focusPosCurrent = focusPosTarget;
    return;
  }

  coreSys->GetDataManager()->GetCameraData(type,eyePosTarget,focusPosTarget);
  coreSys->GetDataManager()->GetCameraData(type,eyePosCurrent,focusPosCurrent);
  
}


  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)