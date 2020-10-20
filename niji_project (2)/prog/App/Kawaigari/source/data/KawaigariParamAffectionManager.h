//======================================================================
/**
 * @file	KawaigariParamAffectionManager.h
 * @brief	niji Kawaigari Affection Parameter Manager
 * @author	PETE
 * @data	16/01/22
 */
//======================================================================
#if !defined( __KAWAIGARI_PARAM_AFFECTION_MANAGER_H__ )
#define __KAWAIGARI_PARAM_AFFECTION_MANAGER_H__
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
  COM_APPEAR_LOWER,COM_APPEAR_LEFT,COM_APPEAR_RIGHT,COM_APPEAR_DO_UP,COM_APPEAR_BACK,COM_APPEAR_ROW,COM_APPEAR_SLEEP,COM_ROW,COM_SLEEP,COM_TOUCH,COM_HATE,COM_FOOD_IDLE,COM_FOOD_TOUCH,COM_LOOK,COM_LOOK_AROUND,COM_LOOK_TOUCH,COM_CAMERA,
  COM_AFFECTION_MAX
};

typedef struct  
{
  u16 idNum;    
  u8 form;
  u8 sex;
  u8 loveAmount[COM_AFFECTION_MAX];
} KWParamData_Affection;

class KawaigariParamAffectionManager
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(KawaigariParamAffectionManager);
public:
  
  KawaigariParamAffectionManager(gfl2::heap::HeapBase *dataHeap,gfl2::heap::HeapBase *workHeap,gfl2::fs::AsyncFileManager *fileManager);
  ~KawaigariParamAffectionManager(void);

  int GetAffectionRequired(int monsNo,int formNo,int sex,int type,int defaultVal=0);
  
  bool IsLoaded(void);
  bool Finalize(void);

#if PM_DEBUG
  void LoadHIO(char *path,gfl2::heap::HeapBase *dataHeap);
#endif

private:

  KWParamData_Affection* GetData(u32 index);
  KWParamData_Affection* GetData(int monsNo,int formNo,int sex);
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


#endif //__KAWAIGARI_PARAM_AFFECTION_MANAGER_H__

