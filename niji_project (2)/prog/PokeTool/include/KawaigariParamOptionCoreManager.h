//======================================================================
/**
 * @file	KawaigariParamOptionCoreManager.h
 * @brief	niji Kawaigari Option Core Parameter Manager
 * @author	PETE
 * @data	16/01/12
 */
//======================================================================
#if !defined( __KAWAIGARI_PARAM_OPTIONCORE_MANAGER_H__ )
#define __KAWAIGARI_PARAM_OPTIONCORE_MANAGER_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_math.h>
#include <util/include/gfl2_GLHeapAllocator.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <fs/include/gfl2_Fs.h>

GFL_NAMESPACE_BEGIN(PokeTool)  

typedef struct  
{
  u16 idNum;  
  u8 formNum;
  u8 sex;
  s16 adjustLookAngle;
} KWParamData_OptionCore;

class KawaigariParamOptionCoreManager
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(KawaigariParamOptionCoreManager);
public:
  
  KawaigariParamOptionCoreManager(gfl2::heap::HeapBase *dataHeap,gfl2::heap::HeapBase *workHeap,gfl2::fs::AsyncFileManager *fileManager);
  ~KawaigariParamOptionCoreManager(void);

  int GetAdjustLookAngle(int monsNo,int formNo,int sex); // 首調整角度 (DEGREES)

  bool IsLoaded(void);
  bool Finalize(void);

private:

  KWParamData_OptionCore* GetData(u32 index);

  gfl2::fs::AsyncFileManager *mFileManager;
  void *dataBuf;
  u32 dataCount;
  bool isLoaded;
};
  
GFL_NAMESPACE_END(PokeTool)


#endif //__KAWAIGARI_PARAM_OPTIONCORE_MANAGER_H__

