//======================================================================
/**
 * @file	KawaigariParamMotionAffectionManager.h
 * @brief	niji Kawaigari Motion Affection Parameter Manager
 * @author	PETE
 * @data	16/01/22
 */
//======================================================================
#if !defined( __KAWAIGARI_PARAM_MOTIONAFFECTION_MANAGER_H__ )
#define __KAWAIGARI_PARAM_MOTIONAFFECTION_MANAGER_H__
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
  MOT_IDLE,MOT_BACK_PIKU,MOT_BACK_TURN,MOT_BACK_TURN_SMILE,MOT_ROW_BEGIN,MOT_ROW,MOT_ROW_WAKE,MOT_SLEEP,MOT_SLEEP_WAKE,MOT_HATE,MOT_NO_IDEA,MOT_DELIGHT1,MOT_DELIGHT2,MOT_DELIGHT3,MOT_HABIT1,MOT_HABIT2,MOT_HABIT3,MOT_HABIT4,MOT_LONELY,MOT_CARE1,MOT_CARE2,MOT_ANGER,MOT_BITE_BEGIN,MOT_BITE_LOOP,MOT_BITE_END,MOT_TOUCH,
  MOT_AFFECTION_MAX,
};

typedef struct  
{
  u16 idNum;    
  u8 form;
  u8 sex;
  u8 loveAmount[MOT_AFFECTION_MAX];
} KWParamData_MotionAffection;

class KawaigariParamMotionAffectionManager
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(KawaigariParamMotionAffectionManager);
public:
  
  KawaigariParamMotionAffectionManager(gfl2::heap::HeapBase *dataHeap,gfl2::heap::HeapBase *workHeap,gfl2::fs::AsyncFileManager *fileManager);
  ~KawaigariParamMotionAffectionManager(void);

  int GetAffectionRequired(int monsNo,int formNo,int sex,int type,int defaultVal=0);
  
  bool IsLoaded(void);
  bool Finalize(void);

#if PM_DEBUG
  void LoadHIO(char *path,gfl2::heap::HeapBase *dataHeap);
#endif

private:

  KWParamData_MotionAffection* GetData(u32 index);
  KWParamData_MotionAffection* GetData(int monsNo,int formNo,int sex);
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


#endif //__KAWAIGARI_PARAM_MOTIONAFFECTION_MANAGER_H__

