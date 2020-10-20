//======================================================================
/**
 * @file ResourceManager.h
 * @date 2016/09/12 16:39:59
 * @author fang_yicheng
 * @brief リソースマネージャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __RESOURCE_MANAGER_H_INCLUDED__
#define __RESOURCE_MANAGER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <ctype.h>

#include <System/include/HeapDefine.h>
#include <AppLib/include/Util/app_util_heap.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/GameProcManager.h>

#include <arc_def_index/arc_def.h>
#include <arc_index/mantain_chara.gaix>
#include <arc_index/mantain_field.gaix>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Resource)

class ResourceManager
{
  GFL_FORBID_COPY_AND_ASSIGN(ResourceManager);

public:

  ResourceManager();
  virtual ~ResourceManager();

  void Initialize(gfl2::heap::HeapBase* pHeap, s32 arcId, u32 datNum, u32 isCompressed = 0, u32 align = 128);
  void Terminate();

  bool LoadResource();


  void LoadResourceSync();

  void ReleaseResource();

  s32  GetResourceNum();

  void* GetResourceData(s32 resId) const;
  void** GetResourceDataList();

protected:

  virtual void OnResourceLoadFinished() {};

  void StartOpenArcFile();
  bool IsArcFileOpened();

  void StartLoadResource();
  bool IsResourceLoaded();

  void StartCloseArcFile();
  bool IsArcFileClosed();
  
protected:

  enum SEQ
  {
    SEQ_NONE = -1,
    SEQ_START = 0,
    SEQ_OPEN,
    SEQ_LOAD,
    SEQ_CLOSE,
    SEQ_END
  };
  SEQ m_seq;

  gfl2::heap::HeapBase*   m_pHeap;
  s32   m_arcId;
  u32   m_datNum;
  bool  m_isCompressed;
  u32   m_align;

  gfl2::fs::AsyncFileManager* m_pFileManager;

  void** m_ppResourceData;

};

GFL_NAMESPACE_END(Resource)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __RESOURCE_MANAGER_H_INCLUDED__
