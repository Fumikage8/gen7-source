//======================================================================
/**
 * @file	KawaigariParamFaceManager.h
 * @brief	niji Kawaigari Face Parameter Manager
 * @author	PETE
 * @data	16/01/19
 */
//======================================================================
#if !defined( __KAWAIGARI_PARAM_FACE_MANAGER_H__ )
#define __KAWAIGARI_PARAM_FACE_MANAGER_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_math.h>
#include <util/include/gfl2_GLHeapAllocator.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <fs/include/gfl2_Fs.h>

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

enum KW_DATA_TYPE_ID_FACE
{
  KW_DTYPE_UNKOWN = 0,
  KW_DTYPE_ID,  
  KW_DTYPE_FACE_EYE_DEFAULT,
  KW_DTYPE_FACE_EYE_BLINK,
  KW_DTYPE_FACE_MOUTH_DEFAULT,
  KW_DTYPE_FACE_MOUTH_LOOK,
  KW_DTYPE_FACE_MOUTH_LISTEN,
  KW_DTYPE_FACE_EYE_TOUCHU,
  KW_DTYPE_FACE_MOUTH_TOUCHU,
  KW_DTYPE_FACE_EYE_TOUCHD,
  KW_DTYPE_FACE_MOUTH_TOUCHD1,
  KW_DTYPE_FACE_MOUTH_TOUCHD2,
  KW_DTYPE_FACE_EYE_TOUCH_LIKE,
  KW_DTYPE_FACE_MOUTH_TOUCH_LIKE,
  KW_DTYPE_FACE_EYE_TOUCH_HATE,
  KW_DTYPE_FACE_MOUTH_TOUCH_HATE,
  KW_DTYPE_FACE_EYE_FULL,
  KW_DTYPE_FACE_MOUTH_FULL,
  KW_DTYPE_FACE_EYE_ELECTRIC,
  KW_DTYPE_FACE_MOUTH_ELECTRIC,
  KW_DTYPE_FACE_EYE_ICE,
  KW_DTYPE_FACE_MOUTH_ICE,  
  KW_DTYPE_FACE_EYE_EYECLOSE,
  KW_DTYPE_FACE_MAX,
};

typedef struct  
{
  u16 idNum;
  u8 form;
  u8 sex;
  u8 faceMouthData[KW_DTYPE_FACE_MAX-KW_DTYPE_FACE_EYE_DEFAULT];  
} KWParamData_Face;

class KawaigariParamFaceManager
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(KawaigariParamFaceManager);
public:
  
  KawaigariParamFaceManager(gfl2::heap::HeapBase *dataHeap,gfl2::heap::HeapBase *workHeap,gfl2::fs::AsyncFileManager *fileManager);
  ~KawaigariParamFaceManager(void);

  int GetFaceAnimationIndex(int monsNo,int formNo,int sex,int type,int defaultVal=-1);
  
  bool IsLoaded(void);
  bool Finalize(void);

#if PM_DEBUG
  void LoadHIO(char *path,gfl2::heap::HeapBase *dataHeap);
#endif

private:

  KWParamData_Face* GetData(u32 index);
  KWParamData_Face* GetData(int monsNo,int formNo,int sex);
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


#endif //__KAWAIGARI_PARAM_FACE_MANAGER_H__

