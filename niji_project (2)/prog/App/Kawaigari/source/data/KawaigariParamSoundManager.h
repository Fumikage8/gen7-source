//======================================================================
/**
 * @file	KawaigariParamSoundManager.h
 * @brief	niji Kawaigari Sound Parameter Manager
 * @author	PETE
 * @data	16/01/22
 */
//======================================================================
#if !defined( __KAWAIGARI_PARAM_SOUND_MANAGER_H__ )
#define __KAWAIGARI_PARAM_SOUND_MANAGER_H__
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
  KW_SMACK,KW_TOUCH_UPPER,KW_TOUCH_LOWER,KW_APPEAR_DO_UP,KW_APPEAR_LOWER,KW_APPEAR_UPPER,
  KW_APPEAR_LEFT,KW_APPEAR_RIGHT,KW_APPEAR_TURN,KW_DELIGHT1,KW_DELIGHT2,KW_DELIGHT3,KW_NO_IDEA,
  KW_HABIT1,KW_HABIT2,KW_HABIT3,KW_HABIT4,KW_CARE1,KW_CARE2,KW_ANGER,KW_HATE,KW_LONELY,KW_ROW,
  KW_BECKON,KW_AWAKE,KW_DRAW,KW_VOICE,KW_FALLSWEET,KW_HEART2,KW_THINK,KW_TAP,KW_HIT,KW_FACE_START,
  KW_FACE_END,KW_FACEGAME,KW_STROKE,KW_ELSHOCK,KW_JUNK,KW_ICE,KW_WATER,KW_SEStart01,KW_SEStart02,KW_SEStart03,KW_SEStart04,
  SOUND_MAX,
};

typedef struct  
{
  u16 idNum;    
  u8 form;
  u8 sex;
  u32 soundID[SOUND_MAX];
} KWParamData_Sound;

class KawaigariParamSoundManager
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(KawaigariParamSoundManager);
public:
  
  KawaigariParamSoundManager(gfl2::heap::HeapBase *dataHeap,gfl2::heap::HeapBase *workHeap,gfl2::fs::AsyncFileManager *fileManager);
  ~KawaigariParamSoundManager(void);

  int GetSoundID(int monsNo,int formNo,int sex,int type,int defaultVal=-1);
  
  bool IsLoaded(void);
  bool Finalize(void);

#if PM_DEBUG
  void LoadHIO(char *path,gfl2::heap::HeapBase *dataHeap);
#endif

private:

  KWParamData_Sound* GetData(u32 index);
  KWParamData_Sound* GetData(int monsNo,int formNo,int sex);
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


#endif //__KAWAIGARI_PARAM_SOUND_MANAGER_H__

