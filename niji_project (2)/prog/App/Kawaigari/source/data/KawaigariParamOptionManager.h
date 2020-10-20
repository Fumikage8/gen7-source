//======================================================================
/**
 * @file	KawaigariParamOptionManager.h
 * @brief	niji Kawaigari Option Parameter Manager
 * @author	PETE
 * @data	16/01/14
 */
//======================================================================
#if !defined( __KAWAIGARI_PARAM_OPTION_MANAGER_H__ )
#define __KAWAIGARI_PARAM_OPTION_MANAGER_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_math.h>
#include <util/include/gfl2_GLHeapAllocator.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <fs/include/gfl2_Fs.h>

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

typedef struct  
{
  u16 idNum;
  u8 formNo;
  u8 sex;
  s16 adjustAngleLook;  
  s16 adjustAngleEat;  
  s16 adjustAngleLower;  
  s16 adjustAngleUpper;  
  u8 foodAnimMax;
  s8 offsetEar;
  s16 idleCamXMin;  
  s16 idleCamXMax;  
  s16 idleCamYMin;  
  s16 idleCamYMax;
  u8 bodyTurn;
  u8 faceGame;
  s16 declineAngle;
  u8 slowMotion;
  s16 foodDist;
  u16 foodScale;
} KWParamData_Option;

class KawaigariParamOptionManager
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(KawaigariParamOptionManager);
public:
  
  KawaigariParamOptionManager(gfl2::heap::HeapBase *dataHeap,gfl2::heap::HeapBase *workHeap,gfl2::fs::AsyncFileManager *fileManager);
  ~KawaigariParamOptionManager(void);  

  float GetAdjustAngleEat(int monsNo,int formNo,int sex,float defaultVal=0.0f); // (DEGREES)
  float GetAdjustAngleLook(int monsNo,int formNo,int sex,float defaultVal=0.0f); // (DEGREES)
  float GetAdjustAngleUpper(int monsNo,int formNo,int sex,float defaultVal=10.0f); // (DEGREES)
  float GetAdjustAngleLower(int monsNo,int formNo,int sex,float defaultVal=-25.0f); // (DEGREES)
  int GetFoodAnimMax(int monsNo,int formNo,int sex,int defaultVal=5);
  float GetOffsetEar(int monsNo,int formNo,int sex,float defaultVal=-8.0f); // (DEGREES)
  bool UseSlowMotion(int monsNo,int formNo,int sex,bool defaultVal=false);

  float GetFoodDist(int monsNo,int formNo,int sex,float defaultVal=130.0f);
  float GetFoodScale(int monsNo,int formNo,int sex,float defaultVal=1.0f);

  // Idle Camera:
  int GetIdleCameraXMin(int monsNo,int formNo,int sex,int defaultVal=40);
  int GetIdleCameraXMax(int monsNo,int formNo,int sex,int defaultVal=80);
  int GetIdleCameraYMin(int monsNo,int formNo,int sex,int defaultVal=40);
  int GetIdleCameraYMax(int monsNo,int formNo,int sex,int defaultVal=60);
 
  bool IsLoaded(void);
  bool Finalize(void);

#if PM_DEBUG
  void LoadHIO(char *path,gfl2::heap::HeapBase *dataHeap);
#endif

private:

  KWParamData_Option* GetData(u32 index);
  KWParamData_Option* GetData(int monsNo,int formNo,int sex);
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


#endif //__KAWAIGARI_PARAM_OPTION_MANAGER_H__

