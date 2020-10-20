//======================================================================
/**
 * @file	KawaigariParamGroupManager.h
 * @brief	niji Kawaigari Group Parameter Manager
 * @author	PETE
 * @data	16/01/22
 */
//======================================================================
#if !defined( __KAWAIGARI_PARAM_GROUP_MANAGER_H__ )
#define __KAWAIGARI_PARAM_GROUP_MANAGER_H__
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
  u8 form;
  u8 sex;
  u8 group;
} KWParamData_Group;

class KawaigariParamGroupManager
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(KawaigariParamGroupManager);
public:
  
  KawaigariParamGroupManager(gfl2::heap::HeapBase *dataHeap,gfl2::heap::HeapBase *workHeap,gfl2::fs::AsyncFileManager *fileManager);
  ~KawaigariParamGroupManager(void);

  int GetGroupID(int monsNo,int formNo,int sex,int defaultVal=0);
  
  bool IsLoaded(void);
  bool Finalize(void);

#if PM_DEBUG
  void LoadHIO(char *path,gfl2::heap::HeapBase *dataHeap);
#endif

private:

  KWParamData_Group* GetData(u32 index);
  KWParamData_Group* GetData(int monsNo,int formNo,int sex);
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


#endif //__KAWAIGARI_PARAM_GROUP_MANAGER_H__

