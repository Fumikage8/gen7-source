//======================================================================
/**
 * @file	KawaigariParamCameraManager.h
 * @brief	niji Kawaigari Camera Parameter Manager
 * @author	PETE
 * @data	16/01/12
 */
//======================================================================
#if !defined( __KAWAIGARI_PARAM_CAMERA_MANAGER_H__ )
#define __KAWAIGARI_PARAM_CAMERA_MANAGER_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_math.h>
#include <util/include/gfl2_GLHeapAllocator.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <fs/include/gfl2_Fs.h>

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

enum
{
  CAM_X_CAM_FAR,CAM_Y_CAM_FAR,CAM_Z_CAM_FAR,TGT_X_CAM_FAR,TGT_Y_CAM_FAR,TGT_Z_CAM_FAR,CAM_X_CAM_WHOLE_BODY,CAM_Y_CAM_WHOLE_BODY,CAM_Z_CAM_WHOLE_BODY,TGT_X_CAM_WHOLE_BODY,TGT_Y_CAM_WHOLE_BODY,TGT_Z_CAM_WHOLE_BODY,CAM_X_CAM_UP,CAM_Y_CAM_UP,CAM_Z_CAM_UP,TGT_X_CAM_UP,TGT_Y_CAM_UP,TGT_Z_CAM_UP,CAM_X_CAM_DO_UP,CAM_Y_CAM_DO_UP,CAM_Z_CAM_DO_UP,TGT_X_CAM_DO_UP,TGT_Y_CAM_DO_UP,TGT_Z_CAM_DO_UP,CAM_X_CAM_LEFT,CAM_Y_CAM_LEFT,CAM_Z_CAM_LEFT,TGT_X_CAM_LEFT,TGT_Y_CAM_LEFT,TGT_Z_CAM_LEFT,CAM_X_CAM_RIGHT,CAM_Y_CAM_RIGHT,CAM_Z_CAM_RIGHT,TGT_X_CAM_RIGHT,TGT_Y_CAM_RIGHT,TGT_Z_CAM_RIGHT,CAM_X_CAM_PEEP,CAM_Y_CAM_PEEP,CAM_Z_CAM_PEEP,TGT_X_CAM_PEEP,TGT_Y_CAM_PEEP,TGT_Z_CAM_PEEP,CAM_X_CAM_BEHIND,CAM_Y_CAM_BEHIND,CAM_Z_CAM_BEHIND,TGT_X_CAM_BEHIND,TGT_Y_CAM_BEHIND,TGT_Z_CAM_BEHIND,CAM_X_CAM_HIT1,CAM_Y_CAM_HIT1,CAM_Z_CAM_HIT1,TGT_X_CAM_HIT1,TGT_Y_CAM_HIT1,TGT_Z_CAM_HIT1,CAM_X_CAM_EAT,CAM_Y_CAM_EAT,CAM_Z_CAM_EAT,TGT_X_CAM_EAT,TGT_Y_CAM_EAT,TGT_Z_CAM_EAT,
  CAMERA_MAX,
};

typedef struct  
{
  u16 idNum;    
  u8 formNo;
  u8 sex;
  s16 yPos;
  s16 camPos[CAMERA_MAX];
} KWParamData_Camera;

class KawaigariParamCameraManager
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(KawaigariParamCameraManager);
public:
  
  KawaigariParamCameraManager(gfl2::heap::HeapBase *dataHeap,gfl2::heap::HeapBase *workHeap,gfl2::fs::AsyncFileManager *fileManager);
  ~KawaigariParamCameraManager(void);

 
  void GetCameraData(int monsNo,int formNo,int sex,int type,gfl2::math::Vector3 &eyePos,gfl2::math::Vector3 &focusPos);
  float GetAppearYPos(int monsNo,int formNo,int sex,float defaultVal=-23.0f);
  KWParamData_Camera *GetCameraData(int monsNo,int formNo,int sex);

  bool IsLoaded(void);
  bool Finalize(void);

#if PM_DEBUG
  void LoadHIO(char *path,gfl2::heap::HeapBase *dataHeap);
#endif

private:

  KWParamData_Camera* GetData(u32 index);
  int GetIndex(int monsNo,int formNo,int sex);

  gfl2::fs::AsyncFileManager *mFileManager;
  void *dataBuf;
  u32 dataCount;
  bool isLoaded;

  int checkMonsNo,checkFormNo,checkSexNo;
  int lastResult;

};
  
  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)


#endif //__KAWAIGARI_PARAM_CAMERA_MANAGER_H__

