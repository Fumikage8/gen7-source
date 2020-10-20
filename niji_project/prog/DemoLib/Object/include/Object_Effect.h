//======================================================================
/**
 * @file	Object_Effect.h
 * @brief	オブジェクト Effect
 * @author	ariizumi
 * @data	15/03/25
 */
//======================================================================
#if !defined( __OBJECT_EFFECT_H__ )
#define __OBJECT_EFFECT_H__
#pragma once


#include <fs/include/gfl2_Fs.h>
#include <math/include/gfl2_math.h>
#include <heap/include/gfl2_Heap.h>
#include <Effect/include/gfl2_EffectSystem.h>

#include <DemoLib/Object/include/Object.h>


GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Obj)

class ObjectRenderPipeLine;

class Object_Effect :public Object
{
public:
  struct LOAD_PARAM
  {
    gfl2::fs::ArcFile::ARCID    arcId;
    gfl2::fs::ArcFile::ARCDATID dataId;
    bool                        isComp;
    gfl2::heap::HeapBase        *dataHeap;
    gfl2::heap::HeapBase        *workHeap;
  #if PM_DEBUG
    char *dataFilePath;
  #endif
    LOAD_PARAM()
      :arcId(gfl2::fs::ArcFile::ARCID_NULL)
      ,dataId(gfl2::fs::ArcFile::ARCDATID_NULL)
      ,isComp(false)
#if PM_DEBUG
      ,dataFilePath(NULL)
#endif
      {}
  };


  //コンストラクタ
  Object_Effect(gfl2::Effect::System *effSys);
  //デストラクタ
  virtual ~Object_Effect();

  //モデル生成
  virtual void StartLoad(gfl2::fs::AsyncFileManager *fileMng, const LOAD_PARAM &param);
#if PM_DEBUG
  virtual void StartLoad_Debug(gfl2::fs::AsyncFileManager *fileMng, const LOAD_PARAM &param);
#endif
  virtual bool IsFinishLoad(void);
  //StartLoadで呼んだリソースで作成
  virtual void Create( gfl2::Effect::GFHeap *effHeap , int resIdx );
  //外からバッファを渡して作成
  virtual void CreateBuffer(void *buf,gfl2::Effect::GFHeap *effHeap , int resIdx );
  //外でEffectSystemにEntryResourceしてから作成
  virtual void CreateResIdx(gfl2::Effect::GFHeap *effHeap , int resIdx );
  virtual void Delete(void);

  //ObjectRenderPipeLine経由で追加する(Effectは経由しません。)
  virtual void AddRenderPath(int mGroupId = 0);
  virtual void RemoveRenderPath(void);
  virtual bool IsAddRenderPath(void){return mIsAddRenderPath;}
  //継承
  virtual void SetVisible(const bool flg);

  void StopParticle(void);

  bool IsAlive(void);

  //リソースインデックスの取得
  int GetResourceIndex(void)const{return mResIdx;}

  void ForceUpdate(int step);
  void UpdatePosition(void);

protected:
  //IsEnableはチェック済み。
  //毎フレーム呼ばれる
  virtual void UpdateAlways(int step=1);
  //IsUpdateが立っていたら来る関数。
  //IsEnableはチェック済みなので座標更新はこちらで。
  virtual void UpdateCore(int step=1);

  gfl2::fs::AsyncFileManager *mFileManager;

  gfl2::Effect::System *mEffectSystem;
  gfl2::Effect::GFHeap *mEffectHeap;

  gfl2::Effect::Handle **mEffectHandle;

  void *mEffRes;
  int  mResIdx;
  int  mEffNum;

  bool mIsEntryResource;  //EntryResourceをしたか？
  bool mIsAddRenderPath;
  int mGroupId;

#if PM_DEBUG
  bool mIsDebugLoad;
#endif
};

GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)


#endif //__OBJECT_GFMDL_H__