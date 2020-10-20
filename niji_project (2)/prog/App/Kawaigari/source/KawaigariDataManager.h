//======================================================================
/**
 * @file	KawaigariDataManager.h
 * @brief	Kawaigari All Data Handling (Camera, Parameters, etc)
 * @author	PETE
 * @data	2015.10.22
 */
//======================================================================

#if !defined __KAWAIGARI_DATAMANAGER_H_INCLUDED__
#define __KAWAIGARI_DATAMANAGER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>
#include <Math/include/gfl2_Matrix.h>
#include <Math/include/gfl2_Quaternion.h>

#include <util/include/gfl2_GLHeapAllocator.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <fs/include/gfl2_Fs.h>

#include "data/KawaigariParamCameraManager.h"
#include "data/KawaigariParamOptionManager.h"
#include "data/KawaigariParamFaceManager.h"
#include "data/KawaigariParamSnackAffectionManager.h"
#include "data/KawaigariParamGroupManager.h"
#include "data/KawaigariParamAffectionManager.h"
#include "data/KawaigariParamMotionAffectionManager.h"
#include "data/KawaigariParamSoundManager.h"
#include "data/KawaigariParamCareManager.h"

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

class KawaigariMain;

class KawaigariDataManager
{

public:
  KawaigariDataManager(void);
  ~KawaigariDataManager(void);

#if PM_DEBUG
  void HIOReload(void);
  void HIOReload_Camera(void);
  void HIOReload_Care(void);
#endif

  bool Initalize(gfl2::heap::HeapBase *dataHeap,gfl2::heap::HeapBase *workHeap,gfl2::fs::AsyncFileManager *fileManager);
  bool Finalize(void);

  void SetCoreSys(KawaigariMain *core) { coreSys = core; }

  // Camera Data:
  void GetCameraData(int type,gfl2::math::Vector3 &eyePos,gfl2::math::Vector3 &focusPos);
  void GetYAppearPosition(float &yPos) { yPos = cameraData->GetAppearYPos(mMonsNo,mFormNo,mSexNo); }
  void GetIdleCamera(int &xMin,int &xMax,int &yMin,int &yMax);
  KWParamData_Camera *GetCameraData(void) { return(cameraData->GetCameraData(mMonsNo,mFormNo,mSexNo)); }

  void SetPokemonIDForm(int monsNo,int formNo,int sexNo) { mMonsNo = monsNo; mFormNo = formNo; mSexNo=sexNo; }

  // Option Data:
  inline float GetAdjustAngleEat(void) { return(optionData->GetAdjustAngleEat(mMonsNo,mFormNo,mSexNo)); }
  inline float GetAdjustAngleLook(void) { return(optionData->GetAdjustAngleLook(mMonsNo,mFormNo,mSexNo)); }
  inline int GetFoodAnimMax(void) { return(optionData->GetFoodAnimMax(mMonsNo,mFormNo,mSexNo)); }
  inline float GetOffsetEar(void) { return(optionData->GetOffsetEar(mMonsNo,mFormNo,mSexNo)); }
  inline float GetAdjustAngleUpper(void) { return(optionData->GetAdjustAngleUpper(mMonsNo,mFormNo,mSexNo)); }
  inline float GetAdjustAngleLower(void) { return(optionData->GetAdjustAngleLower(mMonsNo,mFormNo,mSexNo)); }
  inline bool UseSlowMotion(void) { return(optionData->UseSlowMotion(mMonsNo,mFormNo,mSexNo)); }
  inline float GetFoodDist(void) { return(130.0f); /*return(optionData->GetFoodDist(mMonsNo,mFormNo,mSexNo));*/ }
  inline float GetFoodScale(void) { return(1.0f); /*return(optionData->GetFoodScale(mMonsNo,mFormNo,mSexNo));*/ }
  

  // Face Data:
  inline int GetFaceAnimationIndex(int type) { return(faceData->GetFaceAnimationIndex(mMonsNo,mFormNo,mSexNo,type));  }
  
  // Snack Data:
  inline int GetSnackAffection(int type) { return(snackAffectionData->GetSnackAffection(mMonsNo,mFormNo,mSexNo,type)); }

  // Group ID Data:
  inline int GetGroupID(void) { return(groupData->GetGroupID(mMonsNo,mFormNo,mSexNo)); }

  // Affection Required Data:
  inline int GetAffectionRequired(int type) { return(affectionData->GetAffectionRequired(mMonsNo,mFormNo,mSexNo,type)); }
  inline int GetMotionAffectionRequired(int type) { return(motionAffectionData->GetAffectionRequired(mMonsNo,mFormNo,mSexNo,type)); }

  // Sound:
  inline int GetSoundID(int type) { return(soundData->GetSoundID(mMonsNo,mFormNo,mSexNo,type)); }

  // Care Data:
  int GetJointCount(void) { return(careData->GetJointCount(mMonsNo,mFormNo,mSexNo)); }
  void GetJointPos(int index,gfl2::math::Vector3 &pos) { careData->GetJointPos(mMonsNo,mFormNo,mSexNo,index,pos); }
  float GetScale(int index) { return(careData->GetScale(mMonsNo,mFormNo,mSexNo,index)); }
  int GetJointID(int index) { return(careData->GetJointID(mMonsNo,mFormNo,mSexNo,index)); }
  bool TypeEnabled_Dirt(void) { return(careData->TypeEnabled_Dirt(mMonsNo,mFormNo,mSexNo)); }
  bool TypeEnabled_Dust(void) { return(careData->TypeEnabled_Dust(mMonsNo,mFormNo,mSexNo)); }
  bool TypeEnabled_Hair(void) { return(careData->TypeEnabled_Hair(mMonsNo,mFormNo,mSexNo)); }
  bool TypeEnabled_Water(void) { return(careData->TypeEnabled_Water(mMonsNo,mFormNo,mSexNo)); }
  KWParamData_Care *GetCareData(void) { return(careData->GetCareData(mMonsNo,mFormNo,mSexNo)); }

private:

  KawaigariMain *coreSys;
  KawaigariParamCameraManager *cameraData;
  KawaigariParamOptionManager *optionData;
  KawaigariParamFaceManager *faceData;
  KawaigariParamSnackAffectionManager *snackAffectionData;
  KawaigariParamGroupManager *groupData;
  KawaigariParamAffectionManager *affectionData;
  KawaigariParamMotionAffectionManager *motionAffectionData;
  KawaigariParamSoundManager *soundData;
  KawaigariParamCareManager *careData;


  int initSeq;
  int mMonsNo,mFormNo,mSexNo;
  
};

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)

#endif // __KAWAIGARI_DataManager_H_INCLUDED__
