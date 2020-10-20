//======================================================================
/**
 * @file	KawaigariParamCareCoreManager.h
 * @brief	niji Kawaigari Care Core Parameter Manager
 * @author	PETE
 * @data	16/01/05
 */
//======================================================================
#if !defined( __KAWAIGARI_PARAM_CARECORE_MANAGER_H__ )
#define __KAWAIGARI_PARAM_CARECORE_MANAGER_H__
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
  u8 jointCount;
  u8 typeCanUse;
} KWParamData_CareCore;

class KawaigariParamCareCoreManager
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(KawaigariParamCareCoreManager);
public:
  
  KawaigariParamCareCoreManager(gfl2::heap::HeapBase *dataHeap,gfl2::heap::HeapBase *workHeap,gfl2::fs::AsyncFileManager *fileManager);
  ~KawaigariParamCareCoreManager(void);

  int GetJointCount(int monsNo,int formNo,int sex); // お手入れの可能ジョイント数

  bool TypeEnabled_Dirt(int monsNo,int formNo,int sex); // 汚れタイプ可能？
  bool TypeEnabled_Dust(int monsNo,int formNo,int sex); // ホコリタイプ可能？
  bool TypeEnabled_Hair(int monsNo,int formNo,int sex); // 毛タイプ可能？
  bool TypeEnabled_Water(int monsNo,int formNo,int sex); // 水タイプ可能？

  bool IsLoaded(void);
  bool Finalize(void);

private:

  int GetIndex(int monsNo,int formNo,int sex);
  KWParamData_CareCore* GetData(u32 index);

  gfl2::fs::AsyncFileManager *mFileManager;
  void *dataBuf;
  u32 dataCount;
  bool isLoaded;
};
  
GFL_NAMESPACE_END(PokeTool)


#endif //__KAWAIGARI_PARAM_CARECORE_MANAGER_H__

