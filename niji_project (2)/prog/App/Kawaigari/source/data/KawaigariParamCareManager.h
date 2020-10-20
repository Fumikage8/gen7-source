//======================================================================
/**
 * @file	KawaigariParamCareManager.h
 * @brief	niji Kawaigari Care Parameter Manager
 * @author	PETE
 * @data	16/01/28
 */
//======================================================================
#if !defined( __KAWAIGARI_PARAM_CARE_MANAGER_H__ )
#define __KAWAIGARI_PARAM_CARE_MANAGER_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_math.h>
#include <util/include/gfl2_GLHeapAllocator.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <fs/include/gfl2_Fs.h>

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

#define MAX_CARE_JOINT 5

typedef struct  
{
  u16 idNum;  
  u8 formNum;
  u8 sex;
  u8 jointCount;  
  u8 jointID[MAX_CARE_JOINT];
  s16 xPos[MAX_CARE_JOINT];
  s16 yPos[MAX_CARE_JOINT];
  s16 zPos[MAX_CARE_JOINT];
  u16 scale[MAX_CARE_JOINT];
  u8 typeCanUse;
} KWParamData_Care;

class KawaigariParamCareManager
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(KawaigariParamCareManager);
public:
  
  KawaigariParamCareManager(gfl2::heap::HeapBase *dataHeap,gfl2::heap::HeapBase *workHeap,gfl2::fs::AsyncFileManager *fileManager);
  ~KawaigariParamCareManager(void);

  int GetJointCount(int monsNo,int formNo,int sex,int defaultVal=0);
  void GetJointPos(int monsNo,int formNo,int sex,int index,gfl2::math::Vector3 &pos);
  float GetScale(int monsNo,int formNo,int sex,int index,float defaultVal=1.0f);
  int GetJointID(int monsNo,int formNo,int sex,int index,int defaultVal=1);
  bool TypeEnabled_Dirt(int monsNo,int formNo,int sex,bool defaultValue=false);
  bool TypeEnabled_Dust(int monsNo,int formNo,int sex,bool defaultValue=false);
  bool TypeEnabled_Hair(int monsNo,int formNo,int sex,bool defaultValue=false);
  bool TypeEnabled_Water(int monsNo,int formNo,int sex,bool defaultValue=false);
  KWParamData_Care *GetCareData(int monsNo,int formNo,int sex);

  bool IsLoaded(void);
  bool Finalize(void);

#if PM_DEBUG
  void LoadHIO(char *path,gfl2::heap::HeapBase *dataHeap);
#endif

private:

  KWParamData_Care* GetData(u32 index);
  KWParamData_Care* GetData(int monsNo,int formNo,int sex);
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


#endif //__KAWAIGARI_PARAM_CARE_MANAGER_H__

